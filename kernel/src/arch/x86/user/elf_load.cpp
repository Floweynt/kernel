#include <bits/mathhelper.h>
#include <cstddef>
#include <cstdint>
#include <elf/elf.h>
#include <gdt/gdt.h>
#include <misc/cast.h>
#include <misc/kassert.h>
#include <misc/pointer.h>
#include <mm/mm.h>
#include <mm/paging/paging.h>
#include <mm/paging/paging_entries.h>
#include <process/context.h>
#include <process/process.h>

namespace user
{
    namespace
    {
        auto check_ident(void* elf_data) -> bool
        {
            elf::elf_ident* header = as_ptr(elf_data);

            if (header->magic[0] != 0x7f || header->magic[1] != 'E' || header->magic[2] != 'L' || header->magic[3] != 'F')
            {
                return false;
            }

            // I dont support loading 32-bit binaries yet
            if (header->wordsize != elf::EI_CLASS_64)
            {
                return false;
            }

            if (header->endianness != elf::EI_DATA_LITTLE)
            {
                return false;
            }

            if (header->version != elf::EI_VERSION_CURRENT)
            {
                return false;
            }

            // we just assume it's sysv abi, since we dont have our own special ABI
            if (header->abi != elf::EI_OSABI_SYSV)
            {
                return false;
            }

            if (header->abi_version != 0)
            {
                return false;
            }

            return true;
        }

        auto validate_elf(elf::elf64_header* header) -> bool
        {
            expect(header->type == elf::ET_EXEC, "bad elf type");
            if (header->machine != elf::ELF_MACH_AMD64)
            {
                return false;
            }

            if (header->version != 1)
            {
                return false;
            }

            if (header->ph_ent_size != sizeof(elf::elf64_program_header) || header->eh_size != sizeof(elf::elf64_header))
            {
                return false;
            }

            elf::elf64_program_header* segments = cast_ptr(ptr_off(header, header->ph_off));

            for (std::size_t i = 0; i < header->ph_num; i++)
            {
                auto& segment = segments[i];
                switch (segment.type)
                {
                case elf::PT_NULL:
                case elf::PT_LOAD:
                case elf::PT_NOTE:
                    // okay
                    break;
                default:
                    return false;
                }

                if (segment.filesz > segment.memsz)
                {
                    return false;
                }
            }

            return true;
        }
    } // namespace

    auto load_elf(void* elf_data, proc::thread& thread) -> bool
    {
        if (!check_ident(elf_data))
        {
            return false;
        }

        elf::elf64_header* header = as_ptr(elf_data);
        if (!validate_elf(header))
        {
            return false;
        }

        // setup state
        auto builder = proc::context_builder(proc::context_builder::USER, header->entry);
        paging::page_table_entry* table = as_ptr(expect_nonnull(mm::pmm_allocate_clean(), ":3"));
        builder.set_cr3(as_uptr(table));

        elf::elf64_program_header* segments = cast_ptr(ptr_off(header, header->ph_off));

        for (std::size_t i = 0; i < header->ph_num; i++)
        {
            auto& segment = segments[i];
            switch (segment.type)
            {
            case elf::PT_LOAD: {
                if (!segment.memsz)
                {
                    break;
                }

                // TODO: don't copy, just add the mapping!
                // we have to map from div_rounddown(vaddr, page_size) to div_roundup(vaddr + memsz, page_size)

                paging::page_prop prop{.rw = bool(segment.flags & 0x2), .us = true, .x = bool(segment.flags & 0x4)};

                void* current_page = nullptr;

                for (std::size_t addr = segment.vaddr, offset = segment.offset; addr < segment.vaddr + segment.memsz; addr++, offset++)
                {
                    if (addr % paging::PAGE_SMALL_SIZE == 0 || current_page == nullptr)
                    {
                        current_page = mm::pmm_allocate_clean();
                        paging::map_page_for(table, paging::SMALL, addr, mm::make_physical(current_page), prop, true);
                    }

                    std::uint8_t current_byte = (offset > segment.offset + segment.filesz) ? 0 : *as_ptr<std::uint8_t>(ptr_off(elf_data, offset));
                    *as_ptr<std::uint8_t>(ptr_off(current_page, addr % paging::PAGE_SMALL_SIZE)) = current_byte;
                }
            }
                // these aren't loaded sections, just ignore them
            case elf::PT_NULL:
            case elf::PT_NOTE:
                break;
            default:
                return false;
            }
        }

        builder.set_flag(cpuflags::IF);
        // builder.set_stack(paging::PAGE_SMALL_SIZE);

        thread.ctx = builder.build();

        paging::copy_kernel_page_tables(as_ptr(table), smp::core_local::get().pagemap);
        return true;
    }
} // namespace user

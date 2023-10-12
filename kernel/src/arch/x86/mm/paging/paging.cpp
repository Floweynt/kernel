#include <asm/asm_cpp.h>
#include <bits/utils.h>
#include <cstring>
#include <debug/debug.h>
#include <kinit/limine.h>
#include <misc/cast.h>
#include <misc/kassert.h>
#include <mm/mm.h>
#include <mm/paging/paging.h>
#include <mm/paging/paging_entries.h>
#include <smp/smp.h>
#include <sync/spinlock.h>

#define GET_VIRTUAL_POS(n) get_bits<(4 - n) * 9 + 12, (4 - n) * 9 + 20>(VIRT_LOAD_POSITION)

namespace paging
{
    void install() { write_cr3(mm::make_physical(smp::core_local::get().pagemap)); }

    static inline constexpr std::uint64_t type_to_align[] = {
        0xfff,
        0x1fffff,
        0x3fffffff,
    };

    namespace
    {
        lock::spinlock paging_global_lock;
    }

    template <void* (*Fn)()>
    auto do_map_page_for(page_table_entry* table, page_type type, std::uintptr_t virtual_addr, std::uintptr_t physical_addr, page_prop prop,
                         bool overwrite) -> bool
    {
        virtual_addr &= ~type_to_align[type];
        physical_addr &= ~type_to_align[type];

        page_table_entry* current_entry = table;
        for (int i = 0; i < (3 - type); i++)
        {
            auto index = get_page_entry(virtual_addr, i);
            std::uint64_t& entry = current_entry[index];
            if (!entry)
            {
                auto* mem = Fn();
                if (mem == nullptr)
                {
                    debug::panic("cannot allocate physical memory for paging");
                }

                std::memset(mem, 0, PAGE_SMALL_SIZE);
                entry = make_page_pointer(mm::make_physical(mem), {
                                                                      .rw = true, // since x86 ands all permission bits, we want this to be true!
                                                                      .us = true,
                                                                      .x = true,
                                                                  });
            }

            current_entry = mm::make_virtual<page_table_entry>(current_entry[index] & MASK_TABLE_POINTER);
        }
        // write last entry
        current_entry += get_page_entry(virtual_addr, 3 - type);
        if (*current_entry && !overwrite)
        {
            return false;
        }
        switch (type)
        {
        case SMALL:
            *current_entry = make_page_small(physical_addr, prop);
            break;
        case MEDIUM:
            *current_entry = make_page_medium(physical_addr, prop);
            break;
        case BIG:
            *current_entry = make_page_large(physical_addr, prop);
            break;
        }

        return true;
    }

    auto map_page_for(page_table_entry* table, page_type type, std::uintptr_t virtual_addr, std::uintptr_t physical_addr, page_prop prop,
                      bool overwrite) -> bool
    {
        return do_map_page_for<+[]() { return mm::pmm_allocate(); }>(table, type, virtual_addr, physical_addr, prop, overwrite);
    }

    auto map_page_for_early(page_table_entry* table, page_type type, std::uintptr_t virtual_addr, std::uintptr_t physical_addr, page_prop prop,
                            bool overwrite) -> bool
    {
        return do_map_page_for<mm::pmm_stupid_allocate>(table, type, virtual_addr, physical_addr, prop, overwrite);
    }

    auto request_page(page_type type, std::uintptr_t vaddr, std::uintptr_t paddr, page_prop prop, bool overwrite) -> bool
    {
        lock::spinlock_guard guard(paging_global_lock);
        // obtain pointer to entry
        page_table_entry* current_entry = smp::core_local::get().pagemap;
        if (current_entry == nullptr)
        {
            current_entry = smp::core_local::get().pagemap = as_ptr(mm::pmm_allocate());
        }

        return map_page_for(current_entry, type, vaddr, paddr, prop, overwrite);
    }

    auto request_page_early(page_type type, std::uint64_t vaddr, std::uint64_t paddr, page_prop prop, bool overwrite) -> bool
    {
        lock::spinlock_guard guard(paging_global_lock);
        // obtain pointer to entry
        page_table_entry* current_entry = smp::core_local::get().pagemap;
        if (current_entry == nullptr)
        {
            current_entry = smp::core_local::get().pagemap = as_ptr(mm::pmm_stupid_allocate());
        }

        return map_page_for_early(current_entry, type, vaddr, paddr, prop, overwrite);
    }

    void sync(std::uintptr_t virtual_addr)
    {
        lock::spinlock_guard guard(paging_global_lock);
        std::uint16_t index = paging::get_page_entry<3>(virtual_addr);
        std::uint64_t value = smp::core_local::get().pagemap[index];

        for (std::size_t i = 0; i < boot_resource::instance().core_count(); i++)
        {
            smp::core_local::get(i).pagemap[index] = value;
        }
    }

    template <decltype(map_hhdm_page) Fn>
    void do_map_hhdm_section(std::uintptr_t addr, std::size_t length, paging::page_prop prop, bool overwrite)
    {
        // align everything
        length = length + addr % PAGE_SMALL_SIZE;
        addr = addr & ~(PAGE_SMALL_SIZE - 1);

        std::int64_t pages = as_signed(std::div_roundup(length, PAGE_SMALL_SIZE));

        while (addr % paging::PAGE_MEDIUM_SIZE)
        {
            if (pages-- == 0)
            {
                return;
            }
            Fn(paging::page_type::SMALL, addr, prop, overwrite);
            addr += paging::PAGE_SMALL_SIZE;
        }

        while ((pages >= as_signed(PAGE_MEDIUM_TO_SMALL_RATIO)) && (addr % paging::PAGE_LARGE_SIZE))
        {
            Fn(paging::page_type::MEDIUM, addr, prop, overwrite);
            addr += paging::PAGE_MEDIUM_SIZE;
            pages -= PAGE_MEDIUM_TO_SMALL_RATIO;
        }

        // okay, this is aligned...
        while (pages >= as_signed(PAGE_LARGE_TO_SMALL_RATIO))
        {
            Fn(paging::page_type::BIG, addr, prop, overwrite);
            addr += paging::PAGE_LARGE_SIZE;
            pages -= PAGE_LARGE_TO_SMALL_RATIO;
        }

        while (pages >= as_signed(PAGE_MEDIUM_TO_SMALL_RATIO))
        {
            Fn(paging::page_type::MEDIUM, addr, prop, overwrite);
            addr += paging::PAGE_MEDIUM_SIZE;
            pages -= PAGE_MEDIUM_TO_SMALL_RATIO;
        }

        while (pages--)
        {
            Fn(paging::page_type::SMALL, addr, prop, overwrite);
            addr += paging::PAGE_SMALL_SIZE;
        }
    }

    void map_hhdm_section(std::uint64_t addr, std::uint64_t len, paging::page_prop prop, bool overwrite)
    {
        return do_map_hhdm_section<map_hhdm_page>(addr, len, prop, overwrite);
    }

    void map_hhdm_section_early(std::uint64_t addr, std::uint64_t len, paging::page_prop prop, bool overwrite)
    {
        return do_map_hhdm_section<map_hhdm_page_early>(addr, len, prop, overwrite);
    }

    void sync_page_tables(std::size_t dest_core, std::size_t src_core)
    {
        copy_kernel_page_tables(smp::core_local::get(dest_core).pagemap, smp::core_local::get(src_core).pagemap);
    }

    void copy_kernel_page_tables(page_table_entry* dest, const page_table_entry* src)
    {
        std::memcpy(dest + 256, src + 256, 256 * sizeof(paging::page_table_entry));
    }
} // namespace paging

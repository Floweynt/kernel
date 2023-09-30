#pragma once

#include <bits/mathhelper.h>
#include <config.h>
#include <cstddef>
#include <debug/debug.h>
#include <kinit/boot_resource.h>
#include <kinit/limine.h>
#include <mm/malloc.h>
#include <mm/mm.h>
#include <mm/paging/paging.h>
#include <mm/paging/paging_entries.h>
namespace mm
{
    namespace
    {
        std::size_t stupid_pmm_current_index = 0;
        std::size_t stupid_pmm_offset = 0;
        std::size_t allocations;
    } // namespace

    auto pmm_stupid_allocate() -> void*
    {
        allocations++;
        auto& entries = boot_resource::instance().memmap();
        auto& entry_count = boot_resource::instance().memmap_length();

        while (entries[stupid_pmm_current_index].type != LIMINE_MEMMAP_USABLE && stupid_pmm_current_index < entry_count)
        {
            stupid_pmm_current_index++;
        }

        if (stupid_pmm_current_index >= entry_count)
        {
            debug::panic("cannot allocate physical memory for paging");
        }

        const auto& entry = entries[stupid_pmm_current_index];

        std::size_t max_off = entry.length / paging::PAGE_SMALL_SIZE;
        auto ptr = entry.base + stupid_pmm_offset * paging::PAGE_SMALL_SIZE;
        stupid_pmm_offset++;
        if (stupid_pmm_offset >= max_off)
        {
            stupid_pmm_offset = 0;
            stupid_pmm_current_index++;
        }

        return mm::make_virtual<void>(ptr);
    }

    void init()
    {
        // first, we set up paging
        wrmsr(msr::IA32_EFER, rdmsr(msr::IA32_EFER) | (1 << 11));
        // wrmsr(msr::IA32_PAT, 0x706050403020100);

        std::size_t kernel_pages = std::div_roundup(boot_resource::instance().kernel_size(), paging::PAGE_SMALL_SIZE);

        for (std::size_t i = 0; i < kernel_pages + 0x10; i++)
        {
            std::uint64_t vaddr = config::get_val<"mmap.start.kernel"> + i * paging::PAGE_SMALL_SIZE;
            paging::request_page_early(paging::SMALL, vaddr, mm::make_physical_kern(vaddr), {.x = true}, true);
        }

        for (std::size_t i = 0; i < 10; i++)
        {
            std::uint64_t vaddr = 0xffffffff90000000 + i * paging::PAGE_SMALL_SIZE;
            paging::request_page_early(paging::SMALL, vaddr, mm::make_physical_kern(vaddr), {.x = true}, true);
        }

        boot_resource::instance().iterate_mmap([](const limine_memmap_entry& e) {
            paging::page_prop flags;

            switch (e.type)
            {
            case LIMINE_MEMMAP_USABLE:
            case LIMINE_MEMMAP_KERNEL_AND_MODULES:
            case LIMINE_MEMMAP_FRAMEBUFFER:
            case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
            case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
                break;
            case LIMINE_MEMMAP_RESERVED:
            case LIMINE_MEMMAP_BAD_MEMORY:
            case LIMINE_MEMMAP_ACPI_NVS:
                flags.rw = false;
            default:
                return;
            }

            paging::map_hhdm_section_early(e.base, e.length, flags);
        });

        for (std::size_t i = 0; i < config::get_val<"preallocate-pages">; i++)
        {
            void* ptr = mm::pmm_stupid_allocate();
            if (ptr == nullptr)
            {
                debug::panic("cannot get memory for heap");
            }
            paging::request_page_early(paging::page_type::SMALL, config::get_val<"mmap.start.heap"> + i * paging::PAGE_SMALL_SIZE,
                                       mm::make_physical(ptr));
        }

        // okay, now it's time to set up the PFN table
        // two phase: we first map everything, then actually setup PFN
        void* current_page = nullptr;
        boot_resource::instance().iterate_mmap([&](const limine_memmap_entry& e) {
            if (e.type != LIMINE_MEMMAP_USABLE)
            {
                return;
            }

            auto start = e.base;

            // so we need to map from pfn_addr(start) to pfn_addr(start) + (e.length / PAGE_SMALL_SIZE) * sizeof(page_info)

            auto pfn_start = start / paging::PAGE_SMALL_SIZE * sizeof(page_info);
            auto pfn_end = pfn_start + (e.length / paging::PAGE_SMALL_SIZE) * sizeof(page_info);

            std::size_t page_start = pfn_start / paging::PAGE_SMALL_SIZE;
            std::size_t page_end = std::div_roundup(pfn_end, paging::PAGE_SMALL_SIZE);

            for (std::size_t i = page_start; i < page_end; i++)
            {
                if (current_page == nullptr)
                {
                    current_page = pmm_stupid_allocate();
                    if (current_page == nullptr)
                    {
                        debug::panic("failed to allocate physical memory");
                    }
                }

                if (paging::request_page_early(paging::SMALL, i * paging::PAGE_SMALL_SIZE + config::get_val<"mmap.start.pfn">,
                                               make_physical(current_page)))
                {
                    current_page = nullptr;
                }
            }
        });

        // we want to switch to our tables now...
        paging::install();

        // now we set PFN table
        std::size_t mmap_index = 0;
        boot_resource::instance().iterate_mmap([&](const limine_memmap_entry& e) {
            if (e.type != LIMINE_MEMMAP_USABLE)
            {
                return;
            }

            // TODO: ddd this to some used list if condition isn't met?
            if (mmap_index >= stupid_pmm_current_index)
            {
                std::size_t start_index = 0;
                if (mmap_index == stupid_pmm_current_index)
                {
                    start_index = stupid_pmm_offset;
                }

                for (; start_index < e.length / paging::PAGE_SMALL_SIZE; start_index++)
                {
                    new (&get_pfn(e.base + start_index * paging::PAGE_SMALL_SIZE)) page_info();
                    pmm_get_free_list().add_front(&get_pfn(e.base + start_index * paging::PAGE_SMALL_SIZE));
                }
            }

            mmap_index++;
        });

        alloc::init(as_vptr(config::get_val<"mmap.start.heap">), paging::PAGE_SMALL_SIZE * config::get_val<"preallocate-pages">);
    }
} // namespace mm

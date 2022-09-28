#include "paging.h"
#include <asm/asm_cpp.h>
#include <debug/debug.h>
#include <kinit/limine.h>
#include <mm/pmm.h>
#include <smp/smp.h>
#include <sync/spinlock.h>

#define GET_VIRTUAL_POS(n) get_bits<(4 - n) * 9 + 12, (4 - n) * 9 + 20>(VIRT_LOAD_POSITION)

namespace paging
{
    void install() { write_cr3(mm::make_physical(smp::core_local::get().pagemap)); }

    static inline constexpr std::uint64_t type2align[] = {
        0xfff,
        0x1fffff,
        0x3fffffff,
    };

    static lock::spinlock paging_global_lock;

    bool request_page(page_type pt, std::uintptr_t virtual_addr, std::uintptr_t physical_addr, page_prop prop,
                      bool overwrite)
    {
        virtual_addr &= ~type2align[pt];
        physical_addr &= ~type2align[pt];

        lock::spinlock_guard g(paging_global_lock);
        // obtain pointer to entry
        page_table_entry* current_entry = smp::core_local::get().pagemap;
        if (current_entry == nullptr)
            current_entry = smp::core_local::get().pagemap = (page_table_entry*)mm::pmm_allocate();

        for (int i = 0; i < (3 - pt); i++)
        {
            std::uint64_t& e = current_entry[get_page_entry(virtual_addr, i)];
            if (!e)
            {
                auto r = mm::pmm_allocate();
                if (r == 0)
                    debug::panic("cannot allocate physical memory for paging");
                e = make_page_pointer(mm::make_physical(r), prop);
            }

            current_entry =
                mm::make_virtual<page_table_entry>(current_entry[get_page_entry(virtual_addr, i)] & MASK_TABLE_POINTER);
        }
        // writelast entry
        current_entry += get_page_entry(virtual_addr, 3 - pt);
        if (*current_entry && !overwrite)
            return false;
        switch (pt)
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

    void sync(std::uintptr_t virtual_addr)
    {
        lock::spinlock_guard g(paging_global_lock);
        std::uint16_t index = paging::get_page_entry<3>(virtual_addr);
        std::uint64_t value = smp::core_local::get().pagemap[index];

        for (std::size_t i = 0; i < boot_resource::instance().core_count(); i++)
            smp::core_local::get(i).pagemap[index] = value;
    }

    void map_section(std::uintptr_t addr, std::size_t length, paging::page_prop prop)
    {
        std::int64_t pages = length / PAGE_SMALL_SIZE;

        while (addr % paging::PAGE_MEDIUM_SIZE)
        {
            if (pages-- == 0)
                return;
            paging::map_hhdm_phys(paging::page_type::SMALL, addr, prop);
            addr += paging::PAGE_SMALL_SIZE;
        }

        while ((pages >= 0x200) && (addr % paging::PAGE_LARGE_SIZE))
        {
            paging::map_hhdm_phys(paging::page_type::MEDIUM, addr, prop);
            addr += paging::PAGE_MEDIUM_SIZE;
            pages -= 0x200;
        }

        // okay, this is aligned...
        while (pages >= 0x40000)
        {
            paging::map_hhdm_phys(paging::page_type::BIG, addr, prop);
            addr += paging::PAGE_LARGE_SIZE;
            pages -= 0x40000;
        }

        while (pages >= 0x200)
        {
            paging::map_hhdm_phys(paging::page_type::MEDIUM, addr, prop);
            addr += paging::PAGE_MEDIUM_SIZE;
            pages -= 0x200;
        }

        while (pages--)
        {
            paging::map_hhdm_phys(paging::page_type::SMALL, addr, prop);
            addr += paging::PAGE_SMALL_SIZE;
        }
    }

    void init()
    {
        wrmsr(msr::IA32_EFER, rdmsr(msr::IA32_EFER) | (1 << 11));
        // wrmsr(msr::IA32_PAT, 0x706050403020100);

        std::size_t kernel_pages = std::div_roundup(boot_resource::instance().kernel_size(), paging::PAGE_SMALL_SIZE);

        for (std::size_t i = 0; i < kernel_pages + 10; i++)
        {
            std::uint64_t vaddr = 0xffffffff80000000 + i * paging::PAGE_SMALL_SIZE;
            paging::request_page(paging::SMALL, vaddr, mm::make_physical_kern(vaddr), {.x = true}, true);
        }

        for (std::size_t i = 0; i < 10; i++)
        {
            std::uint64_t vaddr = 0xffffffff90000000 + i * paging::PAGE_SMALL_SIZE;
            paging::request_page(paging::SMALL, vaddr, mm::make_physical_kern(vaddr), {.x = true}, true);
        }

        boot_resource::instance().iterate_mmap([](const limine_memmap_entry& e) {
            paging::page_prop flags;

            switch (e.type)
            {
            case LIMINE_MEMMAP_USABLE:
            case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
            case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            case LIMINE_MEMMAP_KERNEL_AND_MODULES:
            case LIMINE_MEMMAP_FRAMEBUFFER:
                break;
            case LIMINE_MEMMAP_RESERVED:
            case LIMINE_MEMMAP_BAD_MEMORY:
            case LIMINE_MEMMAP_ACPI_NVS:
                flags.rw = false;
                break;
            default:
                return;
            }

            paging::map_section(e.base, e.length, flags);
        });

        for (std::size_t i = 0; i < config::get_val<"preallocate-pages">; i++)
        {
            void* d;
            if (!(d = mm::pmm_allocate()))
                debug::panic("cannot get memory for heap");
            paging::request_page(paging::page_type::SMALL, config::get_val<"mmap.start.heap"> + i * PAGE_SMALL_SIZE,
                                 mm::make_physical(d));
        }

        paging::install();
    }

    void sync_page_tables(std::size_t dest_core, std::size_t src_core)
    {
        std::memcpy(smp::core_local::get(dest_core).pagemap + 256, smp::core_local::get(src_core).pagemap + 256,
                    256 * sizeof(paging::page_table_entry));
    }

} // namespace paging

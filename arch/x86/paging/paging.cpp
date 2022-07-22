#include "paging.h"
#include <asm/asm_cpp.h>
#include <mm/pmm.h>
#include <panic.h>
#include <smp/smp.h>
#include <sync/spinlock.h>

#define GET_VIRTUAL_POS(n) get_bits<(4 - n) * 9 + 12, (4 - n) * 9 + 20>(VIRT_LOAD_POSITION)

namespace paging
{
    void install() { write_cr3(mm::make_physical(smp::core_local::get().pagemap)); }

    static inline constexpr uint64_t type2align[] = {
        0xfff,
        0x1fffff,
        0x3fffffff,
    };

    static lock::spinlock l;

    bool request_page(page_type pt, uint64_t virtual_addr, uint64_t physical_address, page_prop flags, bool overwrite)
    {
        virtual_addr &= ~type2align[pt];
        physical_address &= ~type2align[pt];

        lock::spinlock_guard g(l);
        // obtain pointer to entry
        page_table_entry* current_ent = smp::core_local::get().pagemap;
        if (current_ent == nullptr)
            current_ent = smp::core_local::get().pagemap = (page_table_entry*)mm::pmm_allocate();

        for (int i = 0; i < (3 - pt); i++)
        {
            uint64_t& e = current_ent[get_page_entry(virtual_addr, i)];
            if (!e)
            {
                auto r = mm::pmm_allocate();
                if (r == nullptr)
                    std::panic("cannot allocate physical memory for paging");
                std::memset(r, 0, 4096);
                e = make_page_pointer(mm::make_physical(r), flags);
            }

            current_ent =
                mm::make_virtual<page_table_entry>(current_ent[get_page_entry(virtual_addr, i)] & MASK_TABLE_POINTER);
        }
        // writelast entry
        current_ent += get_page_entry(virtual_addr, 3 - pt);
        if (*current_ent && !overwrite)
            return false;
        switch (pt)
        {
        case SMALL:
            *current_ent = make_page_small(physical_address, flags);
            break;
        case MEDIUM:
            *current_ent = make_page_medium(physical_address, flags);
            break;
        case BIG:
            *current_ent = make_page_large(physical_address, flags);
            break;
        }

        return true;
    }

    void sync(uint64_t virtual_addr)
    {
        lock::spinlock_guard g(l);
        uint16_t index = paging::get_page_entry<3>(virtual_addr);
        uint64_t value = smp::core_local::get().pagemap[index];

        for (std::size_t i = 0; i < boot_resource::instance().core_count(); i++)
            smp::core_local::get(i).pagemap[index] = value;
    }

    void map_section(uint64_t addr, uint64_t _len, paging::page_prop flags)
    {
        int64_t len = _len / PAGE_SMALL_SIZE;

        while (addr % paging::PAGE_MEDIUM_SIZE)
        {
            if (len-- == 0)
                return;
            paging::map_hhdm_phys(paging::page_type::SMALL, addr, flags);
            addr += paging::PAGE_SMALL_SIZE;
        }

        while ((len >= 0x200) && (addr % paging::PAGE_LARGE_SIZE))
        {
            paging::map_hhdm_phys(paging::page_type::MEDIUM, addr, flags);
            addr += paging::PAGE_MEDIUM_SIZE;
            len -= 0x200;
        }

        // okay, this is aligned...
        while (len >= 0x40000)
        {
            paging::map_hhdm_phys(paging::page_type::BIG, addr, flags);
            addr += paging::PAGE_LARGE_SIZE;
            len -= 0x40000;
        }

        while (len >= 0x200)
        {
            paging::map_hhdm_phys(paging::page_type::MEDIUM, addr, flags);
            addr += paging::PAGE_MEDIUM_SIZE;
            len -= 0x200;
        }

        while (len--)
        {
            paging::map_hhdm_phys(paging::page_type::SMALL, addr, flags);
            addr += paging::PAGE_SMALL_SIZE;
        }
    }

    void init()
    {
        wrmsr(msr::IA32_EFER, rdmsr(msr::IA32_EFER) | (1 << 11));
        // wrmsr(msr::IA32_PAT, 0x706050403020100);

        std::size_t kpages = std::detail::div_roundup(boot_resource::instance().kernel_size(), paging::PAGE_SMALL_SIZE);

        // workaround for weird stuff
        for (std::size_t i = 0; i < kpages + 10; i++)
        {
            uint64_t vaddr = 0xffffffff80000000 + i * paging::PAGE_SMALL_SIZE;
            paging::request_page(paging::SMALL, vaddr, mm::make_physical_kern(vaddr), {.x = true}, true);
        }

        boot_resource::instance().iterate_mmap([](const stivale2_mmap_entry& e) {
            paging::page_prop flags;

            switch (e.type)
            {
            case 1:
            case 3:
            case 0x1000:
            case 0x1001:
            case 0x1002:
                break;
            case 2:
            case 4:
            case 5:
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
            if (!(d = mm::pmm_allocate_pre_smp()))
                std::panic("cannot get memory for heap");
            paging::request_page(paging::page_type::SMALL, config::get_val<"mmap.start.heap"> + i * PAGE_SMALL_SIZE,
                                 mm::make_physical(d));
        }

        paging::install();
    }

} // namespace paging

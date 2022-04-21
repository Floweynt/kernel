#include "paging.h"
#include <asm/asm_cpp.h>
#include <mm/pmm.h>
#include <panic.h>
#include <smp/smp.h>

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

    bool request_page(page_type pt, uint64_t virtual_addr, uint64_t physical_address, uint8_t flags, bool overwrite)
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
                e = make_page_pointer(mm::make_physical(r), 0b00000001);
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

        for(std::size_t i = 0; i < boot_resource::instance().core_count(); i++)
            smp::core_local::get(i).pagemap[index] = value;
    }
} // namespace paging

#include "paging.h"
#include <asm/asm_cpp.h>
#include <panic.h>
#include <pmm/pmm.h>

#define GET_VIRTUAL_POS(n) get_bits<(4 - n) * 9 + 12, (4 - n) * 9 + 20>(VIRT_LOAD_POSITION)

namespace paging
{
    static page_table_entry root_table alignas(4096)[512];

    void install() { write_cr3(pmm::make_physical_kern(root_table)); }

    static inline constexpr uint64_t type2align[] = {
        0xfff,
        0x1fffff,
        0x3fffffff,
    };

    bool request_page(page_type pt, uint64_t virtual_addr, uint64_t physical_address, uint8_t flags, bool overwrite)
    {
        // virtual_addr should be aligned
        virtual_addr &= ~type2align[pt];
        physical_address &= ~type2align[pt];

        // obtain pointer to entry
        page_table_entry* current_ent = root_table;
        for (int i = 0; i < (3 - pt); i++)
        {
            uint64_t& e = current_ent[get_page_entry(virtual_addr, i)];
            if (!e)
            {
                auto r = pmm::pmm_allocate();
                if (r == nullptr)
                    std::panic("cannot allocate physical memory for paging");
                std::memset(r, 0, 4096);
                e = make_page_pointer(pmm::make_physical(r), 0b00000001);
            }

            current_ent =
                pmm::make_virtual<page_table_entry>(current_ent[get_page_entry(virtual_addr, i)] & MASK_TABLE_POINTER);
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
} // namespace paging

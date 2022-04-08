#include "paging.h"
#include "utils.h"

#define GET_VIRTUAL_POS(n) get_bits<(4 - n) * 9 + 12, (4 - n) * 9 + 20>(VIRT_LOAD_POSITION)

namespace paging
{
    static page_table_entry root_table alignas(4096) [512];

    bool request_page(page_type pt, void* root, uint64_t virtual_addr, uint64_t physical_address, bool overwrite)
    {
        // virtual_addr should be aligned
        virtual_addr &= ~(4095);
    }
} // namespace paging

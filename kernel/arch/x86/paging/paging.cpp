#include "paging.h"
#include "utils.h"
#include "kinit/kinit.h"

#define GET_VIRTUAL_POS(n) get_bits<(4 - n) * 9 + 12, (4 - n) * 9 + 20>(VIRT_LOAD_POSITION)

namespace paging
{
    page_table PLACE_AT_START root_table = {0};
    page_table PLACE_AT_START kernel_l2 = {0};
    page_table PLACE_AT_START kernel_l3 = {0};

    page_table PLACE_AT_START identity_l2 = {0};
    page_table PLACE_AT_START identity_l3 = {0};

    bool request_page(page_type pt, void* root, uint64_t virtual_addr, uint64_t physical_address, bool override)
    {
        // traverse
    }
}
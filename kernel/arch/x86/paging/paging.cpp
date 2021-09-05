#include "paging.h"
#include "utils.h"
#include "kinit.h"

#define GET_VIRTUAL_POS(n) get_bits<(4 - n) * 9 + 12, (4 - n) * 9 + 20>(VIRT_LOAD_POSITION)

namespace paging
{
    page_table PLACE_AT_START root_table;
    page_table PLACE_AT_START kernel_l2;
    page_table PLACE_AT_START kernel_l3;

    page_table PLACE_AT_START remap_boot_l2;
    page_table PLACE_AT_START remap_boot_l3;
    page_table PLACE_AT_START remap_boot_l4;

    bool request_page(page_type pt, uint64_t virtual_addr, uint64_t physical_address, bool override)
    {

    }
}

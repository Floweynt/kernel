#ifndef __ARCH_X86_PAGING_H__
#define __ARCH_X86_PAGING_H__
#include "paging_entries.h"
#include <bitmanip.h>
namespace paging
{
    enum page_type
    {
        SMALL,  // 4KiB
        MEDIUM, // 2 MiB
        BIG     // 1 GiB
    };

    template <uint8_t t>
    constexpr uint16_t get_page_entry(uint64_t virtual_addr)
    {
        return std::get_bits<(4 - t) * 9 + 12, (4 - t) * 9 + 20>(virtual_addr) >> ((4 - t) * 9 + 12);
    }

    bool request_page(page_type pt, uint64_t virtual_addr, uint64_t physical_address, bool override);

    void* get_page(uint64_t physical_address, uint64_t len);

    using page_table_entry = uint64_t;
} // namespace paging

#endif

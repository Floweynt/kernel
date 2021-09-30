#ifndef __ARCH_X86_PAGING_H__
#define __ARCH_X86_PAGING_H__
#include "utils/utils.h"
#include "paging_entries.h"
#include "utils.h"
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
        return get_bits<(4 - t) * 9 + 12, (4 - t) * 9 + 20>(virtual_addr) >> ((4 - t) * 9 + 12);
    }

    inline void load_cr3(void* virt_addr, uint64_t loaded_addr)
    {
        void* phys_addr = GET_PHYSICAL_POINTER_ADDR(virt_addr, loaded_addr);
        asm volatile("mov %0, %%cr3\n\t" : : "r"(phys_addr));
    }

    bool request_page(page_type pt, uint64_t virtual_addr, uint64_t physical_address, bool override);

    void* get_page(uint64_t physical_address, uint64_t len);

    using page_table = alignas(4096) uint64_t[512];
}

#endif

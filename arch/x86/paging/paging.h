#ifndef __ARCH_X86_PAGING_H__
#define __ARCH_X86_PAGING_H__
#include "paging_entries.h"
#include <bitmanip.h>
#include <mm/mm.h>
namespace paging
{
    enum page_type : uint8_t
    {
        SMALL = 0, // 4KiB
        MEDIUM,    // 2 MiB
        BIG        // 1 GiB
    };
    
    void install();

    template <uint8_t t>
    constexpr uint16_t get_page_entry(uint64_t virtual_addr)
    {
        return std::get_bits<(3 - t) * 9 + 12, (3 - t) * 9 + 20>(virtual_addr) >> ((3 - t) * 9 + 12);
    }

    constexpr uint16_t get_page_entry(uint64_t virtual_addr, uint8_t t)
    {
        return std::get_bits(virtual_addr, (3 - t) * 9 + 12, (3 - t) * 9 + 20) >> ((3 - t) * 9 + 12);
    }

    bool request_page(page_type pt, uint64_t vaddr, uint64_t paddr, page_prop prop = {}, bool overwrite = false);
    
    inline bool map_hhdm_phys(page_type pt, uint64_t paddr, page_prop prop = {}, bool overwrite = false)
    {
        return request_page(pt, mm::make_virtual(paddr), paddr, prop, overwrite);
    }

    inline bool map_hhdm_virt(page_type pt, uint64_t vaddr, page_prop prop = {}, bool overwrite = false)
    {
        return request_page(pt, mm::make_physical(vaddr), vaddr, prop, overwrite);
    }
} // namespace paging

#endif

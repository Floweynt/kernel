#ifndef __ARCH_X86_PAGING_ENTRIES_H__
#define __ARCH_X86_PAGING_ENTRIES_H__

#include "config.h"
#include "utils/utils.h"
#include <cstdint.h>

namespace paging
{
    constexpr uint64_t PRESENT = 0x1;
    constexpr uint64_t RD_WR = 0x2;
    constexpr uint64_t USR_SUP = 0x4;
    constexpr uint64_t ACCESSED = 0x20;
    constexpr uint64_t PAGE_SIZE = 0x80;

    constexpr uint64_t MASK_TABLE_POINTER = 0xFFFFFFFFFF000;
    constexpr uint64_t MASK_TABLE_LARGE = 0xFFFFFC0000000;
    constexpr uint64_t MASK_TABLE_MEDIUM = 0xFFFFFFFE00000;
    constexpr uint64_t MASK_TABLE_SMALL = 0xFFFFFFFFFF000;

    constexpr uint64_t MASK_PROT_KEY = 0x7800000000000000;

    template <typename T>
    uint64_t set_ptr(uint64_t table, T* ptr, uint64_t mask)
    {
        return (table & ~mask) | ((uint64_t)ptr & mask);
    }

    inline uint64_t set_prot_key(uint64_t table, uint8_t prot_key)
    {
        return (table & ~MASK_PROT_KEY) | ((((uint64_t)table) << 59) & MASK_PROT_KEY);
    }
}; // namespace paging

#endif

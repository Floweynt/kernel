#ifndef __ARCH_X86_PAGING_ENTRIES_H__
#define __ARCH_X86_PAGING_ENTRIES_H__

#include "config.h"
#include "utils/utils.h"
#include <cstdint>

namespace paging
{
    constexpr uint64_t MASK_PRESENT = 0x1;
    constexpr uint64_t MASK_RW = 0x2;
    constexpr uint64_t MASK_USER = 0x4;
    constexpr uint64_t MASK_ACCESSED = 0x20;

    constexpr uint64_t MASK_TABLE_POINTER = 0xFFFFFFFFFF000;
    constexpr uint64_t MASK_TABLE_LARGE = 0xFFFFFC0000000;
    constexpr uint64_t MASK_TABLE_MEDIUM = 0xFFFFFFFE00000;
    constexpr uint64_t MASK_TABLE_SMALL = 0xFFFFFFFFFF000;

    constexpr uint64_t MASK_PROT_KEY = 0x7800000000000000;

    template <typename T>
    void set_table_pointer(uint64_t& table, T* ptr, uint64_t mask)
    {
        table = (table & ~mask) | ((uint64_t)table & mask);
    }

    inline void set_prot_key(uint64_t& table, uint8_t prot_key)
    {
        table = (table & ~MASK_PROT_KEY) | ((((uint64_t)table) << 59) & MASK_PROT_KEY);
    }
};

#endif

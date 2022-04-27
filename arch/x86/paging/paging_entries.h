#ifndef __ARCH_X86_PAGING_ENTRIES_H__
#define __ARCH_X86_PAGING_ENTRIES_H__

#include "config.h"
#include <bitmanip.h>
#include <cstdint>

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

    constexpr uint64_t set_prot_key(uint64_t table, uint8_t prot_key)
    {
        return (table & ~MASK_PROT_KEY) | ((((uint64_t)table) << 59) & MASK_PROT_KEY);
    }

    enum cache_permissions : uint8_t
    {
        UC = 0,
        WC,
        RESERVED0,
        RESERVED1,
        WT,
        WP,
        WB,
        UCM,
    };

    struct page_prop
    {
        cache_permissions cache = UC;
        bool rw = true;
        bool us = false;
        bool x = false;
    };

    constexpr uint64_t make_page_pointer(uint64_t target, page_prop flags)
    {
        return 1 |
            ((uint8_t) flags.rw << 1) |
            ((uint8_t) flags.us << 2) |
            ((uint64_t) !flags.x << 63) |
            (target & MASK_TABLE_POINTER);
    }

    constexpr uint64_t make_page_small(uint64_t target, page_prop flags)
    {
        return 1 |
            PAGE_SIZE |
            ((uint8_t) flags.rw << 1) |
            ((uint8_t) flags.us << 2) |
            ((uint64_t) !flags.x << 63) |
            ((flags.cache) & 0b011 << 3) |
            ((flags.cache) & 0b100 << 7) |
            (target & MASK_TABLE_SMALL);
    }

    constexpr uint64_t make_page_medium(uint64_t target, page_prop flags)
    {
        return 1 |
            PAGE_SIZE |
            ((uint8_t) flags.rw << 1) |
            ((uint8_t) flags.us << 2) |
            ((uint64_t) !flags.x << 63) |
            ((flags.cache) & 0b011 << 3) |
            ((flags.cache) & 0b100 << 12) |
            (target & MASK_TABLE_MEDIUM);
    }

    constexpr uint64_t make_page_large(uint64_t target, page_prop flags)
    {
        return 1 |
            PAGE_SIZE |
            ((uint8_t) flags.rw << 1) |
            ((uint8_t) flags.us << 2) |
            ((uint64_t) !flags.x << 63) |
            ((flags.cache) & 0b011 << 3) |
            ((flags.cache) & 0b100 << 12) |
            (target & MASK_TABLE_LARGE);
    }

    using page_table_entry = uint64_t;
}; // namespace paging

#endif

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

    // flags order
    // bit from 0-7
    // rw, us, pwt, pcd, exec 
    constexpr uint64_t make_page_pointer(uint64_t target, uint8_t flags)
    {
        flags &= 0b00001111;
        return 1 | ((uint32_t)flags << 1) | (target & MASK_TABLE_POINTER) | (((uint64_t)flags & 0x80) << 63);
    }

    constexpr uint64_t make_page_medium(uint64_t target, uint8_t flags)
    {
        flags &= 0b00001111;
        return 1 | PAGE_SIZE | ((uint32_t)flags << 1) | (target & MASK_TABLE_MEDIUM) | (((uint64_t)flags & 0x80) << 63);
    }

    constexpr uint64_t make_page_large(uint64_t target, uint8_t flags)
    {
        flags &= 0b00001111;
        return 1 | PAGE_SIZE | ((uint32_t)flags << 1) | (target & MASK_TABLE_LARGE) | (((uint64_t)flags & 0x80) << 63);
    }

    constexpr auto make_page_small = make_page_pointer;
}; // namespace paging

#endif

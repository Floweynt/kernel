#pragma once

#include "config.h"
#include <bitmanip.h>
#include <cstdint>

namespace paging
{
    constexpr std::uint64_t PRESENT = 0x1;
    constexpr std::uint64_t RD_WR = 0x2;
    constexpr std::uint64_t USR_SUP = 0x4;
    constexpr std::uint64_t ACCESSED = 0x20;
    constexpr std::uint64_t PAGE_SIZE = 0x80;

    constexpr std::uint64_t MASK_TABLE_POINTER = 0xFFFFFFFFFF000;
    constexpr std::uint64_t MASK_TABLE_LARGE = 0xFFFFFC0000000;
    constexpr std::uint64_t MASK_TABLE_MEDIUM = 0xFFFFFFFE00000;
    constexpr std::uint64_t MASK_TABLE_SMALL = 0xFFFFFFFFFF000;

    constexpr std::uint64_t MASK_PROT_KEY = 0x7800000000000000;

    template <typename T>
    auto set_ptr(std::uint64_t table, T* ptr, std::uint64_t mask) -> std::uint64_t
    {
        return (table & ~mask) | ((std::uint64_t)ptr & mask);
    }

    constexpr auto set_prot_key(std::uint64_t table, std::uint8_t  /*prot_key*/) -> std::uint64_t
    {
        return (table & ~MASK_PROT_KEY) | ((((std::uint64_t)table) << 59) & MASK_PROT_KEY);
    }

    enum cache_permissions : std::uint8_t
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

    constexpr auto make_page_pointer(std::uint64_t target, page_prop flags) -> std::uint64_t
    {
        return 1 |
            ((std::uint8_t) flags.rw << 1) |
            ((std::uint8_t) flags.us << 2) |
            ((std::uint64_t) !flags.x << 63) |
            (target & MASK_TABLE_POINTER);
    }

    constexpr auto make_page_small(std::uint64_t target, page_prop flags) -> std::uint64_t
    {
        return 1 |
            PAGE_SIZE |
            ((std::uint8_t) flags.rw << 1) |
            ((std::uint8_t) flags.us << 2) |
            ((std::uint64_t) !flags.x << 63) |
            ((flags.cache) & 0b011 << 3) |
            ((flags.cache) & 0b100 << 7) |
            (target & MASK_TABLE_SMALL);
    }

    constexpr auto make_page_medium(std::uint64_t target, page_prop flags) -> std::uint64_t
    {
        return 1 |
            PAGE_SIZE |
            ((std::uint8_t) flags.rw << 1) |
            ((std::uint8_t) flags.us << 2) |
            ((std::uint64_t) !flags.x << 63) |
            ((flags.cache) & 0b011 << 3) |
            ((flags.cache) & 0b100 << 12) |
            (target & MASK_TABLE_MEDIUM);
    }

    constexpr auto make_page_large(std::uint64_t target, page_prop flags) -> std::uint64_t
    {
        return 1 |
            PAGE_SIZE |
            ((std::uint8_t) flags.rw << 1) |
            ((std::uint8_t) flags.us << 2) |
            ((std::uint64_t) !flags.x << 63) |
            ((flags.cache) & 0b011 << 3) |
            ((flags.cache) & 0b100 << 12) |
            (target & MASK_TABLE_LARGE);
    }

    using page_table_entry = std::uint64_t;
}; // namespace paging

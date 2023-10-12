// cSpell:ignore hhdm
#pragma once

#include "paging_entries.h"
#include <bitmanip.h>
#include <cstdint>
#include <misc/pointer.h>
#include <mm/mm.h>

namespace paging
{
    enum page_type : std::uint8_t
    {
        SMALL = 0, // 4KiB
        MEDIUM,    // 2 MiB
        BIG        // 1 GiB
    };

    void install();

    auto map_page_for(page_table_entry* table, page_type type, std::uintptr_t virtual_addr, std::uintptr_t physical_addr, page_prop prop,
                      bool overwrite) -> bool;
    auto map_page_for_early(page_table_entry* table, page_type type, std::uintptr_t virtual_addr, std::uintptr_t physical_addr, page_prop prop,
                            bool overwrite) -> bool;
    auto request_page(page_type type, std::uint64_t vaddr, std::uint64_t paddr, page_prop prop = {}, bool overwrite = false) -> bool;
    auto request_page_early(page_type type, std::uint64_t vaddr, std::uint64_t paddr, page_prop prop = {}, bool overwrite = false) -> bool;

    template <std::uint8_t t>
    constexpr auto get_page_entry(std::uint64_t virtual_addr) -> std::uint16_t
    {
        return std::get_bits<(3 - t) * 9 + 12, (3 - t) * 9 + 20>(virtual_addr) >> ((3 - t) * 9 + 12);
    }

    constexpr auto get_page_entry(std::uint64_t virtual_addr, std::uint8_t type) -> std::uint16_t
    {
        return std::get_bits(virtual_addr, (3 - type) * 9 + 12, (3 - type) * 9 + 20) >> ((3 - type) * 9 + 12);
    }

    INLINE auto map_hhdm_page(page_type pt, std::uint64_t paddr, page_prop prop = {}, bool overwrite = false) -> bool
    {
        return request_page(pt, mm::make_virtual(paddr), paddr, prop, overwrite);
    }

    INLINE auto map_hhdm_page_early(page_type pt, std::uint64_t paddr, page_prop prop = {}, bool overwrite = false) -> bool
    {
        return request_page_early(pt, mm::make_virtual(paddr), paddr, prop, overwrite);
    }

    void map_hhdm_section(std::uint64_t addr, std::uint64_t len, paging::page_prop prop, bool overwrite = false);
    void map_hhdm_section_early(std::uint64_t addr, std::uint64_t len, paging::page_prop prop, bool overwrite = false);
    void sync_page_tables(std::size_t dest_core, std::size_t src_core);
    void copy_kernel_page_tables(page_table_entry* dest, const page_table_entry* src);
} // namespace paging

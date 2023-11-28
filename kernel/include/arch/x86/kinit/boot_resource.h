// cSpell:ignore kinit, stivale, lapic, rsdp, xsdt
#pragma once

#include "limine.h"
#include <acpi/acpi.h>
#include <cstddef>
#include <cstdint>

// boot_warn_flags
enum init_warn_flags : std::uint32_t
{
    WARN_MMAP_OVERFLOW = 1 << 0,
    WARN_PMM_OVERFLOW = 1 << 1
};

class modules
{
    void* symbols;

public:
    modules();
    [[nodiscard]] constexpr auto get_symbols() const -> void* { return symbols; }
};

class boot_resource
{
    inline static constexpr std::size_t MMAP_ENTRIES = 48;
    std::uint64_t phys_addr;
    std::uint64_t ksize;
    std::size_t mmap_length;
    std::size_t pmrs_length;
    std::size_t cores;
    std::size_t bsp_id_lapic;
    limine_memmap_entry mmap_entries[MMAP_ENTRIES];
    acpi::rsdp_descriptor* root_table;
    bool smp_status;
    std::uint32_t flags;
    modules mods;

public:
    boot_resource();
    static auto instance() -> boot_resource&;

    [[nodiscard]] constexpr auto kernel_phys_addr() const -> std::uint64_t { return phys_addr; }
    [[nodiscard]] constexpr auto kernel_size() const -> std::uint64_t { return ksize; }
    [[nodiscard]] constexpr auto rsdp() const -> acpi::rsdp_descriptor* { return root_table; }
    [[nodiscard]] constexpr auto core_count() const -> std::size_t { return cores; }
    [[nodiscard]] constexpr auto bsp_id() const -> std::size_t { return bsp_id_lapic; }

    template <typename T>
    void iterate_mmap(T callback)
    {
        for (std::size_t i = 0; i < mmap_length; i++)
        {
            callback(mmap_entries[i]);
        }
    }

    template <typename T>
    void iterate_xsdt(T callback)
    {
        auto* table = as_ptr<acpi::xsdt>(root_table->xsdt_address + 0xffff800000000000ul);
        std::size_t len = (table->h.length - sizeof(acpi::acpi_sdt_header)) / 8;
        for (std::size_t i = 0; i < len; i++)
        {
            callback(table->table[i]);
        }
    }

    [[nodiscard]] auto is_smp() const -> bool { return smp_status; }
    constexpr void mark_smp_start() { smp_status = true; }

    constexpr void warn_init(init_warn_flags flag) { flags = flags | flag; }
    [[nodiscard]] constexpr auto warn_init() const -> std::uint32_t { return flags; }

    [[nodiscard]] constexpr auto modules() const -> const modules& { return mods; }

    [[nodiscard]] constexpr auto memmap() const -> const auto& { return mmap_entries; }
    [[nodiscard]] constexpr auto memmap_length() const -> const auto& { return mmap_length; }
};

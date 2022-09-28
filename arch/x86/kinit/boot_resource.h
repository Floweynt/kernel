// cSpell:ignore kinit, stivale, lapic, rsdp, xsdt
#ifndef __KERNEL_ARCH_X86_KINIT_BOOT_RESOURCE_H__
#define __KERNEL_ARCH_X86_KINIT_BOOT_RESOURCE_H__
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
    constexpr void* get_symbols() const { return symbols; } 
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
    static boot_resource& instance();

    constexpr std::uint64_t kernel_phys_addr() const { return phys_addr; }
    constexpr std::uint64_t kernel_size() const { return ksize; }
    constexpr acpi::rsdp_descriptor* rsdp() const { return root_table; }
    constexpr std::size_t core_count() const { return cores; }
    constexpr std::size_t bsp_id() const { return bsp_id_lapic; }

    template <typename T>
    void iterate_mmap(T callback)
    {
        for (std::size_t i = 0; i < mmap_length; i++)
            callback(mmap_entries[i]);
    }

    template <typename T>
    void iterate_xsdt(T callback)
    {
        acpi::xsdt* table = (acpi::xsdt*)(root_table->xsdt_address + 0xffff800000000000ul);
        std::size_t n = (table->h.length - sizeof(acpi::acpi_sdt_header)) / 8;
        for (std::size_t i = 0; i < n; i++)
            callback(table->table[i]);
    }

    bool is_smp() const { return smp_status; }
    constexpr void mark_smp_start() { smp_status = true; }

    constexpr void warn_init(init_warn_flags f) { flags = flags | f; }
    constexpr std::uint32_t warn_init() const { return flags; }

    constexpr const modules& modules() const { return mods; }
};

#endif

#ifndef __KERNEL_ARCH_X86_KINIT_BOOT_RESOURCE_H__
#define __KERNEL_ARCH_X86_KINIT_BOOT_RESOURCE_H__
#include "stivale2.h"
#include <acpi/acpi.h>
#include <cstddef>
#include <cstdint>

class boot_resource
{
    uint64_t phys_addr;
    uint64_t ksize;
    std::size_t mmap_length;
    std::size_t cores;
    std::size_t bsp_id_lapic;
    stivale2_mmap_entry mmap_entries[0x100];
    acpi::rsdp_descriptor* root_table;

public:
    boot_resource(stivale2_struct*);
    static boot_resource& instance();

    constexpr uint64_t kernel_phys_addr() const { return phys_addr; }
    constexpr uint64_t kernel_size() const { return ksize; }
    constexpr acpi::rsdp_descriptor* rsdp() const { return root_table; }
    constexpr std::size_t core_count() { return cores; }
    constexpr std::size_t bsp_id() { return bsp_id_lapic; }

    template <typename T>
    void iterate_mmap(T cb)
    {
        for (std::size_t i = 0; i < mmap_length; i++)
            cb(mmap_entries[i]);
    }

    template <typename T>
    void iterate_xsdt(T cb)
    {
        acpi::xsdt* table = (acpi::xsdt*)(root_table->xsdt_address + 0xffff800000000000ul);
        std::size_t n = (table->h.length - sizeof(acpi::acpi_sdt_header)) / 8;
        for (std::size_t i = 0; i < n; i++)
            cb(table->table[i]);
    }
};

#endif

#ifndef __ARCH_X86_ACPI_ACPI_H__
#define __ARCH_X86_ACPI_ACPI_H__
#include <cstddef>
#include <cstdint>

namespace acpi
{
    struct [[gnu::packed]] rsdp_descriptor
    {
        char signature[8];
        uint8_t checksum;
        char oem_id[6];
        uint8_t revision;
        uint32_t rsdt_address;
        uint32_t length;
        uint64_t xsdt_address;
        uint8_t extended_checksum;
        uint8_t reserved[3];
    };

    struct acpi_sdt_header
    {
        uint32_t signature;
        uint32_t length;
        uint8_t revision;
        uint8_t checksum;
        char oem_id[6];
        char oem_table_id[8];
        uint32_t oem_revision;
        uint32_t creator_id;
        uint32_t creator_revision;
    };

    struct [[gnu::packed]] xsdt
    {
        acpi_sdt_header h;
        acpi_sdt_header* table[];
    };

    struct [[gnu::packed]] madt
    {
        inline constexpr static uint32_t SIGNATURE = 0x43495041; // "APIC"
        acpi_sdt_header parent;
        uint32_t lapic_addr;
        uint32_t flags;
    };

    struct [[gnu::packed]] madt_entry_descriptor
    {
        uint8_t type;
        uint8_t length;
    };

    struct [[gnu::packed]] madt_processor_local_apic
    {
        inline constexpr static uint32_t SIGNATURE = 0;
        uint8_t processor_id;
        uint8_t apic_id;
        uint32_t flags;
    };

    struct [[gnu::packed]] madt_io_apic
    {
        inline constexpr static uint32_t SIGNATURE = 1;
        uint8_t id;
        uint8_t reserved;
        uint32_t io_apic_address;
        uint32_t interrupt_base;
    };

    struct [[gnu::packed]] madt_io_apic_source_override
    {
        inline constexpr static uint32_t SIGNATURE = 2;
        uint8_t bus_source;
        uint8_t int_source;
        uint32_t global_system_interrupt;
        uint16_t flags;
    };

    struct [[gnu::packed]] madt_io_apic_nmi_source
    {
        inline constexpr static uint32_t SIGNATURE = 3;
        uint8_t nmi_source;
        uint8_t resered;
        uint16_t flags;
        uint32_t global_system_interrupt;
    };

    struct [[gnu::packed]] madt_local_apic_nmi
    {
        inline constexpr static uint32_t SIGNATURE = 4;
        uint8_t processor_id;
        uint16_t flags;
        uint8_t lint;
    };

    struct [[gnu::packed]] madt_local_apic_address_override
    {
        inline constexpr static uint32_t SIGNATURE = 5;
        uint16_t reserved;
        uint64_t phy_addr;
    };

    struct [[gnu::packed]] madt_processor_local_2xapic
    {
        inline constexpr static uint32_t SIGNATURE = 9;
        uint16_t reserved;
        uint32_t apic_2x_id;
        uint32_t flags;
        uint32_t acpi_id;
    };

    template <typename T>
    bool check(T* desc)
    {
        char* ptr = (char*)desc;
        uint64_t sum = 0;

        for (std::size_t i = 0; i < sizeof(T); i++)
            sum += ptr[i];

        return sum % 0x100 == 0;
    }

    template <typename T>
    T* get_table(xsdt* table)
    {
        std::size_t n = (table->h.length - sizeof(acpi_sdt_header)) / 8;
        for (std::size_t i = 0; i < n; i++)
            if (table->table[i]->signature == T::SIGNATURE)
                return (T*)table->table[i];
    }

    template <typename T>
    T* madt_get_entry(madt* table)
    {
        // iterate entries
        madt_entry_descriptor* entry = (madt_entry_descriptor*)(table + 1);

        while ((uint64_t)entry < ((uint64_t)table + table->parent.length))
        {
            if (T::SIGNATURE == entry->type)
                return (T*)(entry + 1);

            entry = (madt_entry_descriptor*)((uint8_t*)entry + entry->length);
        }
    }

    inline constexpr uint32_t BGRT_SIGNATURE = 0x54524742; // "BGRT"
    inline constexpr uint32_t BERT_SIGNATURE = 0x54524542; // "BERT"
    inline constexpr uint32_t CPEP_SIGNATURE = 0x50455043; // "CPEP"
} // namespace acpi

#endif

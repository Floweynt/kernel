#ifndef __ARCH_X86_ACPI_ACPI_H__
#define __ARCH_X86_ACPI_ACPI_H__
#include <cstddef>
#include <cstdint>
namespace acpi
{
    /// \brief Root System Description Pointer Descriptor
    /// Technically version 2.0 version of the rsdp, which contains the xsdt address
    /// instead of the rsdt address
    /// See: https://wiki.osdev.org/RSDP
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

    /// \brief Root System Description Table
    /// The is the root table that includes pointers to all the other tables
    /// See: https://wiki.osdev.org/RSDT
    struct [[gnu::packed]] acpi_sdt_header
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

    /// \brief eXtended System Descriptor Table
    /// The more modern version of the RSDT, which should be used.
    /// See: https://wiki.osdev.org/XSDT
    struct [[gnu::packed]] xsdt
    {
        acpi_sdt_header h;
        acpi_sdt_header* table[];
    };

    /// \brief Multiple APIC Description Table
    /// Describes information about the APIC, including LAPIC and I/O-APIC information
    /// See: https://wiki.osdev.org/MADT
    struct [[gnu::packed]] madt
    {
        inline constexpr static uint32_t SIGNATURE = 0x43495041; // "APIC"
        acpi_sdt_header parent;
        uint32_t lapic_addr;
        uint32_t flags;
    };

    /// \brief A single entry within the MADT
    ///
    struct [[gnu::packed]] madt_entry_descriptor
    {
        uint8_t type;
        uint8_t length;
    };

    /// \brief The entry within the MADT that contains information about the LAPIC
    ///
    struct [[gnu::packed]] madt_processor_local_apic
    {
        inline constexpr static uint32_t SIGNATURE = 0;
        uint8_t processor_id;
        uint8_t apic_id;
        uint32_t flags;
    };

    /// \brief The entry within the MADT that contains information about the I/O-APIC
    /// 
    struct [[gnu::packed]] madt_io_apic
    {
        inline constexpr static uint32_t SIGNATURE = 1;
        uint8_t id;
        uint8_t reserved;
        uint32_t io_apic_address;
        uint32_t interrupt_base;
    };

    /// \brief The entry within the MADT that handles interrupt source overrides
    /// From OsDev wiki:
    /// "contains the data for an Interrupt Source Override. This explains how IRQ sources are mapped to global system
    /// interrupts. For example, IRQ source for the timer is 0, and the global system interrupt will usually be 2.
    /// So you could look for the I/O APIC with the base below 2 and within its redirection entries, then make
    /// the redirection entry for (2 - base) to be the timer interrupt."
    struct [[gnu::packed]] madt_io_apic_source_override
    {
        inline constexpr static uint32_t SIGNATURE = 2;
        uint8_t bus_source;
        uint8_t int_source;
        uint32_t global_system_interrupt;
        uint16_t flags;
    };

    /// \brief The entry within the MADT that describes non-maskable interrupt sources
    ///
    struct [[gnu::packed]] madt_io_apic_nmi_source
    {
        inline constexpr static uint32_t SIGNATURE = 3;
        uint8_t nmi_source;
        uint8_t reserved;
        uint16_t flags;
        uint32_t global_system_interrupt;
    };

    /// \brief The entry within the MADT that describes LAPIC NMIs
    ///
    struct [[gnu::packed]] madt_local_apic_nmi
    {
        inline constexpr static uint32_t SIGNATURE = 4;
        uint8_t processor_id;
        uint16_t flags;
        uint8_t lint;
    };

    /// \brief The entry within the MADT that provides 64 bit systems with an override of the physical address of the LAPIC
    /// There can only be one of these defined in the MADT. If defined the 64-bit Local APIC address stored should be
    /// used instead of the 32-bit Local APIC address stored in the MADT header.
    /// This appears to be obsolete as a result of the IA32_APIC_BASE MSR
    struct [[gnu::packed]] madt_local_apic_address_override
    {
        inline constexpr static uint32_t SIGNATURE = 5;
        uint16_t reserved;
        uint64_t phy_addr;
    };

    /// \brief The entry within the MADt that describes an 2x-apic
    struct [[gnu::packed]] madt_processor_local_2xapic
    {
        inline constexpr static uint32_t SIGNATURE = 9;
        uint16_t reserved;
        uint32_t apic_2x_id;
        uint32_t flags;
        uint32_t acpi_id;
    };

    /// \brief Checks an table's checksum value, and return true if the checksum was successful
    /// \return Wether or not the table is valid
    template <typename T>
    bool check(T* desc)
    {
        char* ptr = (char*)desc;
        uint64_t sum = 0;

        for (std::size_t i = 0; i < sizeof(T); i++)
            sum += ptr[i];

        return (sum & 0xff) == 0;
    }

    /// \brief Obtains a table, given the table type
    /// \tparam T The type of the table
    /// \return A pointer to the table, or null if not found
    ///
    /// The table type must have a public static uint32_t member called SIGNATURE, which will be checked against in order to
    /// determine table type
    template <typename T>
    T* get_table(xsdt* table)
    {
        std::size_t n = (table->h.length - sizeof(acpi_sdt_header)) / 8;
        for (std::size_t i = 0; i < n; i++)
            if (table->table[i]->signature == T::SIGNATURE)
                return (T*)table->table[i];
        return nullptr;
    }

    /*
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
    */
} // namespace acpi

#endif

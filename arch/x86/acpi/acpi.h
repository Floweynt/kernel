#pragma once

#include <cstddef>
#include <cstdint>
#include <concepts>

namespace acpi
{
    /// \brief Root System Description Pointer Descriptor
    /// Technically version 2.0 version of the rsdp, which contains the xsdt address
    /// instead of the rsdt address
    /// See: https://wiki.osdev.org/RSDP
    struct [[gnu::packed]] rsdp_descriptor
    {
        const char signature[8];
        const std::uint8_t checksum;
        const char oem_id[6];
        const std::uint8_t revision;
        const std::uint32_t rsdt_address;
        const std::uint32_t length;
        const std::uint64_t xsdt_address;
        const std::uint8_t extended_checksum;
        const std::uint8_t reserved[3];
    };

    /// \brief Root System Description Table
    /// The is the root table that includes pointers to all the other tables
    /// See: https://wiki.osdev.org/RSDT
    struct [[gnu::packed]] acpi_sdt_header
    {
        const std::uint32_t signature;
        const std::uint32_t length;
        const std::uint8_t revision;
        const std::uint8_t checksum;
        const char oem_id[6];
        const char oem_table_id[8];
        const std::uint32_t oem_revision;
        const std::uint32_t creator_id;
        const std::uint32_t creator_revision;
    };

    /// \brief eXtended System Descriptor Table
    /// The more modern version of the RSDT, which should be used.
    /// See: https://wiki.osdev.org/XSDT
    struct [[gnu::packed]] xsdt
    {
        const acpi_sdt_header h;
        const acpi_sdt_header* table[];
    };

    /// \brief Multiple APIC Description Table
    /// Describes information about the APIC, including LAPIC and I/O-APIC information
    /// See: https://wiki.osdev.org/MADT
    struct [[gnu::packed]] madt
    {
        inline constexpr static const std::uint32_t SIGNATURE = 0x43495041; // "APIC"
        const acpi_sdt_header parent;
        const std::uint32_t lapic_addr;
        const std::uint32_t flags;
    };

    /// \brief A single entry within the MADT
    ///
    struct [[gnu::packed]] madt_entry_descriptor
    {
        const std::uint8_t type;
        const std::uint8_t length;
    };

    /// \brief The entry within the MADT that contains information about the LAPIC
    ///
    struct [[gnu::packed]] madt_processor_local_apic
    {
        inline constexpr static const std::uint32_t SIGNATURE = 0;
        const std::uint8_t processor_id;
        const std::uint8_t apic_id;
        const std::uint32_t flags;
    };

    /// \brief The entry within the MADT that contains information about the I/O-APIC
    /// 
    struct [[gnu::packed]] madt_io_apic
    {
        inline constexpr static const std::uint32_t SIGNATURE = 1;
        const std::uint8_t id;
        const std::uint8_t reserved;
        const std::uint32_t io_apic_address;
        const std::uint32_t interrupt_base;
    };

    /// \brief The entry within the MADT that handles interrupt source overrides
    /// From OsDev wiki:
    /// "contains the data for an Interrupt Source Override. This explains how IRQ sources are mapped to global system
    /// interrupts. For example, IRQ source for the timer is 0, and the global system interrupt will usually be 2.
    /// So you could look for the I/O APIC with the base below 2 and within its redirection entries, then make
    /// the redirection entry for (2 - base) to be the timer interrupt."
    struct [[gnu::packed]] madt_io_apic_source_override
    {
        inline constexpr static const std::uint32_t SIGNATURE = 2;
        const std::uint8_t bus_source;
        const std::uint8_t int_source;
        const std::uint32_t global_system_interrupt;
        const std::uint16_t flags;
    };

    /// \brief The entry within the MADT that describes non-maskable interrupt sources
    ///
    struct [[gnu::packed]] madt_io_apic_nmi_source
    {
        inline constexpr static const std::uint32_t SIGNATURE = 3;
        const std::uint8_t nmi_source;
        const std::uint8_t reserved;
        const std::uint16_t flags;
        const std::uint32_t global_system_interrupt;
    };

    /// \brief The entry within the MADT that describes LAPIC NMIs
    ///
    struct [[gnu::packed]] madt_local_apic_nmi
    {
        inline constexpr static const std::uint32_t SIGNATURE = 4;
        const std::uint8_t processor_id;
        const std::uint16_t flags;
        const std::uint8_t lint;
    };

    /// \brief The entry within the MADT that provides 64 bit systems with an override of the physical address of the LAPIC
    /// There can only be one of these defined in the MADT. If defined the 64-bit Local APIC address stored should be
    /// used instead of the 32-bit Local APIC address stored in the MADT header.
    /// This appears to be obsolete as a result of the IA32_APIC_BASE MSR
    struct [[gnu::packed]] madt_local_apic_address_override
    {
        inline constexpr static const std::uint32_t SIGNATURE = 5;
        const std::uint16_t reserved;
        const std::uint64_t phy_addr;
    };

    /// \brief The entry within the MADt that describes an 2x-apic
    struct [[gnu::packed]] madt_processor_local_2xapic
    {
        inline constexpr static const std::uint32_t SIGNATURE = 9;
        const std::uint16_t reserved;
        const std::uint32_t apic_2x_id;
        const std::uint32_t flags;
        const std::uint32_t acpi_id;
    };

    /// \brief Checks an table's checksum value, and return true if the checksum was successful
    /// \return Wether or not the table is valid
    template <typename T>
    bool check(const T* desc)
    {
        char* ptr = (char*)desc;
        std::uint64_t sum = 0;

        for (std::size_t i = 0; i < sizeof(T); i++)
            sum += ptr[i];

        return (sum & 0xff) == 0;
    }

    template<typename T>
    concept acpi_table = requires(T a)
    {
        { T::SIGNATURE } -> std::same_as<const std::uint32_t>;
    };

    /// \brief Obtains a table, given the table type
    /// \tparam T The type of the table
    /// \return A pointer to the table, or null if not found
    ///
    /// The table type must have a public static const std::uint32_t member called SIGNATURE, which will be checked against in order to
    /// determine table type
    template <acpi_table T>
    T* get_table(const xsdt* table)
    {
        std::size_t n = (table->h.length - sizeof(acpi_sdt_header)) / 8;
        for (std::size_t i = 0; i < n; i++)
            if (table->table[i]->signature == T::SIGNATURE)
                return (T*)table->table[i];
        return nullptr;
    }
} // namespace acpi

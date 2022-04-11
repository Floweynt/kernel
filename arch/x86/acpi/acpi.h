#ifndef __ARCH_X86_ACPI_ACPI_H__
#define __ARCH_X86_ACPI_ACPI_H__
#include <cstdint>
#include <cstddef>

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
        char signature[4];
        uint32_t length;
        uint8_t revision;
        uint8_t checksum;
        char oem_id[6];
        char oem_table_id[8];
        uint32_t oem_revision;
        uint32_t creator_id;
        uint32_t creator_revision;
    };

    struct xsdt
    {
        acpi_sdt_header h;
        void* table[];
    };

    template<typename T>
    bool check(T* desc)
    {
        char* ptr = (char*)desc;
        uint64_t sum = 0;

        for(std::size_t i = 0; i < sizeof(T); i++)
            sum += ptr[i];

        return sum % 0x100 == 0;
    }
}

#endif

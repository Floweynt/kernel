#pragma once

#include <asm/asm_cpp.h>

namespace pci
{
    inline constexpr std::uint16_t CONFIG_ADDRESS = 0xcf8;
    inline constexpr std::uint16_t CONFIG_DATA = 0xcfc;

    class pci_device_ident
    {
        std::uint8_t _bus;
        std::uint8_t _slot;
        constexpr std::uint32_t make_address(std::uint8_t func, std::uint8_t offset) const
        {
            return ((std::uint32_t)_bus << 16) | ((std::uint32_t)_slot << 11) | ((std::uint32_t)func << 8) | ((std::uint32_t)offset & 0xfc) |
                   ((std::uint32_t)0x80000000);
        }

    public:
        constexpr pci_device_ident(std::uint8_t bus, std::uint8_t slot) : _bus(bus), _slot(slot) {}
        constexpr pci_device_ident(const pci_device_ident& rhs) : _bus(rhs._bus), _slot(rhs._slot) {}
        constexpr std::uint8_t bus() const { return _bus; }
        constexpr std::uint8_t slot() const { return _slot; }

        inline std::uint32_t read_config(std::uint8_t func, std::uint8_t offset) const
        {
            outl(CONFIG_ADDRESS, make_address(func, offset));
            return inl(CONFIG_DATA);
        }

        inline void write_config(std::uint8_t func, std::uint8_t offset, std::uint32_t data) const
        {
            outl(CONFIG_ADDRESS, make_address(func, offset));
            outl(CONFIG_DATA, data);
        }

        inline std::uint16_t device_id(std::uint8_t func) const { return read_config(func, 0) >> 16; }
        inline std::uint16_t vendor_id(std::uint8_t func) const { return read_config(func, 0); }
        inline std::uint16_t status(std::uint8_t func) const { return read_config(func, 4) >> 16; }
        inline std::uint16_t command(std::uint8_t func) const { return read_config(func, 4); }
        inline std::uint8_t class_code(std::uint8_t func) const { return read_config(func, 8) >> 24; }
        inline std::uint8_t subclass(std::uint8_t func) const { return read_config(func, 8) >> 16; }
        inline std::uint8_t prog_if(std::uint8_t func) const { return read_config(func, 8) >> 8; }
        inline std::uint8_t revision_id(std::uint8_t func) const { return read_config(func, 8); }
        inline std::uint8_t bist(std::uint8_t func) const { return read_config(func, 12) >> 24; }
        inline std::uint8_t header_type(std::uint8_t func) const { return read_config(func, 12) >> 16; }
        inline std::uint8_t latency_timer(std::uint8_t func) const { return read_config(func, 12) >> 8; }
        inline std::uint8_t cache_line_size(std::uint8_t func) const { return read_config(func, 12); }
    };

    void scan();
} // namespace pci

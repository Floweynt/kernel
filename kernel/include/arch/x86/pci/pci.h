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
        [[nodiscard]] constexpr auto make_address(std::uint8_t func, std::uint8_t offset) const -> std::uint32_t
        {
            return ((std::uint32_t)_bus << 16) | ((std::uint32_t)_slot << 11) | ((std::uint32_t)func << 8) | ((std::uint32_t)offset & 0xfc) |
                   ((std::uint32_t)0x80000000);
        }

    public:
        constexpr pci_device_ident(std::uint8_t bus, std::uint8_t slot) : _bus(bus), _slot(slot) {}
        constexpr pci_device_ident(const pci_device_ident& rhs) = default;
        [[nodiscard]] constexpr auto bus() const -> std::uint8_t { return _bus; }
        [[nodiscard]] constexpr auto slot() const -> std::uint8_t { return _slot; }

        [[nodiscard]] inline auto read_config(std::uint8_t func, std::uint8_t offset) const -> std::uint32_t
        {
            outl(CONFIG_ADDRESS, make_address(func, offset));
            return inl(CONFIG_DATA);
        }

        inline void write_config(std::uint8_t func, std::uint8_t offset, std::uint32_t data) const
        {
            outl(CONFIG_ADDRESS, make_address(func, offset));
            outl(CONFIG_DATA, data);
        }

        [[nodiscard]] inline auto device_id(std::uint8_t func) const -> std::uint16_t { return read_config(func, 0) >> 16; }
        [[nodiscard]] inline auto vendor_id(std::uint8_t func) const -> std::uint16_t { return read_config(func, 0); }
        [[nodiscard]] inline auto status(std::uint8_t func) const -> std::uint16_t { return read_config(func, 4) >> 16; }
        [[nodiscard]] inline auto command(std::uint8_t func) const -> std::uint16_t { return read_config(func, 4); }
        [[nodiscard]] inline auto class_code(std::uint8_t func) const -> std::uint8_t { return read_config(func, 8) >> 24; }
        [[nodiscard]] inline auto subclass(std::uint8_t func) const -> std::uint8_t { return read_config(func, 8) >> 16; }
        [[nodiscard]] inline auto prog_if(std::uint8_t func) const -> std::uint8_t { return read_config(func, 8) >> 8; }
        [[nodiscard]] inline auto revision_id(std::uint8_t func) const -> std::uint8_t { return read_config(func, 8); }
        [[nodiscard]] inline auto bist(std::uint8_t func) const -> std::uint8_t { return read_config(func, 12) >> 24; }
        [[nodiscard]] inline auto header_type(std::uint8_t func) const -> std::uint8_t { return read_config(func, 12) >> 16; }
        [[nodiscard]] inline auto latency_timer(std::uint8_t func) const -> std::uint8_t { return read_config(func, 12) >> 8; }
        [[nodiscard]] inline auto cache_line_size(std::uint8_t func) const -> std::uint8_t { return read_config(func, 12); }
    };

    void scan();
} // namespace pci

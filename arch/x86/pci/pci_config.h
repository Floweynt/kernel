#ifndef __ARCH_X86_PCI_PCI_CONFIG_H__
#define __ARCH_X86_PCI_PCI_CONFIG_H__
#include <asm/asm_cpp.h>

namespace pci
{
    inline constexpr uint16_t CONFIG_ADDRESS = 0xcf8;
    inline constexpr uint16_t CONFIG_DATA = 0xcfc;

    class pci_device_ident
    {
        uint8_t _bus;
        uint8_t _slot;
        constexpr uint32_t make_address(uint8_t func, uint8_t offset)
        {
            return ((uint32_t)_bus << 16) | 
                   ((uint32_t)_slot << 11) | 
                   ((uint32_t)func << 8) | 
                   ((uint32_t)offset & 0xfc) | 
                   ((uint32_t)0x80000000);
        }
    public:
        constexpr pci_device_ident(uint8_t b, uint8_t s) : _bus(b), _slot(s) {}
        constexpr pci_device_ident(const pci_device_ident& rhs) : _bus(rhs._bus), _slot(rhs._slot) {}
        constexpr uint8_t bus() { return _bus; }
        constexpr uint8_t slot() { return _slot; }

        uint32_t read_config(uint8_t func, uint8_t offset)
        {
            outl(CONFIG_ADDRESS, make_address(func, offset));
            return inl(CONFIG_DATA);
        }

        void write_config(uint8_t func, uint8_t offset, uint32_t data)
        {
            outl(CONFIG_ADDRESS, make_address(func, offset));
            outl(CONFIG_DATA, data);
        }
    };
} // namespace pci

#endif

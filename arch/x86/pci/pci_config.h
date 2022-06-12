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
            return ((uint32_t)_bus << 16) | ((uint32_t)_slot << 11) | ((uint32_t)func << 8) | ((uint32_t)offset & 0xfc) |
                   ((uint32_t)0x80000000);
        }

    public:
        constexpr pci_device_ident(uint8_t b, uint8_t s) : _bus(b), _slot(s) {}
        constexpr pci_device_ident(const pci_device_ident& rhs) : _bus(rhs._bus), _slot(rhs._slot) {}
        constexpr uint8_t bus() { return _bus; }
        constexpr uint8_t slot() { return _slot; }

        inline uint32_t read_config(uint8_t func, uint8_t offset)
        {
            outl(CONFIG_ADDRESS, make_address(func, offset));
            return inl(CONFIG_DATA);
        }

        inline void write_config(uint8_t func, uint8_t offset, uint32_t data)
        {
            outl(CONFIG_ADDRESS, make_address(func, offset));
            outl(CONFIG_DATA, data);
        }

        inline uint16_t device_id(uint8_t f) { return read_config(f, 0) >> 16; }
        inline uint16_t vendor_id(uint8_t f) { return read_config(f, 0); }
        inline uint8_t class_id(uint8_t f) { return read_config(f, 8) >> 24; }
        inline uint8_t subclass_id(uint8_t f) { return read_config(f, 8) >> 16; }
        inline uint8_t header_type(uint8_t f) { return read_config(f, 12) >> 16; }
    };

    void checkBus(uint8_t bus);

    void checkFunction(pci_device_ident device, uint8_t function)
    {
        uint8_t baseClass;
        uint8_t subClass;
        uint8_t secondaryBus;

        if ((device.class_id(function) == 0x6) && (device.subclass_id(function) == 0x4))
        {
            secondaryBus = getSecondaryBus(bus, device, function);
            checkBus(secondaryBus);
        }
    }

    void checkDevice(pci_device_ident device)
    {
        uint8_t function = 0;
        if (device.vendor_id(function) == 0xffff)
            return;
        checkFunction(device, function);

        if ((device.header_type(function) & 0x80) != 0)
        {
            for (function = 1; function < 8; function++)
                if (device.vendor_id(function) != 0xffff)
                    checkFunction(device, function);
        }
    }

    void checkBus(uint8_t bus)
    {
        for (uint8_t device = 0; device < 32; device++)
            checkDevice({bus, device});
    }

    void checkAllBuses()
    {
        uint8_t function;
        uint8_t bus;

        auto headerType = pci_device_ident{0, 0}.header_type(0);
        if ((headerType & 0x80) == 0)
        {
            // Single PCI host controller
            checkBus(0);
        }
        else
        {
            // Multiple PCI host controllers
            for (function = 0; function < 8; function++)
            {
                if (pci_device_ident{0, 0}.vendor_id(function) != 0xffff)
                    break;
                bus = function;
                checkBus(bus);
            }
        }
    } // namespace pci
}
#endif

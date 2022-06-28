#include "pci.h"
#include <cstdint>
#include <printf.h>
namespace pci
{
    static void check_bus(uint8_t bus, int level);
    static void check_function(pci_device_ident dev, uint8_t function, int level)
    {
        if ((dev.class_code(function) == 0x6) && (dev.subclass(function) == 0x4))
        {
            uint8_t secondary_bus = dev.read_config(function, 0x18) >> 8;
            std::printf("[PCI]%*c%02hhx:%02hhx.%hhx: bridge dev=%02hhx:%02hhx-r%02hhx\n bus=%02hhx", level, ' ', dev.bus(),
                        dev.slot(), function, dev.device_id(function), dev.subclass(function), dev.revision_id(function),
                        secondary_bus);

            check_bus(secondary_bus, level + 1);
            return;
        }

        std::printf("[PCI]%*c%02hhx:%02hhx.%hhx: class=%02hhx:%02hhx dev=%02hhx:%02hhx-r%02hhx\n", level, ' ', dev.bus(),
                    dev.slot(), function, dev.class_code(function), dev.subclass(function), dev.device_id(function),
                    dev.subclass(function), dev.revision_id(function));
    }

    static void check_device(pci_device_ident dev, int level)
    {
        if (dev.vendor_id(0) == 0xFFFF)
            return;
        check_function(dev, 0, level);
        if (dev.header_type(0) & 0x80)
        {
            for (uint8_t function = 1; function < 8; function++)
            {
                if (dev.vendor_id(function) != 0xFFFF)
                    check_function(dev, function, level);
            }
        }
    }

    static void check_bus(uint8_t bus, int level)
    {
        for (uint8_t device = 0; device < 32; device++)
            check_device({bus, device}, level);
    }

    void scan()
    {
        pci_device_ident root{0, 0};
        if ((root.header_type(0) & 0x80) == 0)
            check_bus(0, 0);
        else
        {
            for (uint8_t function = 0; function < 8; function++)
            {
                if (root.vendor_id(function) != 0xFFFF)
                    break;
                check_bus(function, 0);
            }
        }
    }
} // namespace pci

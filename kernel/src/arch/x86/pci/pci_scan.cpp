#include <pci/pci.h>
#include <config.h>
#include <cstdint>
#include <printf.h>
namespace pci
{
    namespace
    {
        void check_bus(std::uint8_t bus, int level);
        void check_function(pci_device_ident dev, std::uint8_t function, int level)
        {
            if ((dev.class_code(function) == 0x6) && (dev.subclass(function) == 0x4))
            {
                std::uint8_t secondary_bus = dev.read_config_long(function, 0x18) >> 8;
                if constexpr (config::get_val<"debug.log.pci">)
                {
                    std::printf("[PCI]%*c%02hhx:%02hhx.%hhx: bridge dev=%02hx:%02hhx-r%02hhx\n bus=%02hhx", level, ' ', dev.bus(), dev.slot(),
                                function, dev.device_id(function), dev.subclass(function), dev.revision_id(function), secondary_bus);
                }

                check_bus(secondary_bus, level + 1);
                return;
            }
            if constexpr (config::get_val<"debug.log.pci">)
            {
                std::printf("[PCI]%*c%02hhx:%02hhx.%hhx: class=%02hhx:%02hhx dev=%02hx:%02hhx-r%02hhx\n", level, ' ', dev.bus(), dev.slot(), function,
                            dev.class_code(function), dev.subclass(function), dev.device_id(function), dev.subclass(function),
                            dev.revision_id(function));
            }
        }

        void check_device(pci_device_ident dev, int level)
        {
            if (dev.vendor_id(0) == 0xFFFF)
            {
                return;
            }
            check_function(dev, 0, level);
            if (dev.header_type(0) & 0x80)
            {
                for (std::uint8_t function = 1; function < 8; function++)
                {
                    if (dev.vendor_id(function) != 0xFFFF)
                    {
                        check_function(dev, function, level);
                    }
                }
            }
        }

        void check_bus(std::uint8_t bus, int level)
        {
            for (std::uint8_t device = 0; device < 32; device++)
            {
                check_device({bus, device}, level);
            }
        }
    } // namespace

    void scan()
    {
        pci_device_ident root{0, 0};
        if ((root.header_type(0) & 0x80) == 0)
        {
            check_bus(0, 0);
        }
        else
        {
            for (std::uint8_t function = 0; function < 8; function++)
            {
                if (root.vendor_id(function) != 0xFFFF)
                {
                    break;
                }
                check_bus(function, 0);
            }
        }
    }
} // namespace pci

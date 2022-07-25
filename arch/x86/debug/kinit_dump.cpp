#include "kinit_dump.h"
#include "acpi/acpi.h"
#include <config.h>
#include <cpuid/cpuid.h>
#include <kinit/boot_resource.h>
#include <printf.h>
#include <mm/mm.h>

namespace debug
{
    void print_kinfo()
    {
        std::printf("kernel v%s (%s)\n", config::get_str<"version.full-version">, config::get_str<"arch">);
    }
    void dump_memory_map()
    {
        if constexpr(config::get_val<"debug.log.mmap">)
        {
            std::printf("memory map:\n");

            boot_resource::instance().iterate_mmap([](const stivale2_mmap_entry& e) {
                switch (e.type)
                {
                case 0x1:
                    std::printf("[\033cg;USABLE      \033]: ");
                    break;
                case 0x2:
                    std::printf("[\033cr;RESERVED    \033]: ");
                    break;
                case 0x3:
                    std::printf("[\033cy;ACPI RECLAIM\033]: ");
                    break;
                case 0x4:
                    std::printf("[\033cy;ACPI NVS    \033]: ");
                    break;
                case 0x5:
                    std::printf("[\033cr;BAD         \033]: ");
                    break;
                case 0x1000:
                    std::printf("[\033cy;BTL RECLAIM \033]: ");
                    break;
                case 0x1001:
                    std::printf("[\033cc;KERN MODULE \033]: ");
                    break;
                case 0x1002:
                    std::printf("[\033cc;FRAMEBUFFER \033]: ");
                    break;
                default:
                    std::printf("[\033cG;UNKNOWN     \033]: ");
                }
                std::printf("0x%016lx-0x%016lx length=0x%016lx\n", e.base, e.base + e.length, e.length);
            });
        }
    }

    void dump_cpuid_info()
    {
        if constexpr(config::get_val<"debug.log.cpuid">)
        {
            std::printf("cpu_vendor_string: %s\n", cpuid_info::cpu_vendor_string());
            std::printf("cpu_brand_string: %s\n", cpuid_info::cpu_brand_string());

            std::printf("cpu features: ");
            for (std::size_t i = 0; i < config::get_val<"cpuid-feature-size"> * 32; i++)
            {
                if (cpuid_info::test_feature(i) && cpuid_info::FEATURE_STRINGS[i])
                    std::printf("%s ", cpuid_info::FEATURE_STRINGS[i]);
            }
            std::printf("\n");
        }
    }

    void dump_acpi_info()
    {
        if constexpr(config::get_val<"debug.log.acpi">)
        {
            auto rsdp = boot_resource::instance().rsdp();
            std::printf("ACPI info:\n", rsdp->xsdt_address);
            std::printf("  rsdp data:\n");
            std::printf("    revision=%d\n", (int)rsdp->revision);
            std::printf("    length=%d\n", (int)rsdp->length);

            boot_resource::instance().iterate_xsdt([](const acpi::acpi_sdt_header* entry) {
                std::printf("  entry: (sig=0x%08u)\n", mm::make_virtual<acpi::acpi_sdt_header>((uint64_t)entry)->signature);
            });
        }
    }
} // namespace debug

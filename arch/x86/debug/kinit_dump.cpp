#include "kinit_dump.h"
#include "acpi/acpi.h"
#include "kinit/limine.h"
#include <config.h>
#include <cpuid/cpuid.h>
#include <kinit/boot_resource.h>
#include <mm/mm.h>
#include <printf.h>
#include <tty/tty.h>

namespace debug
{
    void print_kinfo()
    {
        std::printf("kernel v%s (%s)\n", config::get_str<"version.full-version">, config::get_str<"arch">);
        std::printf("built with cc: %s-%s cxx: %s-%s\n", config::get_str<"version.cc.id">, config::get_str<"version.cc.ver">,
                    config::get_str<"version.cxx.id">, config::get_str<"version.cxx.ver">);
    }

    void dump_memory_map()
    {
        if constexpr (config::get_val<"debug.log.mmap">)
        {
            std::printf("memory map:\n");

            boot_resource::instance().iterate_mmap([](const limine_memmap_entry& entry) {
                switch (entry.type)
                {
                case LIMINE_MEMMAP_USABLE:
                    std::printf("[" CYAN("USABLE      ") "]: ");
                    break;
                case LIMINE_MEMMAP_RESERVED:
                    std::printf("[" RED("RESERVED    ") "]: ");
                    break;
                case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
                    std::printf("[" YELLOW("ACPI RECLAIM") "]: ");
                    break;
                case LIMINE_MEMMAP_ACPI_NVS:
                    std::printf("[" YELLOW("ACPI NVS    ") "]: ");
                    break;
                case LIMINE_MEMMAP_BAD_MEMORY:
                    std::printf("[" RED("BAD         ") "]: ");
                    break;
                case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
                    std::printf("[" YELLOW("BTL RECLAIM ") "]: ");
                    break;
                case LIMINE_MEMMAP_KERNEL_AND_MODULES:
                    std::printf("[" CYAN("K/MOD       ") "]: ");
                    break;
                case LIMINE_MEMMAP_FRAMEBUFFER:
                    std::printf("[" CYAN("FRAMEBUFFER ") "]: ");
                    break;
                default:
                    // this case should never be reached, but exists as a fallback
                    std::printf("[" GRAY("UNKNOWN     ") "]: ");
                }
                std::printf("0x%016lx-0x%016lx length=0x%016lx\n", entry.base, entry.base + entry.length, entry.length);
            });
        }
    }

    void dump_cpuid_info()
    {
        if constexpr (config::get_val<"debug.log.cpuid">)
        {
            std::printf("cpu_vendor_string: %s\n", cpuid_info::cpu_vendor_string());
            std::printf("cpu_brand_string: %s\n", cpuid_info::cpu_brand_string());

            std::printf("cpu features: ");
            for (std::size_t i = 0; i < config::get_val<"cpuid-feature-size"> * 32; i++)
            {
                if (cpuid_info::test_feature(i) && (cpuid_info::FEATURE_STRINGS[i] != nullptr))
                {
                    std::printf("%s ", cpuid_info::FEATURE_STRINGS[i]);
                }
            }

            std::printf("\n");
        }
    }

    void dump_acpi_info()
    {
        if constexpr (config::get_val<"debug.log.acpi">)
        {
            auto* rsdp = boot_resource::instance().rsdp();
            std::printf("ACPI info (0x%016lx):\n", rsdp->xsdt_address);
            std::printf("  rsdp data:\n");
            std::printf("    revision=%d\n", (int)rsdp->revision);
            std::printf("    length=%d\n", (int)rsdp->length);

            boot_resource::instance().iterate_xsdt([](const acpi::acpi_sdt_header* entry) {
                auto signature = mm::make_virtual<acpi::acpi_sdt_header>(as_uptr(entry))->signature;
                const char* signature_ptr = cast_ptr(&signature);

                std::printf("  entry: (sig=0x%08x '%c%c%c%c')\n", signature, signature_ptr[0], signature_ptr[1],
                            signature_ptr[2], signature_ptr[3]);
            });
        }
    }
} // namespace debug

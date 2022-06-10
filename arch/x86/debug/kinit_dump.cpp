// cSpell:ignore stivale
#include "kinit_dump.h"
#include <kinit/boot_resource.h>
#include <printf.h>

namespace detail
{
    void dump_memory_map()
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
} // namespace detail

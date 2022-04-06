#include "gdt.h"

namespace gdt
{
    struct gdt_descriptor
    {
        uint16_t size;
        uint64_t offset;
    } __attribute__((packed));

    static uint64_t gdt[] = {0, 0x00affb000000ffff, 0x00aff3000000ffff};

    void install_gdt()
    {
        gdt_descriptor descriptor = {.size = sizeof(gdt), .offset = (uint64_t)gdt};
        __asm__ __volatile__("lgdtq %0" : : "m"(descriptor));
    }
} // namespace gdt

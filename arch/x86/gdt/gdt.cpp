#include "gdt.h"

namespace gdt
{
    struct gdt_descriptor
    {
        uint16_t size;
        uint64_t offset;
    } __attribute__((packed));

    static uint64_t gdt[] = {0, 0x00af9b000000ffff, 0x00af93000000ffff};

    void install_gdt()
    {
        gdt_descriptor descriptor = {.size = sizeof(gdt), .offset = (uint64_t)gdt};
        asm volatile("lgdtq %0" : : "m"(descriptor));

        asm volatile("pushq $8\n"
                             "pushq $.L123\n"
                             "lretq\n"
                             ".L123:\n"
                             "movw $16, %%ax\n"
                             "movw %%ax, %%es\n"
                             "movw %%ax, %%ss\n"
                             "movw %%ax, %%ds\n"
                             :
                             :
                             : "%ax");
    }
} // namespace gdt

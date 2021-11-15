#include "gdt.h"

namespace gdt
{
    struct gdt_descriptor
    {
    	uint16_t size;
    	uint64_t offset;
    } __attribute__((packed));

    static gdt_entry gdt[] = {
        0, // null entry
        make_cs(0x00), // code segment for kernel
        make_ds(0x00), // data segment for kernel
        make_cs(0x03), // code segment for user
        make_ds(0x03), // data segment for user
    };

    void install_gdt() {
        gdt_descriptor descriptor = {
            .size = sizeof(gdt),
            .offset = (uint64_t)gdt
        };
        asm volatile("lgdtq %0" : : "m"(descriptor));
    }
}

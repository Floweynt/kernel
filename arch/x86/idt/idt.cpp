#include "idt.h"
#include <asm/asm_cpp.h>
#include <smp/smp.h>

extern char idt_entry_start[];

namespace idt
{
    struct [[gnu::packed]] idt_descriptor
    {
        uint16_t size;
        uint64_t offset;
    };

    idt::idt_entry idt_entries[256] = {};

    void init_idt()
    {
        for (int i = 0; i < 256; i++)
        {
            uint64_t handler = (uint64_t)idt_entry_start + i * 0x10;
            idt_entries[i].offset_low = (uint64_t)handler;
            idt_entries[i].offset_mid = (uint64_t)handler >> 16;
            idt_entries[i].offset_high = ((uint64_t)handler) >> 32;
        }
    }

    void install_idt()
    {
        idt_descriptor descriptor = {.size = sizeof(idt_entries), .offset = (uint64_t)idt_entries};
        asm volatile("lidtq %0" : : "m"(descriptor));
    }

    void register_idt(interrupt_handler handler, std::size_t num, uint8_t type, uint8_t dpl)
    {
        smp::core_local::get().idt_handler_entries[num] = (uintptr_t)handler;
        idt_entries[num].flags = 0x8000 | ((uint16_t)dpl << 13) | ((uint16_t)type << 8);
    }
} // namespace idt

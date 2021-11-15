#include "idt.h"
#include "asm/asm_cpp.h"
#include "interface/driver/tty.h"

uintptr_t idt_handler_entries[256];

extern char idt_entry_start[];
extern char idt_entry_end[];

namespace idt
{
    struct idt_descriptor
    {
    	uint16_t size;
    	uint64_t offset;
    } __attribute__((packed));

    struct idt_entry
    {
       uint16_t offset_low;
       uint16_t cs = 0x8;
       uint16_t flags;
       uint16_t offset_mid;
       uint32_t offset_high;
       uint32_t reserved;
    } __attribute__((packed));

    static idt_entry idt_entries[256] = {};

    void init_idt()
    {
        uint64_t sizeof_handler = (uint64_t)(idt_entry_end - idt_entry_start);

        for(int i = 0; i < 256; i++)
        {
            uint64_t handler = (uint64_t)idt_entry_start + i * sizeof_handler;
        	idt_entries[i].offset_low = (uint64_t)handler;
            idt_entries[i].offset_mid = (uint64_t)handler >> 16;
        	idt_entries[i].offset_high = ((uint64_t)handler) >> 32;
        }
    }

    void install_idt()
    {
        idt_descriptor descriptor = {
            .size = sizeof(idt_entries),
            .offset = (uint64_t)idt_entries
        };
        asm volatile("lidtq %0" : : "m"(descriptor));
    }

    void register_idt(interrupt_handler handler, size_t num, uint8_t type, uint8_t dpl)
    {
        idt_handler_entries[num] = (uintptr_t)handler;
    	idt_entries[num].flags = 0x8000 | ((uint16_t)dpl << 13) | ((uint16_t)type << 8);
    }
}

#include "idt.h"

static idt_entry idt_entries[256] = {};
static idt_descriptor descriptor = {};
uintptr_t idt_handler_entries[256];

extern char idt_entry_start[];
extern char idt_entry_end[];

void init_idt()
{
	descriptor.offset = (uint64_t)idt_handler_entries;
	descriptor.size = sizeof(idt_handler_entries);
    uint64_t sizeof_handler = (uint64_t)(idt_entry_end - idt_entry_start);

    for(int i = 0; i < 256; i++)
    {
        uint64_t handler = (uint64_t)idt_entry_start + i * sizeof_handler;
    	idt_entries[i].offset_low = (uint64_t)handler;
        idt_entries[i].offset_mid = (uint64_t)handler >> 32;
    	idt_entries[i].offset_high = ((uint64_t)handler) >> 48;
    }
}

void install_idt()
{
	__asm__ __volatile__("lidtq (%0)" : : "r"(&descriptor));
}

void register_idt(interrupt_handler handler, uint8_t attr, size_t num)
{
    idt_handler_entries[num] = (uintptr_t)handler;
	idt_entries[num].flags = attr;
}

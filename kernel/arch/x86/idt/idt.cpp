#include "idt.h"

idt_entry idt_entries[256] = {};
idt_descriptor descriptor = {};
uintptr_t asm_handler_entries[256];

void init_idt()
{
	descriptor.offset = (uint32_t)idt_entries;
	descriptor.size = sizeof(idt_entries);
}

void install_idt()
{
	__asm__ __volatile__("lidt (%0)" : : "r"(&descriptor));
}

void register_idt(interrupt_handler handler, uint8_t attr, size_t num)
{
	idt_entries[num].attr = attr;
	idt_entries[num].offset_low = (uint32_t)handler;
	idt_entries[num].offset_high = ((uint32_t)handler) >> 16;
}

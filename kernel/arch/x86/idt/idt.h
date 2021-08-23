#ifndef __X86_IDT_H__
#define __X86_IDT_H__
#include <cstdint>
#include <cstddef>

using interrupt_handler = void (*)();

struct idt_entry
{
	uint16_t offset_low	= 0x0000;
	uint16_t cs	= 0x0008;
	uint8_t zero = 0x00;
	uint8_t attr = 0x00;
	uint16_t offset_high = 0x0000;
};

struct idt_descriptor
{
	uint16_t size;
	uint32_t offset;
};

void init_idt();
void install_idt();
void register_idt(interrupt_handler handler, uint8_t attrs, size_t num);

#endif

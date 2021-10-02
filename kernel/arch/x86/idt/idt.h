#ifndef __X86_IDT_H__
#define __X86_IDT_H__
#pragma pack(push, 1)
#include <cstdint>
#include <cstddef>

using interrupt_handler = void (*)(uint64_t, void*);

struct idt_entry
{
   uint16_t offset_low;
   uint16_t cs = 0x8;
   uint8_t reserved_ist;
   uint8_t flags;
   uint16_t offset_mid;
   uint32_t offset_high;
   uint32_t reserved;
};

struct idt_descriptor
{
	uint16_t size;
	uint64_t offset;
};

void init_idt();
void install_idt();
void register_idt(interrupt_handler handler, uint8_t attrs, size_t num);

#pragma pack(pop)
#endif

#ifndef __X86_IDT_H__
#define __X86_IDT_H__
#pragma pack(push, 1)
#include <cstdint>
#include <cstddef>

using interrupt_handler = void (*)(uint64_t, void*);

constexpr uint64_t MASK_DPL_R0 = 0x0;
constexpr uint64_t MASK_DPL_R3 = 0x3;
constexpr uint64_t MASK_TYPE = 0x1;

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
void register_idt(interrupt_handler handler, size_t num, uint8_t type = 0b1110, uint8_t dpl = 0x0);

#pragma pack(pop)
#endif

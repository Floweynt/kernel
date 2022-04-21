#ifndef __X86_IDT_H__
#define __X86_IDT_H__
#include <cstddef>
#include <cstdint>

namespace idt
{
    using interrupt_handler = void (*)(uint64_t, uint64_t);

    constexpr uint64_t MASK_DPL_R0 = 0x0;
    constexpr uint64_t MASK_DPL_R3 = 0x3;
    constexpr uint64_t MASK_TYPE = 0x1;

    void init_idt();
    void install_idt();
    void register_idt(interrupt_handler handler, std::size_t num, uint8_t type = 0b1110, uint8_t dpl = 0x0);

    struct [[gnu::packed]] idt_entry
    {
        uint16_t offset_low;
        uint16_t cs = 0x8;
        uint16_t flags;
        uint16_t offset_mid;
        uint32_t offset_high;
        uint32_t reserved;
    } __attribute__((packed));

} // namespace idt
#endif

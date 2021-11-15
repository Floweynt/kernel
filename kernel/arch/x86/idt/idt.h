#ifndef __X86_IDT_H__
#define __X86_IDT_H__
#include <cstdint>
#include <cstddef>

namespace idt {
    using interrupt_handler = void (*)(uint64_t, void*);

    constexpr uint64_t MASK_DPL_R0 = 0x0;
    constexpr uint64_t MASK_DPL_R3 = 0x3;
    constexpr uint64_t MASK_TYPE = 0x1;

    void init_idt();
    void install_idt();
    void register_idt(interrupt_handler handler, size_t num, uint8_t type = 0b1110, uint8_t dpl = 0x0);
}
#endif

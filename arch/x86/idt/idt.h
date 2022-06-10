#ifndef __X86_IDT_H__
#define __X86_IDT_H__
#include <cstddef>
#include <cstdint>

namespace idt
{
    using interrupt_handler = void (*)(uint64_t, uint64_t);

    inline constexpr uint64_t MASK_DPL_R0 = 0x0;
    inline constexpr uint64_t MASK_DPL_R3 = 0x3;
    inline constexpr uint64_t MASK_TYPE = 0x1;

    /// \brief Initialize the data structures required for the IDT to function
    ///
    void init_idt();

    /// \brief Installs the IDT
    ///
    void install_idt();

    /// \brief Registers a handler entry in the IDT
    /// \param handler The callback to run when the interrupt has occurred
    /// \param num The interrupt vector to register
    /// \param type The gate type
    /// \param dpl The DPL requirement
    /// \return Some value lol TODO: document
    bool register_idt(interrupt_handler handler, std::size_t num, uint8_t type = 0b1110, uint8_t dpl = 0x0);

    /// \brief Registers a handler entry at an available slot
    /// 
    std::size_t register_idt(interrupt_handler handler, uint8_t type = 0b1110, uint8_t dpl = 0x0);

    struct [[gnu::packed]] idt_entry
    {
        uint16_t offset_low;
        uint16_t cs = 0x8;
        uint16_t flags;
        uint16_t offset_mid;
        uint32_t offset_high;
        uint32_t reserved;
    };
} // namespace idt
#endif

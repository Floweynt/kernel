#pragma once

#include <cstddef>
#include <cstdint>

namespace idt
{
    using interrupt_handler = void (*)(std::uint64_t, std::uint64_t);

    inline constexpr std::uint64_t MASK_DPL_R0 = 0x0;
    inline constexpr std::uint64_t MASK_DPL_R3 = 0x3;
    inline constexpr std::uint64_t MASK_TYPE = 0x1;

    /// \brief Initialize the data structures required for the IDT to function
    ///
    void init_idt();

    /// \brief Installs the IDT
    ///
    void install_idt();

    class idt_builder
    {
        interrupt_handler handler;
        std::uint8_t gate : 4;
        std::uint8_t _ist : 3;
        std::uint8_t _dpl : 2;

    public:
        explicit constexpr idt_builder(interrupt_handler handler) : handler(handler), gate(0xe), _ist(0), _dpl(0) {}
        constexpr idt_builder& gate_intr()
        {
            gate = 0xe;
            return *this;
        }
        constexpr idt_builder& gate_trap()
        {
            gate = 0xf;
            return *this;
        }
        constexpr idt_builder& dpl(std::uint8_t d)
        {
            _dpl = d;
            return *this;
        }
        constexpr idt_builder& ist(std::uint8_t i)
        {
            _ist = i;
            return *this;
        }

        constexpr std::uint16_t flag() const
        {
            return 0x8000 | ((std::uint16_t)_dpl << 14) | ((std::uint16_t)gate << 8) | _ist;
        }

        constexpr interrupt_handler cb() const { return handler; }
    };

    /// \brief Registers a handler entry in the IDT
    /// \param handler The callback to run when the interrupt has occurred
    /// \param num The interrupt vector to register
    /// \param type The gate type
    /// \param dpl The DPL requirement
    /// \return wether or not the handler was registered
    bool register_idt(const idt_builder&, std::size_t num);

    /// \brief Registers a handler entry at an available slot
    ///
    std::size_t register_idt(const idt_builder&);

    struct [[gnu::packed]] idt_entry
    {
        std::uint16_t offset_low;
        const std::uint16_t cs = 0x8;
        std::uint16_t flags;
        std::uint16_t offset_mid;
        std::uint32_t offset_high;
        std::uint32_t reserved;
    };
} // namespace idt

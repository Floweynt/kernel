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
        inline static constexpr auto GATE_INTR = 0xe;
        inline static constexpr auto GATE_TRAP = 0xf;

        interrupt_handler handler;
        std::uint8_t gate : 4 {GATE_INTR};
        std::uint8_t _ist : 3 {};
        std::uint8_t _dpl : 2 {};

    public:
        explicit constexpr idt_builder(interrupt_handler handler) : handler(handler) {}
        constexpr auto gate_intr() -> idt_builder&
        {
            gate = GATE_INTR;
            return *this;
        }
        constexpr auto gate_trap() -> idt_builder&
        {
            gate = GATE_TRAP;
            return *this;
        }
        constexpr auto dpl(std::uint8_t dpl) -> idt_builder&
        {
            _dpl = dpl;
            return *this;
        }
        constexpr auto ist(std::uint8_t ist) -> idt_builder&
        {
            _ist = ist;
            return *this;
        }

        [[nodiscard]] constexpr auto flag() const -> std::uint16_t
        {
            return 0x8000 | ((std::uint16_t)_dpl << 14) | ((std::uint16_t)gate << 8) | _ist;
        }

        [[nodiscard]] constexpr auto get_handler() const -> interrupt_handler { return handler; }
    };

    /// \brief Registers a handler entry in the IDT
    /// \param handler The callback to run when the interrupt has occurred
    /// \param num The interrupt vector to register
    /// \param type The gate type
    /// \param dpl The DPL requirement
    /// \return wether or not the handler was registered
    auto register_idt(const idt_builder&, std::size_t num) -> bool;

    /// \brief Registers a handler entry at an available slot
    ///
    auto register_idt(const idt_builder&) -> std::size_t;

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

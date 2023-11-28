#pragma once

#include <cstdint>
namespace apic
{
    enum class lvt_timer_mode
    {
        ONE_SHOT = 0b00,
        PERIODIC = 0b01,
        TSC_DEADLINE = 0b10,
    };

    constexpr auto build_lvt_timer(lvt_timer_mode mode, bool is_masked, bool is_send_pending, std::uint8_t vector) -> std::uint32_t
    {
        return (static_cast<std::uint32_t>(mode) << 17) | (static_cast<std::uint32_t>(is_masked) << 16) |
               (static_cast<std::uint32_t>(is_send_pending) << 12) | (vector);
    }

    enum class lvt_delivery_mode
    {
        FIXED = 0b000,
        SMI = 0b010,
        NMI = 0b100,
        EXTINT = 0b111,
        INIT = 0b101
    };

    constexpr auto build_error(bool is_masked, bool is_send_pending, std::uint8_t vector)
    {
        return (static_cast<std::uint32_t>(is_masked) << 16) | (static_cast<std::uint32_t>(is_send_pending) << 12) | vector;
    }

    constexpr auto build_lvt_cmci(bool is_masked, bool is_send_pending, lvt_delivery_mode mode, std::uint8_t vector) -> std::uint32_t
    {
        return build_error(is_masked, is_send_pending, vector) | (static_cast<std::uint32_t>(mode) << 8);
    }

    constexpr auto build_lvt_perf_mon_counter(bool is_masked, bool is_send_pending, lvt_delivery_mode mode, std::uint8_t vector) -> std::uint32_t
    {
        return build_lvt_cmci(is_masked, is_send_pending, mode, vector);
    }

    constexpr auto build_lvt_thermal(bool is_masked, bool is_send_pending, lvt_delivery_mode mode, std::uint8_t vector) -> std::uint32_t
    {
        return build_lvt_cmci(is_masked, is_send_pending, mode, vector);
    }

    constexpr auto build_lint(bool is_masked, bool is_level, bool remote_irr, bool int_in_pin_polarity, bool is_send_pending, lvt_delivery_mode mode,
                              std::uint8_t vector) -> std::uint32_t
    {
        return build_lvt_cmci(is_masked, is_send_pending, mode, vector) | (static_cast<std::uint32_t>(is_level) << 15) |
               (static_cast<std::uint32_t>(remote_irr) << 14) | (static_cast<std::uint32_t>(int_in_pin_polarity) << 13);
    }
} // namespace apic

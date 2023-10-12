#pragma once

#include <misc/cast.h>
#include <cstdint>
#include <bitbuilder.h>

namespace gdt
{
    /// \brief generates and installs a GDT
    ///
    void install_gdt();
    void reload_gdt_smp();

    // placed here due to IST (TSS) being a part of the GDT
    struct [[gnu::packed]] interrupt_stack_table
    {
        std::uint32_t reserved0;
        std::uintptr_t rsp0;
        std::uintptr_t rsp1;
        std::uintptr_t rsp2;
        std::uint64_t reserved1;
        std::uintptr_t ist1;
        std::uintptr_t ist2;
        std::uintptr_t ist3;
        std::uintptr_t ist4;
        std::uintptr_t ist5;
        std::uintptr_t ist6;
        std::uintptr_t ist7;
        std::uint64_t reserved2;
        std::uint16_t reserved3;
        std::uint16_t io_bp;
    };

    //static_assert(0x00af9b000000ffff == std::build_pattern<"#l16 #b16 #b8 ardesxxp llll 0m">());

    struct [[gnu::packed]] gdt_entries
    {
        const std::uint64_t null = 0;
        const std::uint64_t cs = 0x00af9b000000ffff;
        const std::uint64_t ds = 0x00af93000000ffff;
        const std::uint64_t user_cs = 0x00affb000000ffff;
        const std::uint64_t user_ds = 0x00aff3000000ffff;

        const std::uint16_t limit = 0x6b;
        std::uint16_t base_lowest{};
        std::uint8_t base_lo{};
        const std::uint8_t access = 0x89;
        const std::uint8_t mixed = 0b00000000;
        std::uint8_t base_mid{};
        std::uint32_t base_high{};
        const std::uint32_t reserved0 = 0;

        inline void set_ist(interrupt_stack_table* ist)
        {
            base_lowest = as_uptr(ist);
            base_lo = as_uptr(ist) >> 16;
            base_mid = as_uptr(ist) >> 24;
            base_high = as_uptr(ist) >> 32;
        }
    };

    inline static constexpr std::uint64_t KERNEL_CS = 0x8;
    inline static constexpr std::uint64_t KERNEL_DS = 0x10;
    inline static constexpr std::uint64_t USER_CS = 0x18;
    inline static constexpr std::uint64_t USER_DS = 0x20;
} // namespace gdt

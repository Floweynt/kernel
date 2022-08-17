#ifndef __X86_GDT_H__
#define __X86_GDT_H__
#include <cstdint>
namespace gdt
{
    /// \brief generates and installs a GDT
    ///
    void install_gdt();
    void reload_gdt_smp();

    // placed here due to IST (TSS) being a part of the GDT
    struct [[gnu::packed]] interrupt_stack_table
    {
        uint32_t reserved0;
        uintptr_t rsp0;
        uintptr_t rsp1;
        uintptr_t rsp2;
        uint64_t reserved1;
        uintptr_t ist1;
        uintptr_t ist2;
        uintptr_t ist3;
        uintptr_t ist4;
        uintptr_t ist5;
        uintptr_t ist6;
        uintptr_t ist7;
        uint64_t reserved2;
        uint16_t reserved3;
        uint16_t io_bp;
    };

    struct [[gnu::packed]] gdt_entries
    {
        const uint64_t null = 0;
        const uint64_t cs = 0x00af9b000000ffff;
        const uint64_t ds = 0x00af93000000ffff;
        const uint64_t user_cs = 0x00affb000000ffff;
        const uint64_t user_ds = 0x00aff3000000ffff;

        const uint16_t limit = 0x6b;
        uint16_t base_lowest;
        uint8_t base_lo;
        const uint8_t access = 0x89;
        const uint8_t mixed = 0b00000000;
        uint8_t base_mid;
        uint32_t base_high;
        const uint32_t reserved0 = 0;

        inline void set_ist(interrupt_stack_table* ist)
        {
            base_lowest = (uintptr_t)ist;
            base_lo = ((uintptr_t)ist) >> 16;
            base_mid = ((uintptr_t)ist) >> 24;
            base_high = ((uintptr_t)ist) >> 32;
        }
    };
} // namespace gdt

#endif

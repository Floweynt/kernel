#ifndef __X86_GDT_H__
#define __X86_GDT_H__
#include <cstdint>
namespace gdt
{
    using gdt_entry = uint64_t;
    inline constexpr uint64_t make_cs(uint8_t dpl)
    {
        return 0x209b0000000000 | ((uint64_t)(dpl & 0x3) << 45);
    }

    inline constexpr uint64_t make_ds(uint8_t dpl)
    {
        return 0x920000000000 | ((uint64_t)(dpl & 0x3) << 45);
    }

    void install_gdt();
}

#endif

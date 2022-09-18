#ifndef __ARCH_X86_DEBUG_DEBUG_H__
#define __ARCH_X86_DEBUG_DEBUG_H__

#include <cstdint>
namespace debug
{
    enum stack_types
    {
        KINIT,
        IRQ,
        SMP
    };

    void panic(const char* msg, bool crash = true);


    struct symbol
    {
        const char* name;
        std::uint32_t offset;
    };

    symbol sym_for(std::uint64_t address);
}

#define mark_stack(type) asm volatile("movq %0, (%%rbp)" : : "r"((std::uint64_t)type));

#endif

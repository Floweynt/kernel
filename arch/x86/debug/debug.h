#ifndef __ARCH_X86_DEBUG_DEBUG_H__
#define __ARCH_X86_DEBUG_DEBUG_H__

namespace debug
{
    enum stack_types
    {
        KINIT,
        IRQ,
        SMP
    };

    void panic(const char* msg, bool crash = true);
}

#define mark_stack(type) asm volatile("movq %0, (%%rbp)" : : "r"((uint64_t)type));

#endif

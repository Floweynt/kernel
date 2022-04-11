#ifndef __ARCH_X86_CONTEXT_H__
#define __ARCH_X86_CONTEXT_H__
#include <cstdint>

// this is a fully restorable execution context
struct context
{
    enum registers
    {
        RAX = 0,
        RBX,
        RCX,
        RDX,
        RSI,
        RDI,
        R8,
        R9,
        R10,
        R11,
        R12,
        R13,
        R14,
        R15
    };

    uint64_t ss;
    uint64_t rsp;
    uint64_t rflags;
    uint64_t cs;
    uint64_t rip;

    uint64_t rgp[14];
    uint64_t rbp;

    // yoinked from intel docs
    // high
    // 0x00: SS             <- RSP before interrupt
    // 0x08: RSP
    // 0x10: RFLAGS
    // 0x18: CS
    // 0x20: RIP
    // 0x28: ERROR ?? 0
    // 0x30: interrupt #
    // 0x38: filler
    // 0x40: RAX - RDX
    // 0x60: RSI, RDI
    // 0x70: R8-R15
    // 0xB0: RBP            <- stackpos
    // 0xB8: Call RIP
    //  returns: error code
    inline uint64_t load_ctx(void* stackpos)
    {
        uint64_t* stack = (uint64_t*)stackpos;
        rbp = *stack--;

        for (int i = 13; i != -1; i--)
            rgp[i] = *stack--;
        stack--;
        stack--;
        uint64_t ret = *stack--;
        rip = *stack--;
        cs = *stack--;
        rflags = *stack--;
        rsp = *stack--;
        ss = *stack--;
        return ret;
    }

    inline uint64_t restore_ctx(void* stackpos)
    {
        uint64_t* stack = (uint64_t*)stackpos;
        *stack-- = rbp;

        for (int i = 13; i != -1; i--)
            *stack-- = rgp[i];
        stack -= 3;
        *stack-- = rip;
        *stack-- = cs;
        *stack-- = rflags;
        *stack-- = rsp;
        *stack-- = ss;
    }
};
#endif

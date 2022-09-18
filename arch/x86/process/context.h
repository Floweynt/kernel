#ifndef __ARCH_X86_CONTEXT_H__
#define __ARCH_X86_CONTEXT_H__
#include <cstdint>

// this is a fully restorable execution context
namespace proc
{
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
            R15,
            RBP
        };

        inline static constexpr const char* REGISTER_NAMES[] = {"rax", "rbx", "rcx", "rdx", "rsi", "rdi", "r8", "r9",
                                                                "r10", "r11", "r12", "r13", "r14", "r15", "rbp"};

        std::uint64_t rip;
        std::uint64_t cs;
        std::uint64_t rflags;
        std::uint64_t rsp;
        std::uint64_t ss;
        std::uint64_t rgp[15];

        // high
        // 0x00: SS             <- RSP before interrupt
        // 0x08: RSP
        // 0x10: RFLAGS
        // 0x18: CS
        // 0x20: RIP
        // 0x28: ERROR ?? 0
        // 0x30: interrupt #
    };
} // namespace proc
#endif

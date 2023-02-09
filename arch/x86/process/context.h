#pragma once

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

        std::uintptr_t cr3; // the current page tables for the process represented by this execution context
                            // for kernel space, it should point to the global kernel space page tables
    };
} // namespace proc

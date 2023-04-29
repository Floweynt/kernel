#pragma once

#include "asm/asm_cpp.h"
#include <cstdint>
#include <gdt/gdt.h>

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

    class context_builder
    {
        context my_context;

    public:
        enum mode
        {
            KERNEL,
            USER
        };

        constexpr context_builder(mode mode, std::uintptr_t entry)
            : my_context{
                  .rip = entry,
                  .cs = static_cast<std::uint64_t>(mode == KERNEL ? gdt::KERNEL_CS : (gdt::USER_CS | 3)),
                  .rflags = cpuflags::FLAGS,
                  .ss = static_cast<std::uint64_t>(mode == KERNEL ? gdt::KERNEL_DS : (gdt::USER_DS | 3)),
              }
        {
        }

        constexpr auto set_reg(context::registers reg, std::uint64_t value) -> auto&
        {
            my_context.rgp[reg] = value;
            return *this;
        }

        constexpr auto set_stack(std::uintptr_t buffer) -> auto&
        {
            my_context.rsp = buffer;
            return *this;
        }

        constexpr auto set_cr3(std::uintptr_t buffer) -> auto&
        {
            my_context.cr3 = buffer;
            return *this;
        }

        constexpr auto set_flag(cpuflags::cpuflags flag) -> auto&
        {
            my_context.rflags |= flag;
            return *this;
        }

        constexpr auto build() -> const auto& { return my_context; }
    };
} // namespace proc

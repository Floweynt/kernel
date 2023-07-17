#pragma once

#include "process/context.h"
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
    void log_register(proc::context* ctx);

    struct symbol
    {
        const char* name;
        std::uint32_t offset;
    };

    auto sym_for(std::uint64_t address) -> symbol;
}

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define mark_stack(type) asm volatile("movq %0, (%%rbp)" : : "r"((std::uint64_t)(type)));  

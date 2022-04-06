#ifndef __KERNEL_PROCESS__
#define __KERNEL_PROCESS__
#include <cstdint>

struct process_info
{
    process_registers registers;
};

using flags_t = uint64_t;

struct process_registers
{
    // GP registers
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;

    uint64_t rsp;
    uint64_t rbp;

    uint64_t rsi;
    uint64_t rdi;

    // flags
    flags_t rflags;

    // segment registers

    uint64_t ss;
    uint64_t cs;
    uint64_t ds;
    uint64_t es;
    uint64_t fs;
    uint64_t gs;

    // ip
    uint64_t rip;
};

struct idt_stack_regs
{
    uint64_t rip;
    uint64_t cs;
    flags_t rflags;
    uint64_t rsp;
    uint64_t ss;
}

#endif

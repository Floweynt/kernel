#pragma once

#include <debug/debug.h>
#include <idt/idt.h>
#include <smp/smp.h>
#include <sync_wrappers.h>
#include <utility>

namespace handlers
{
    void handle_div_by_zero(std::uint64_t, std::uint64_t);
    void handle_debug(std::uint64_t, std::uint64_t);
    void handle_breakpoints(std::uint64_t, std::uint64_t);
    void handle_overflow(std::uint64_t, std::uint64_t);
    void handle_bounds(std::uint64_t, std::uint64_t);
    void handle_ud(std::uint64_t, std::uint64_t);
    void handle_nm(std::uint64_t, std::uint64_t);
    void handle_double_fault(std::uint64_t, std::uint64_t);
    void handle_bad_tss(std::uint64_t, std::uint64_t);
    void handle_np(std::uint64_t, std::uint64_t);
    void handle_stack(std::uint64_t, std::uint64_t);
    void handle_gp(std::uint64_t, std::uint64_t);
    void handle_page_fault(std::uint64_t, std::uint64_t);

    inline void handle_noop(std::uint64_t c, std::uint64_t)
    {
        sync::printf("c=%lu\n", c);
        while (1) {}
    }
    void handle_timer(std::uint64_t, std::uint64_t);

    inline constexpr idt::interrupt_handler INTERRUPT_HANDLERS[] = {
        handle_div_by_zero, handle_debug, handle_noop,         handle_breakpoints, handle_overflow, handle_bounds,
        handle_ud,          handle_nm,    handle_double_fault, handle_noop,        handle_bad_tss,  handle_np,
        handle_stack,       handle_gp,    handle_page_fault,   handle_noop,        handle_noop,     handle_noop,
        handle_noop,        handle_noop,  handle_noop,         handle_noop,        handle_noop,     handle_noop,
        handle_noop,        handle_noop,  handle_noop,         handle_noop,        handle_noop,     handle_noop,
        handle_noop,        handle_noop,
    };
} // namespace handlers

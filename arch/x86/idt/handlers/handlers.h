#ifndef __ARCH_X86_IDT_HANDLERS_HANDLERS_H__
#define __ARCH_X86_IDT_HANDLERS_HANDLERS_H__
#include <idt/idt.h>
#include <debug/debug.h>
#include <smp/smp.h>
#include <sync_wrappers.h>
#include <utility>

namespace handlers
{
    // void handle_div_by_zero(uint64_t, uint64_t);
    inline void handle_noop(uint64_t c, uint64_t) { sync::printf("c=%lu\n", c); while(1) {} }
    void handle_gp(uint64_t, uint64_t);
    void handle_timer(uint64_t, uint64_t);

    inline constexpr idt::interrupt_handler INTERRUPT_HANDLERS[] = {
        handle_noop, handle_noop, handle_noop, handle_noop, handle_noop, handle_noop, handle_noop, handle_noop,
        handle_noop, handle_noop, handle_noop, handle_noop, handle_noop, handle_gp,   handle_noop, handle_noop,
        handle_noop, handle_noop, handle_noop, handle_noop, handle_noop, handle_noop, handle_noop, handle_noop,
        handle_noop, handle_noop, handle_noop, handle_noop, handle_noop, handle_noop, handle_noop, handle_noop,
    };
} // namespace handlers

#endif

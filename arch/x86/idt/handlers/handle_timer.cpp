#include "handlers.h"
#include "klog/klog.h"
#include <printf.h>
#include <process/scheduler/scheduler.h>

namespace handlers
{
    void handle_timer(uint64_t, uint64_t)
    {
        smp::core_local::get().apic.end();
        //scheduler::schedule();
        static std::size_t n = 0;
        n = (n + 1) % 16;
        for(std::size_t i = 0; i < n + 100; i++)
            asm volatile("nop");
    }
} // namespace handlers

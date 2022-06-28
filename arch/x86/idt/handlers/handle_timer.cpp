#include "handlers.h"
#include <printf.h>
#include <process/scheduler/scheduler.h>

namespace handlers
{
    void handle_timer(uint64_t, uint64_t)
    {
        smp::core_local::get().apic.end();
        scheduler::schedule();
    }
} // namespace handlers

#include "handlers.h"
#include "klog/klog.h"
#include <printf.h>
#include <process/scheduler/scheduler.h>

namespace handlers
{
    void handle_timer(std::uint64_t, std::uint64_t)
    {
        smp::core_local::get().apic.end();
        smp::core_local::get().scheduler.load_sched_task_ctx();
    }
} // namespace handlers

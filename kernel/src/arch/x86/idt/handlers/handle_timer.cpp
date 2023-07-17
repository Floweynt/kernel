#include "klog/klog.h"
#include <idt/handlers/handlers.h>
#include <printf.h>
#include <process/scheduler/scheduler.h>

namespace handlers
{
    void handle_timer(std::uint64_t /*unused*/, std::uint64_t /*unused*/)
    {
        auto& local = smp::core_local::get();
        local.timer_tick_count++;
        local.apic.end();
        local.scheduler.load_sched_task_ctx();
    }
} // namespace handlers

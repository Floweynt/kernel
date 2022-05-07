#include "scheduler.h"
#include <process/process.h>
#include <smp/smp.h>

namespace scheduler
{
    void schedule()
    {
        auto& local = smp::core_local::get();
        auto& current_task = local.tasks.front();
        local.tasks.pop();
        local.tasks.push(local.current_tid);
        local.current_tid = current_task;
        local.ctxbuffer = &proc::get_process(current_task.proc).threads[current_task.thread].ctx;
    }
}

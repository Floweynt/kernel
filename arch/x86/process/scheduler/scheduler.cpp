#include "scheduler.h"
#include "klog/klog.h"
#include <process/process.h>
#include <smp/smp.h>

namespace scheduler
{
    void schedule()
    {
        auto& local = smp::core_local::get();
        if(local.tasks == nullptr)
            klog::log("cringe");
        auto next_task = local.tasks->front();
        local.tasks->pop();
        local.tasks->push(local.current_tid);
        local.current_tid = next_task;
        local.ctxbuffer = &proc::get_process(next_task.proc).threads[next_task.thread].ctx;
    }
}

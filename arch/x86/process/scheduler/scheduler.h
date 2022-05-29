#ifndef __ARCH_X86_PROCESS_SCHEDULER_SCHEDULER_H__
#define __ARCH_X86_PROCESS_SCHEDULER_SCHEDULER_H__
#include <process/process.h>
#include <cyclic_queue>

namespace scheduler
{
    void schedule();
    //bool task_sort(proc::task_id, proc::task_id);

    using task_queue = std::cyclic_queue<proc::task_id, 512, true>;
}

#endif

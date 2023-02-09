#pragma once

#include <cstdint>
#include <cyclic_queue>
#include <process/process.h>
#include <sync/spinlock.h>
#include <utility>
#include <vector>

namespace scheduler
{
    class scheduler
    {
        stdext::circular_queue<proc::thread*, 32> tasks;
        proc::thread* idle;

    public:
        void add_task(proc::thread* th);
        void set_state(proc::task_id tid, proc::thread_state state);
        void load_sched_task_ctx();
        void set_idle(proc::thread* th);
    };
} // namespace scheduler

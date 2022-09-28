#ifndef __ARCH_X86_PROCESS_SCHEDULER_SCHEDULER_H__
#define __ARCH_X86_PROCESS_SCHEDULER_SCHEDULER_H__
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
        struct heap_entry
        {
            proc::thread* th;
            std::int64_t task_priority;
        };

        std::vector<heap_entry> task_heap;
        lock::spinlock schedule_lock;
        std::size_t sched_ticks;

        inline std::int64_t compute_priority(proc::thread& th)
        {
            if (th.state == proc::thread_state::RUNNING)
                return std::INT64_MAX - th.latest_scheduled_tick;
            else if (th.state == proc::thread_state::IDLE)
                return std::INT64_MIN + 1;
            else
                return std::INT64_MIN;
        }

        constexpr std::size_t parent(std::size_t n) { return (n - 1) / 2; }
        constexpr std::size_t childr(std::size_t n) { return 2 * n + 2; }
        constexpr std::size_t childl(std::size_t n) { return 2 * n + 1; }
        constexpr bool is_root(std::size_t n) { return n == 0; }
        constexpr bool is_lowest(std::size_t n) { return childl(n) >= task_heap.size(); }

        inline void heap_swap(std::size_t i, std::size_t new_i)
        {
            std::swap(task_heap[i], task_heap[new_i]);
            std::swap(task_heap[i].th->sched_index, task_heap[new_i].th->sched_index);
        }

        void heap_update(std::size_t index, std::int64_t p);
        proc::thread* heap_pop();
        void heap_push(proc::thread* th);

    public:
        inline void add_task(proc::thread* th) { heap_push(th); }

        void update(proc::task_id tid)
        {
            lock::spinlock_guard g(schedule_lock);
            auto th = proc::get_thread(tid);
            auto idx = th.sched_index;
            heap_update(idx, compute_priority(th));
        }

        void load_sched_task_ctx();
    };
} // namespace scheduler

#endif

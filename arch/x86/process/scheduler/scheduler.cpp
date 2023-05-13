#include "scheduler.h"
#include "klog/klog.h"
#include <cstddef>
#include <process/process.h>
#include <smp/smp.h>

namespace scheduler
{
    /* TODO: switch to heap sched
    void scheduler::heap_update(std::size_t target_index, std::int64_t priority)
    {
        auto old_priority = task_heap[target_index].task_priority;
        task_heap[target_index].task_priority = priority;
        if (priority > old_priority)
        {
            std::size_t index = target_index;
            while (!is_root(index) && priority > task_heap[parent(index)].task_priority)
            {
                std::size_t new_index = parent(index);
                heap_swap(index, new_index);
                index = new_index;
            }
        }
        else if (priority < old_priority)
        {
            std::size_t index = target_index;
            while (!is_lowest(index))
            {
                if (task_heap[childl(index)].task_priority <= priority && task_heap[childr(index)].task_priority <= priority)
                    break;
                std::size_t new_index = task_heap[childl(index)].task_priority > task_heap[childr(index)].task_priority
                                            ? childl(index)
                                            : childr(index);
                heap_swap(index, new_index);
                index = new_index;
            }
        }
    }

    proc::thread* scheduler::heap_pop()
    {
        heap_swap(0, task_heap.size() - 1);
        proc::thread* ret = task_heap.end()->th;
        task_heap.size();

        std::size_t index = 0;
        std::int64_t priority = task_heap[0].task_priority;
        while (!is_lowest(index))
        {
            if (task_heap[childl(index)].task_priority <= priority && task_heap[childr(index)].task_priority <= priority)
                break;
            std::size_t new_index = task_heap[childl(index)].task_priority > task_heap[childr(index)].task_priority
                                        ? childl(index)
                                        : childr(index);
            heap_swap(index, new_index);
            index = new_index;
        }

        return ret;
    }

    void scheduler::heap_push(proc::thread* th)
    {
        task_heap.resize(task_heap.size() + 1);
    }

    void scheduler::load_sched_task_ctx()
    {
        if (task_heap.size() == 0)
            klog::panic("idle() task not found!");

        auto& local = smp::core_local::get();
        proc::thread* next_thread = task_heap[0].th;
        proc::thread* current_thread = local.current_thread;
        if (current_thread)
            heap_update(current_thread->sched_index, compute_priority(*current_thread));

        next_thread->latest_scheduled_tick = sched_ticks++;
        local.current_thread = next_thread;
        local.ctxbuffer = &next_thread->ctx;
    }*/

    void scheduler::add_task(proc::thread* thread)
    {
        if (thread->state == proc::thread_state::WAITING)
        {
            return; // no-op
        }
        tasks.push(thread);
    }

    void scheduler::set_state(proc::task_id tid, proc::thread_state state)
    {
        SPINLOCK_SYNC_BLOCK;
        auto* thread = &proc::get_thread(tid);
        if (thread->state == state)
        {
            return;
        }

        if (state == proc::thread_state::RUNNING)
        {
            tasks.push(thread);
        }

        thread->state = state;
    }

    void scheduler::load_sched_task_ctx()
    {
        auto& local = smp::core_local::get();
        if ((local.current_thread != nullptr) && local.current_thread->state == proc::thread_state::RUNNING)
        {
            tasks.push(local.current_thread);
        }

        proc::thread* next_thread = idle;
        while (tasks.pop(next_thread))
        {
            if (next_thread->state == proc::thread_state::RUNNING)
            {
                break;
            }
        }

        local.current_thread = next_thread;
        local.ctxbuffer = &next_thread->ctx;
        // klog::log("sched task %d:%d\n", next_thread->id.proc, next_thread->id.thread);
        // debug::log_register(&next_thread->ctx);
    }

    void scheduler::set_idle(proc::thread* thread)
    {
        if (thread->state != proc::thread_state::IDLE)
        {
            klog::panic("cannot set non-idle task as idle");
        }
        idle = thread;
    }
} // namespace scheduler

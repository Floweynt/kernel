#pragma once

#include "spinlock.h"
#include <atomic>
#include <cyclic_queue>
#include <smp/smp.h>

namespace lock
{
    template<std::size_t N, bool queue_heap = false>
    class semaphore
    {
        std::size_t count;
        spinlock internal_spinlock;
        stdext::circular_queue<proc::thread*, N, queue_heap> q;
    public:
        constexpr semaphore() : count(N) {}
        void lock()
        { 
            spinlock_guard guard(internal_spinlock);
            if(count) {
                count--;
            } else
            {
                q.push(smp::core_local::get().current_thread);
                // resched
            }
        }

        void release()
        {
            spinlock_guard guard(internal_spinlock);
            count++;
            // scheduler stuff
        }
    };

    class dynamic_semaphore
    {
        std::size_t count;
        spinlock internal_spinlock;
        stdext::dynamic_circular_queue<proc::thread*> q;
    public:
        inline dynamic_semaphore(std::size_t count) : count(count), q(count) {}
        void lock()
        { 
            spinlock_guard guard(internal_spinlock);
            if(count) {
                count--;
            } else
            {
                q.push(smp::core_local::get().current_thread);
                // resched
            }
        }

        void release()
        {
            spinlock_guard guard(internal_spinlock);
            count++;
            // scheduler stuff
        }
    };

    class mutex
    {
        semaphore<1> s;
    public:
        constexpr mutex() = default;
        inline void lock() { s.lock(); }
        inline void release() { s.release(); }
    };
}

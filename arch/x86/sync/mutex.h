#ifndef __ARCH_X86_SPINLOCK_H__
#define __ARCH_X86_SPINLOCK_H__

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
            spinlock_guard g(internal_spinlock);
            if(count)
                count--;
            else
            {
                q.push(smp::core_local::get().current_thread);
                // resched
            }
        }

        void release()
        {
            spinlock_guard g(internal_spinlock);
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
        inline dynamic_semaphore(std::size_t n) : count(n), q(n) {}
        void lock()
        { 
            spinlock_guard g(internal_spinlock);
            if(count)
                count--;
            else
            {
                q.push(smp::core_local::get().current_thread);
                // resched
            }
        }

        void release()
        {
            spinlock_guard g(internal_spinlock);
            count++;
            // scheduler stuff
        }
    };

    class mutex
    {
        semaphore<1> s;
    public:
        constexpr mutex() {}
        inline void lock() { s.lock(); }
        inline void release() { s.release(); }
    };
}

#endif

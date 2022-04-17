#ifndef __ARCH_X86_SYNC_SPINLOCK_H__
#define __ARCH_X86_SYNC_SPINLOCK_H__

namespace lock
{
    class spinlock
    {
        volatile int l;
    public:
        constexpr spinlock() : l(0) {}
        
        void lock()
        {
            while(!__sync_bool_compare_and_swap(&l, 0, 1));
            __sync_synchronize();
        }

        void release()
        {
            __sync_synchronize();
            l = 0;
        }
    };

    class spinlock_guard
    {
        spinlock& lock;
    public:
        inline spinlock_guard(spinlock& s) : lock(s)
        {
            lock.lock();
        }

        inline ~spinlock_guard()
        {
            lock.release();
        }
    };
};

#endif

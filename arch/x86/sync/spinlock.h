#ifndef __ARCH_X86_SYNC_SPINLOCK_H__
#define __ARCH_X86_SYNC_SPINLOCK_H__

namespace lock
{
    class spinlock
    {
        volatile int l;
    public:
        constexpr spinlock() : l(0) {}
        
        [[gnu::always_inline]]
        void lock()
        {
            while(!__sync_bool_compare_and_swap(&l, 0, 1));
            __sync_synchronize();
        }

        [[gnu::always_inline]]
        void release()
        {
            __sync_synchronize();
            l = 0;
        }
    };
};

#endif

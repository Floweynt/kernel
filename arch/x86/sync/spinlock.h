#ifndef __ARCH_X86_SYNC_SPINLOCK_H__
#define __ARCH_X86_SYNC_SPINLOCK_H__
#include <cstddef>
#include <smp/smp.h>

namespace lock
{
    class spinlock
    {
        volatile unsigned l;
    public:
        constexpr spinlock() : l(0) {}

        void lock(unsigned core = smp::core_local::get().coreid)
        {
            while (!__sync_bool_compare_and_swap(&l, 0, core + 1));
            __sync_synchronize();
        }

        void release()
        {
            __sync_synchronize();
            l = 0;
        }

        inline std::size_t owned_core()
        {
            return l - 1;
        }
    };

    class spinlock_guard
    {
        spinlock& lock;
    public:
        inline spinlock_guard(spinlock& s) : lock(s) { lock.lock(); }
        inline spinlock_guard(spinlock& s, unsigned core) : lock(s) { lock.lock(core); }
        inline ~spinlock_guard() { lock.release(); }
    };

}; // namespace lock

#endif

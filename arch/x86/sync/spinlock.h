#pragma once

#include "kinit/boot_resource.h"
#include <atomic>
#include <cstddef>

namespace lock
{
    class spinlock
    {
        std::atomic<int> l;
    public:
        constexpr spinlock() : l(0) {}

        void lock();

        inline void release()
        {
            l.store(false, std::memory_order_release);
        }

        inline std::size_t owned_core() { return l.load() - 1; }
    };

    class interrupt_lock
    {
    };

    template <typename T>
    concept lockable = requires(T lock)
    {
        lock.lock();
        lock.release();
    };

    template <lockable T>
    class lock_guard
    {
        T& lock;

    public:
        inline lock_guard(T& s) : lock(s) { lock.lock(); }
        inline ~lock_guard() { lock.release(); }
    };

    using spinlock_guard = lock_guard<spinlock>;

}; // namespace lock

#define _CONCAT(prefix, suffix) prefix##suffix
#define CONCAT(prefix, suffix) _CONCAT(prefix, suffix)
#define _SPINLOCK_SYNC_BLOCK(n)                                                                                             \
    static ::lock::spinlock CONCAT(__lock, n);                                                                              \
    ::lock::spinlock_guard CONCAT(__guard, n) { CONCAT(__lock, n) }

#define SPINLOCK_SYNC_BLOCK _SPINLOCK_SYNC_BLOCK(__COUNTER__)

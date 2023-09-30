#pragma once

#include "kinit/boot_resource.h"
#include <asm/asm_cpp.h>
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

        inline void release() { l.store(0, std::memory_order_release); }

        inline auto owned_core() -> std::size_t { return l.load() - 1; }
    };

    class interrupt_lock_guard
    {
    public:
        interrupt_lock_guard() { disable_interrupt(); };
        ~interrupt_lock_guard() { enable_interrupt(); }
    };

    template <typename T>
    concept lockable = requires(T lock) {
        lock.lock();
        lock.release();
    };

    template <lockable T>
    class lock_guard
    {
        T& lock;

    public:
        lock_guard(T& lock) : lock(lock) { lock.lock(); }
        ~lock_guard() { lock.release(); }
    };

    using spinlock_guard = lock_guard<spinlock>;
}; // namespace lock

#define _CONCAT(prefix, suffix) prefix##suffix
#define CONCAT(prefix, suffix) _CONCAT(prefix, suffix)
#define _SPINLOCK_SYNC_BLOCK(n)                                                                                                                      \
    static ::lock::spinlock CONCAT(__lock, n);                                                                                                       \
    ::lock::spinlock_guard CONCAT(__guard, n) { CONCAT(__lock, n) }

#define SPINLOCK_SYNC_BLOCK _SPINLOCK_SYNC_BLOCK(__COUNTER__)

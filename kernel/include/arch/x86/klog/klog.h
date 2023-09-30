#pragma once

#include <config.h>
#include <cstddef>
#include <debug/debug.h>
#include <printf.h>
#include <smp/smp.h>
#include <sync/spinlock.h>

namespace klog
{
    namespace detail
    {
        auto get_klog_lock() -> lock::spinlock&;
    };

    template <typename... Args>
    auto print(const char* fmt, Args... args) -> std::size_t
    {
        return std::printf(fmt, args...);
    }

    template <typename... Args>
    void log(const char* fmt, Args... args)
    {
        lock::spinlock_guard guard(detail::get_klog_lock());

        print("[%lu] ", smp::core_local::get().core_id);
        print(fmt, args...);
        print("\n");
    }

    void log_many(auto callback)
    {
        lock::spinlock_guard guard(detail::get_klog_lock());
        callback();
    }

    //[[gnu::format(printf, 1, 2)]] std::size_t log(const char* fmt, ...);

    [[noreturn]] inline void panic(const char* msg)
    {
        lock::spinlock_guard guard(detail::get_klog_lock());
        print("[%lu] ", smp::core_local::get().core_id);
        debug::panic(msg);
        __builtin_unreachable();
    }

    inline void panic(const char* msg, bool crash)
    {
        lock::spinlock_guard guard(detail::get_klog_lock());
        print("[%lu] ", smp::core_local::get().core_id);
        debug::panic(msg, crash);
    }
} // namespace klog

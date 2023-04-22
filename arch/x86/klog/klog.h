#pragma once

#include "smp/smp.h"
#include <printf.h>
#include <cstddef>
#include <config.h>
#include <sync/spinlock.h>
#include <debug/debug.h>

namespace klog
{
    namespace 
    {
        extern std::size_t start, end;
        lock::spinlock lock;
    }

    template<typename... Args>
    auto log(const char* fmt, Args... args) -> std::size_t
    {
        lock::spinlock_guard guard(lock);
        std::size_t len1 = std::printf("[%lu] ", smp::core_local::get().core_id);
        std::size_t len2 = std::printf(fmt, args...);
        return len1 + len2;
    }

    //[[gnu::format(printf, 1, 2)]] std::size_t log(const char* fmt, ...);

    [[noreturn]] inline void panic(const char* msg)
    {
        lock::spinlock_guard g(lock);
        std::printf("[%lu] ", smp::core_local::get().core_id);
        debug::panic(msg);
        __builtin_unreachable();
    }

    inline void panic(const char* msg, bool crash)
    {
        lock::spinlock_guard guard(lock);
        std::printf("[%lu] ", smp::core_local::get().core_id);
        debug::panic(msg, crash);
    }
}

#ifndef __ARCH_X86_UTILS_LOGGING_H__
#define __ARCH_X86_UTILS_LOGGING_H__
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
    std::size_t log(const char* fmt, Args... args)
    {
        lock::spinlock_guard g(lock);
        std::size_t s1 = std::printf("[%lu] ", smp::core_local::get().coreid);
        std::size_t s2 = std::printf(fmt, args...);
        return s1 + s2;
    }

    [[noreturn]] inline void panic(const char* msg)
    {
        lock::spinlock_guard g(lock);
        std::printf("[%lu] ", smp::core_local::get().coreid);
        debug::panic(msg);
        __builtin_unreachable();
    }

    inline void panic(const char* msg, bool crash)
    {
        lock::spinlock_guard g(lock);
        std::printf("[%lu] ", smp::core_local::get().coreid);
        debug::panic(msg, crash);
    }
}
#endif

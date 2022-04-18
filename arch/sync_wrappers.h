#ifndef __ARCH_X86_IO_WRAPPERS_H__
#define __ARCH_X86_IO_WRAPPERS_H__
#include <sync/spinlock.h>
#include <printf.h>
#include <utility>

namespace sync
{
    template<typename... Args>
    auto printf(const char* fmt, Args... args)
    {
        static lock::spinlock l;

        lock::spinlock_guard g(l);
        return std::printf(fmt, args...);
    }
}

#endif

#pragma once

#include <printf.h>
#include <sync/spinlock.h>
#include <utility>
#include <config.h>
#include <mm/malloc.h>

namespace sync
{
    template <typename... Args>
    auto printf(const char* fmt, Args... args)
    {
        SPINLOCK_SYNC_BLOCK;
        return std::printf(fmt, args...);
    }
} // namespace sync

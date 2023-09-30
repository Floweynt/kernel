#pragma once

#include <config.h>
#include <klog/klog.h>

INLINE void expect(bool condition, const char* message)
{
    if (!condition)
    {
        if (message == nullptr)
        {
            message = "<no message>";
        }

        klog::panic(message);
    }
}

INLINE void expect_true(bool condition, const char* message) { expect(condition, message); }
INLINE void expect_false(bool condition, const char* message) { expect(!condition, message); }
INLINE void expect_null(auto* ptr, const char* message) { expect(ptr == nullptr, message); }

INLINE auto expect_nonnull(auto* ptr, const char* message = "expected non-null pointer") -> auto*
{
    expect(ptr != nullptr, message);
    return ptr;
}

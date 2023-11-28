#include <bits/user_implement.h>
#include <config.h>
#include <debug/debug.h>
#include <mm/malloc.h>
#include <sync/spinlock.h>

namespace std::detail
{
    static lock::spinlock malloc_lock;

    auto malloc(size_t size) -> void*
    {
        lock::spinlock_guard guard(malloc_lock);
        return ::alloc::malloc(size);
    }

    auto aligned_malloc(size_t size, size_t align) -> void*
    {
        lock::spinlock_guard guard(malloc_lock);
        return ::alloc::aligned_malloc(size, align);
    }

    auto realloc(void* pointer, size_t size) -> void*
    {
        lock::spinlock_guard guard(malloc_lock);
        return ::alloc::realloc(pointer, size);
    }

    void free(void* pointer)
    {
        lock::spinlock_guard guard(malloc_lock);
        return ::alloc::free(pointer);
    }

    namespace errors
    {
        [[noreturn]] void __stdexcept_out_of_range()
        {
            debug::panic("stdexcept out of range");
            __builtin_unreachable();
        }

        [[noreturn]] void __stdexcept_bad_alloc()
        {
            debug::panic("stdexcept bad alloc");
            __builtin_unreachable();
        }

        [[noreturn]] void __stdexcept_bad_variant_access()
        {
            debug::panic("stdexcept bad variant access");
            __builtin_unreachable();
        }

        [[noreturn]] void __printf_argument_notfound()
        {
            debug::panic("__printf_argument_notfound");
            __builtin_unreachable();
        }

        [[noreturn]] void __printf_undefined_specifier_for_length()
        {
            debug::panic("printf undefined specifier for length");
            __builtin_unreachable();
        }

        [[noreturn]] void __assert_fail(const char* msg)
        {
            debug::panic(msg);
            __builtin_unreachable();
        }

        [[noreturn]] void __halt()
        {
            while (1)
                asm volatile("hlt");

            __builtin_unreachable();
        }
    } // namespace errors
} // namespace std::detail

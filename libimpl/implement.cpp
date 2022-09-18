#include <bits/user_implement.h>
#include <config.h>
#include <debug/debug.h>
#include <mm/malloc.h>
#include <sync/spinlock.h>

namespace std::detail
{
    static lock::spinlock l;
    void* malloc(size_t size)
    {
        lock::spinlock_guard g(l);
        return ::alloc::malloc(size);
    }

    void* aligned_malloc(size_t size, size_t align)
    {
        lock::spinlock_guard g(l);
        return ::alloc::aligned_malloc(size, align);
    }

    void free(void* ptr)
    {
        lock::spinlock_guard g(l);
        return ::alloc::free(ptr);
    }

    namespace errors
    {
        [[noreturn]] void __stdexcept_out_of_range() { debug::panic("stdexcept out of range"); }
        [[noreturn]] void __stdexcept_bad_alloc() { debug::panic("stdexcept bad alloc"); }
        [[noreturn]] void __stdexcept_bad_variant_access() { debug::panic("stdexcept bad variant access"); }
        [[noreturn]] void __printf_argument_notfound() { debug::panic("what"); }
        [[noreturn]] void __printf_undefined_specifier_for_length()
        {
            debug::panic("printf undefined specifier for length");
        }
        [[noreturn]] void __halt()
        {
            while (1)
                asm volatile("hlt");
        }
    } // namespace errors
} // namespace std::detail

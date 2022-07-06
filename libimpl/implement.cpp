#include <bits/user_implement.h>
#include <config.h>
#include <panic.h>
#include <sync/spinlock.h>
#include <mm/malloc.h>
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
        return nullptr; 
    }

    void free(void* ptr) 
    { 
        lock::spinlock_guard g(l);
        return ::alloc::free(ptr); 
    }

    namespace errors
    {
        [[noreturn]] void __stdexcept_out_of_range() { std::panic("stdexcept out of range"); }
        [[noreturn]] void __stdexcept_bad_alloc() { std::panic("stdexcept bad alloc"); }
        [[noreturn]] void __stdexcept_bad_variant_access() { std::panic("stdexcept bad variant access"); }
        [[noreturn]] void __printf_argument_notfound() { std::panic("what"); }
        [[noreturn]] void __printf_undefined_specifier_for_length() { std::panic("printf undefined specifier for length"); }
        [[noreturn]] void __halt()
        {
            while (1)
                __asm__ __volatile__("hlt");
        }
    } // namespace errors
} // namespace std::detail

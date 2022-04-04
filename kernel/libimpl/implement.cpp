#include <config.h>
#include <bits/user_implement.h>
#include MALLOC_IMPL_PATH

namespace std::detail
{
    void *malloc(size_t size)
    {
        return ::alloc::malloc(size);
    }
    void *aligned_malloc(size_t size, max_align_t align);
    void free();
    void putc(char ch);

    namespace errors 
    {
    [[noreturn]] void __stdexcept_out_of_range();
    [[noreturn]] void __stdexcept_bad_alloc();
    [[noreturn]] void __stdexcept_bad_variant_access();
    [[noreturn]] void __printf_argument_notfound();
    [[noreturn]] void __printf_undefined_specifier_for_length();
    }
}

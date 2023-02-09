#ifndef __NOSTDLIB_USER_IMPLEMENT_H__
#define __NOSTDLIB_USER_IMPLEMENT_H__
#include "../include/cstddef"

namespace std::detail
{
    void* malloc(size_t size);
    void* aligned_malloc(size_t size, size_t align);
    void free(void*);
    void putc(char ch);

    namespace errors
    {
        [[noreturn]] void __stdexcept_out_of_range();
        [[noreturn]] void __stdexcept_bad_alloc();
        [[noreturn]] void __stdexcept_bad_variant_access();
        [[noreturn]] void __printf_argument_notfound();
        [[noreturn]] void __printf_undefined_specifier_for_length();
        [[noreturn]] void __assert_fail(const char*);
        [[noreturn]] void __halt();
    } // namespace errors
} // namespace std::detail
#endif

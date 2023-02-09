#include "../bits/user_implement.h"
#include <cstddef>
#include <new>

using namespace std;

[[nodiscard]] void* operator new(size_t size)
{
    void* ptr = detail::malloc(size);
    if (ptr == nullptr)
        detail::errors::__stdexcept_bad_alloc();
    return ptr;
}

[[nodiscard, gnu::malloc, gnu::alloc_size(1)]] void* operator new(size_t size, align_val_t align)
{
    void* ptr = detail::aligned_malloc(size, (size_t)align);
    if (ptr == nullptr)
        detail::errors::__stdexcept_bad_alloc();
    return ptr;
}

[[nodiscard, gnu::malloc, gnu::alloc_size(1)]] void* operator new(size_t size, const nothrow_t&) noexcept
{
    return detail::malloc(size);
}

[[nodiscard, gnu::malloc, gnu::alloc_size(1)]] void* operator new(size_t size, align_val_t align, const nothrow_t&)
{
    return detail::aligned_malloc(size, (size_t)align);
}

[[nodiscard]] void* operator new[](size_t size)
{
    void* ptr = detail::malloc(size);
    if (ptr == nullptr)
        detail::errors::__stdexcept_bad_alloc();
    return ptr;
}

[[nodiscard, gnu::malloc, gnu::alloc_size(1)]] void* operator new[](size_t size, align_val_t align)
{
    void* ptr = detail::aligned_malloc(size, (size_t)align);
    if (ptr == nullptr)
        detail::errors::__stdexcept_bad_alloc();
    return ptr;
}

[[nodiscard, gnu::malloc, gnu::alloc_size(1)]] void* operator new[](size_t size, const nothrow_t&) noexcept
{
    return detail::malloc(size);
}

[[nodiscard, gnu::malloc, gnu::alloc_size(1)]] void* operator new[](size_t size, align_val_t align, const nothrow_t&)
{
    return detail::aligned_malloc(size, (size_t)align);
}

void operator delete(void* p) noexcept { detail::free(p); }

void operator delete(void* p, align_val_t) noexcept { detail::free(p); }

void operator delete(void* p, const nothrow_t&) noexcept { detail::free(p); }

void operator delete(void* p, align_val_t, const nothrow_t&) noexcept { detail::free(p); }

void operator delete(void* p, size_t) noexcept { detail::free(p); }

void operator delete(void* p, size_t, align_val_t) noexcept { detail::free(p); }

void operator delete[](void* p) noexcept { detail::free(p); }

void operator delete[](void* p, align_val_t) noexcept { detail::free(p); }

void operator delete[](void* p, const nothrow_t&) noexcept { detail::free(p); }

void operator delete[](void* p, align_val_t, const nothrow_t&) noexcept { detail::free(p); }

void operator delete[](void* p, size_t) noexcept { detail::free(p); }

void operator delete[](void* p, size_t, align_val_t) noexcept { detail::free(p); }

#ifndef __ARCH_X86_IO_WRAPPERS_H__
#define __ARCH_X86_IO_WRAPPERS_H__
#include <printf.h>
#include <sync/spinlock.h>
#include <utility>
#include <config.h>
#include MALLOC_IMPL_PATH

namespace sync
{
    template <typename... Args>
    auto printf(const char* fmt, Args... args)
    {
        static lock::spinlock l;

        lock::spinlock_guard g(l);
        return std::printf(fmt, args...);
    }

    struct pre_smp_t {};
    inline constexpr pre_smp_t pre_smp{};

} // namespace sync

void* operator new (std::size_t size, const sync::pre_smp_t&)
{
    static lock::spinlock l;
    lock::spinlock_guard(l, 0);
    return alloc::malloc(size); 
}

#endif

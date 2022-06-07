#ifndef __ARCH_X86_UTILS_LOGGING_H__
#define __ARCH_X86_UTILS_LOGGING_H__
#include <printf.h>
#include <cstddef>
#include <config.h>

namespace klog
{
    namespace 
    {
        extern std::size_t start, end;
    }
    template<typename... Args>
    std::size_t log(const char* fmt, Args... args)
    {
        std::size_t s1 = std::printf("[%ul] ");
        std::size_t s2 = std::printf(fmt, args...);

        return s1;
    }
}
#endif

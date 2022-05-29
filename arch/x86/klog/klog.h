#ifndef __ARCH_X86_UTILS_LOGGING_H__
#define __ARCH_X86_UTILS_LOGGING_H__
#include <printf.h>

namespace klog
{
    template<typename... Args>
    std::size_t log(const char* fmt, Args... args)
    {
        std::printf("[%ul] ");
        return std::printf(fmt, args...);
    }
}
#endif

#ifndef __NOSTDLIB_PRINTF_H__
#define __NOSTDLIB_PRINTF_H__

#include "../bits/user_implement.h"
#include "algorithm"
#include "cctype"
#include "cstdarg"
#include "cstddef"
#include "cstdint"
#include "cstring"
#include "initializer_list"
#include "type_traits"

namespace std
{
    namespace detail::printf
    {
        int do_printf(void (*printer)(char, void*), void* data, const char* format, va_list fmtargs);
    }

    template <typename C, typename... Args>
    int printf_callback(C printer, const char* format, va_list fmtargs)
    {
        return detail::printf::do_printf(
            +[](char ch, void* p) { (*((C*)p))(ch); }, &printer, format, fmtargs);
    }

    [[gnu::format(printf, 1, 2)]] inline int printf(const char* format, ...)
    {
        va_list va;
        va_start(va, format);
        auto ret = detail::printf::do_printf(
            +[](char ch, void*) { detail::putc(ch); }, nullptr, format, va);
        va_end(va);
        return ret;
    }

    [[gnu::format(printf, 2, 3)]] inline int printf_callback(auto callback, const char* format, ...)
    {
        va_list va;
        va_start(va, format);
        auto ret = printf_callback(callback, format, va);
        va_end(va);
        return ret;
    }

    inline int sprintf(char* buffer, const char* format, ...)
    {
        va_list va;
        va_start(va, format);
        auto ret = printf_callback([&, buffer](char ch) mutable { *buffer++ = ch; }, format, va);
        va_end(va);
        return ret;
    }

    inline int snprintf(char* buffer, size_t count, const char* format, ...)
    {
        va_list va;
        va_start(va, format);
        auto ret = printf_callback(
            [&, buffer](char ch) mutable {
                if (count)
                {
                    *buffer++ = ch;
                    count--;
                }
            },
            format, va);
        va_end(va);
        return ret;
    }
} // namespace std

#endif

#ifndef __ARCH_X86_DRIVER_TERMINAL_H__
#define __ARCH_X86_DRIVER_TERMINAL_H__
#include <include/cstddef.h>
#include <interface/driver/tty.h>
#include <kinit/stivale2.h>

namespace driver
{
    class simple_tty : public tty_startup_driver
    {
        void (*putc_impl)(const char*, std::size_t);

    public:
        inline simple_tty(uint64_t address) : putc_impl((decltype(putc_impl))address){};
        inline void putc(char c) { putc_impl(&c, 1); }
    };
} // namespace driver

#endif

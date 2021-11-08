#ifndef __TTY_STARTER_DRIVE__
#define __TTY_STARTER_DRIVE__
#include "utils.h"
#define KDEBUG

namespace driver
{
    class tty_startup_driver
    {
    public:
        virtual void puts(const char* c) = 0;
    };

    extern tty_startup_driver* tty_dvr_startup;

}

inline void puts(const char* ch)
{
    MARKER_BREAK("0x10");
    driver::tty_dvr_startup->puts(ch);
}

inline void print_dbg(const char* ch)
{
//#ifdef KDEBUG
    MARKER_BREAK("0x11");
    puts("DEBUG: ");
    puts(ch);
//#endif
}


#endif

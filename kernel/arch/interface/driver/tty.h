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

#endif

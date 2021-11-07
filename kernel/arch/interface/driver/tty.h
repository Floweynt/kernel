#ifndef __TTY_STARTER_DRIVE__
#define __TTY_STARTER_DRIVE__
#include "utils.h"
#define KDEBUG

class tty_startup_driver
{
public:
    virtual void puts(const char* c) = 0;
};

extern tty_startup_driver* driver;
extern tty_startup_driver* early_tty_driver;

inline void puts(const char* ch)
{
    driver->puts(ch);
}

inline void early_puts(const char* ch)
{
    early_tty_driver->puts(ch);
}

inline void early_dbg(const char* ch)
{
//#ifdef KDEBUG
    MAGIC_BREAK;
    early_puts("DEBUG: ");
    early_puts(ch);
//#endif
}

#endif

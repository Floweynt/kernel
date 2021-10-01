#ifndef __TTY_STARTER_DRIVE__
#define __TTY_STARTER_DRIVE__

class tty_startup_driver
{
public:
    virtual void putc(char c) = 0;
    virtual ~tty_startup_driver() = default;
};
#endif

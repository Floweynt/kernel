#include "tty.h"
#include <bits/user_implement.h>
static driver::tty_startup_driver* drv;

void driver::set_tty_startup_driver(driver::tty_startup_driver* d) { drv = d; }

void std::detail::putc(char ch) { drv->putc(ch); }

#pragma once

#include <kinit/limine.h>

namespace tty
{
    void init();    
}

// useful formatting macros
#define RED(c) "\x1b[31m" c "\x1b[0m"
#define CYAN(c) "\x1b[36m" c "\x1b[0m"
#define GREEN(c) "\x1b[32m" c "\x1b[0m" 
#define YELLOW(c) "\x1b[93m" c "\x1b[0m"
#define GRAY(c) "\x1b[97m" c "\x1b[0m"

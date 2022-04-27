#include "handlers.h"
#include <printf.h>

namespace handlers
{
    void handle_timer(uint64_t, uint64_t)
    {
        std::printf("timer!\n");
    }
}

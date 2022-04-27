#include "handlers.h"

namespace handlers
{
    void handle_div_by_zero(uint64_t i1, uint64_t i2)
    {
        if(smp::core_local::get().ctxbuffer->ss == 0)
        {
            std::panic("division by zero");
        }

        // else kill process
    }
}

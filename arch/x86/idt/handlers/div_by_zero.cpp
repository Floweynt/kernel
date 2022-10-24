#include "handlers.h"
#include <klog/klog.h>

namespace handlers
{
    void handle_div_by_zero(std::uint64_t i1, std::uint64_t i2)
    {
        if (smp::core_local::get().ctxbuffer->ss == 0)
        {
            klog::panic("division by zero");
        }

        // else kill process
    }
} // namespace handlers

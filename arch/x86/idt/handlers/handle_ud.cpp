#include "handlers.h"
#include <klog/klog.h>
#include <smp/smp.h>
#include <tty/tty.h>

namespace handlers
{
    void handle_nm(std::uint64_t i1, std::uint64_t i2)
    {
        klog::log("====================== " RED("#NM") " ======================\n");
        debug::log_register(smp::core_local::get().ctxbuffer);
        klog::panic("#NM");
    }
} // namespace handlers

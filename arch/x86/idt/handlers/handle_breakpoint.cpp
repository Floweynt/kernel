#include "handlers.h"
#include <klog/klog.h>
#include <smp/smp.h>
#include <tty/tty.h>

namespace handlers
{
    void handle_breakpoints(std::uint64_t /*unused*/, std::uint64_t /*unused*/)
    {
        klog::log("====================== " RED("#BP") " ======================\n");
        klog::log("a kernel breakpoint has been hit\n");
        debug::log_register(smp::core_local::get().ctxbuffer);
        klog::panic("#BP");
    }
} // namespace handlers

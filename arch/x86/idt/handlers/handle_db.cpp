#include "handlers.h"
#include <klog/klog.h>
#include <smp/smp.h>
#include <tty/tty.h>

namespace handlers
{
    void handle_debug(std::uint64_t /*unused*/, std::uint64_t /*unused*/)
    {
        klog::log("====================== " RED("#DB") " ======================\n");
        klog::log("a runtime kernel debug exception has occurred\n");
        debug::log_register(smp::core_local::get().ctxbuffer);
        klog::panic("#DB");
    }
} // namespace handlers

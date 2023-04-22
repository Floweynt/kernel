#include "handlers.h"
#include <klog/klog.h>
#include <smp/smp.h>
#include <tty/tty.h>

namespace handlers
{
    void handle_page_fault(std::uint64_t /*unused*/, std::uint64_t /*unused*/)
    {
        klog::log("====================== " RED("#PF") " ======================\n");
        klog::log("on-demand paging and swapping should really be something I implement...\n");
        debug::log_register(smp::core_local::get().ctxbuffer);
        klog::panic("#PF");
    }
} // namespace handlers



#include "handlers.h"
#include <smp/smp.h>
#include <klog/klog.h>
#include <tty/tty.h>
namespace handlers
{
    void handle_bad_tss(std::uint64_t i1, std::uint64_t i2)
    {
        klog::log("====================== " RED("#TS") " ======================\n");
        klog::log("a runtime kernel debug exception has occurred\n");
        debug::log_register(smp::core_local::get().ctxbuffer);
        klog::panic("#TS");
    }
} // namespace handlers
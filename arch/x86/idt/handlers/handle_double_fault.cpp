
#include "handlers.h"
#include <smp/smp.h>
#include <klog/klog.h>
#include <tty/tty.h>
namespace handlers
{
    void handle_double_fault(std::uint64_t i1, std::uint64_t i2)
    {
        klog::log("====================== " RED("#DF") " ======================\n");
        debug::log_register(smp::core_local::get().ctxbuffer);
        klog::panic("#DF");
    }
} // namespace handlers

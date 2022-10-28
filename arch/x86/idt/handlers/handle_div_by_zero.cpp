#include "handlers.h"
#include <smp/smp.h>
#include <klog/klog.h>
#include <tty/tty.h>
namespace handlers
{
    void handle_div_by_zero(std::uint64_t i1, std::uint64_t i2)
    {
        klog::log("====================== " RED("#DE") " ======================\n");
        klog::log("a runtime kernel error has occurred that cannot be recovered from due to division by zero\n");
        debug::log_register(smp::core_local::get().ctxbuffer);
        
        klog::panic("#DE");
    }
} // namespace handlers

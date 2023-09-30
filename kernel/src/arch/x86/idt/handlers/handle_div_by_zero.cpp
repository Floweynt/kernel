#include <klog/klog.h>
#include <idt/handlers/handlers.h>
#include <smp/smp.h>
#include <tty/tty.h>

namespace handlers
{
    void handle_div_by_zero(std::uint64_t /*unused*/, std::uint64_t /*unused*/)
    {
        klog::log("====================== " RED("#DE") " ======================");
        klog::log("a runtime kernel error has occurred that cannot be recovered from due to division by zero");
        debug::log_register(smp::core_local::get().ctxbuffer);
        klog::panic("#DE");
    }
} // namespace handlers

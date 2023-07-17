#include <idt/handlers/handlers.h>
#include <klog/klog.h>
#include <smp/smp.h>
#include <tty/tty.h>

namespace handlers
{
    void handle_bounds(std::uint64_t /*unused*/, std::uint64_t /*unused*/)
    {
        klog::log("====================== " RED("#BR") " ======================\n");
        debug::log_register(smp::core_local::get().ctxbuffer);
        klog::panic("#BR");
    }
} // namespace handlers

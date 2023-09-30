#include <klog/klog.h>
#include <idt/handlers/handlers.h>
#include <smp/smp.h>
#include <tty/tty.h>

namespace handlers
{
    void handle_np(std::uint64_t /*unused*/, std::uint64_t /*unused*/)
    {
        klog::log("====================== " RED("#NP") " ======================");
        debug::log_register(smp::core_local::get().ctxbuffer);
        klog::panic("#NP");
    }
} // namespace handlers

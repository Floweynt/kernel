#include <idt/handlers/handlers.h>
#include <klog/klog.h>
#include <smp/smp.h>
#include <tty/tty.h>

namespace handlers
{
    void handle_bad_tss(std::uint64_t /*unused*/, std::uint64_t /*unused*/)
    {
        klog::log("====================== " RED("#TS") " ======================");
        klog::log("kernel task state segment corrupted");
        debug::log_register(smp::core_local::get().ctxbuffer);
        klog::panic("#TS");
    }
} // namespace handlers

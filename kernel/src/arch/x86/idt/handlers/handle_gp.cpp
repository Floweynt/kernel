#include <klog/klog.h>
#include <idt/handlers/handlers.h>
#include <smp/smp.h>
#include <tty/tty.h>

namespace handlers
{
    void handle_gp(std::uint64_t /*unused*/, std::uint64_t error_code)
    {
        klog::log("====================== " RED("#GP") " ======================\n");
        klog::log("error_code=0x%llx\n", error_code);
        debug::log_register(smp::core_local::get().ctxbuffer);
        klog::panic("#GP");
    }
} // namespace handlers

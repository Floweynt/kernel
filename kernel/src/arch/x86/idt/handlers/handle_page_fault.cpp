#include <idt/handlers/handlers.h>
#include <klog/klog.h>
#include <smp/smp.h>
#include <tty/tty.h>

namespace handlers
{
    void handle_page_fault(std::uint64_t /*unused*/, std::uint64_t error_code)
    {
        klog::log("====================== " RED("#PF") " ======================");
        klog::log("on-demand paging and swapping should really be something I implement...");
        klog::log("error_code=0x%llx", error_code);
        klog::log("page-fault address (cr2) = 0x%016llx", read_cr2());
        debug::log_register(smp::core_local::get().ctxbuffer);
        klog::panic("#PF");
    }
} // namespace handlers


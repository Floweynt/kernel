#include <config.h>
#include <idt/handlers/handlers.h>
#include <klog/klog.h>
#include <misc/kassert.h>
#include <mm/mm.h>
#include <mm/paging/paging.h>
#include <smp/smp.h>
#include <tty/tty.h>

namespace handlers
{
    void handle_page_fault(std::uint64_t /*unused*/, std::uint64_t error_code)
    {
        auto fault_address = read_cr2();

        if constexpr (config::get_val<"sanitize.address">)
        {
            // this is within asan...
            if (fault_address >= 0xffffd00000000000 && fault_address < 0xffffffff80000000)
            {
                klog::log("asan tried to request page @ %016llx", fault_address);
                paging::request_page(paging::SMALL, fault_address, mm::make_physical(expect_nonnull(mm::pmm_allocate_clean())), {.rw = true});
                return;
            }
        }

        klog::log("====================== " RED("#PF") " ======================");
        klog::log("on-demand paging and swapping should really be something I implement...");
        klog::log("error_code=0x%llx", error_code);
        klog::log("page-fault address (cr2) = 0x%016llx", fault_address);
        debug::log_register(smp::core_local::get().ctxbuffer);
        klog::panic("#PF");
    }
} // namespace handlers

#include "smp.h"
#include <asm/asm_cpp.h>
#include <gdt/gdt.h>
#include <paging/paging.h>
#include <pmm/pmm.h>
#include <sync_wrappers.h>

namespace smp
{ 
    void core_local::create()
    {
        entries = (core_local**) pmm::pmm_allocate();
        for(std::size_t i = 0; i < boot_resource::instance().core_count(); i++)
            entries[i] = (core_local*)pmm::pmm_allocate();
        // point entries 
    }

    [[noreturn]] void initalize_smp(stivale2_smp_info* info)
    {
        info = (stivale2_smp_info*)((uint64_t) info | 0xffff800000000000);
        disable_interrupt();
        wrmsr(msr::IA32_GS_BASE, smp::core_local::gs_of(info->extra_argument));

        paging::install();
        gdt::install_gdt();
        idt::install_idt();

        sync::printf("SMP started from core %lu\n", info->lapic_id);

        while(1) 
            __asm__ __volatile__("hlt");
    }
}

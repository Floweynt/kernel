#include "smp.h"
#include <asm/asm_cpp.h>
#include <gdt/gdt.h>
#include <mm/pmm.h>
#include <paging/paging.h>
#include <sync_wrappers.h>

namespace smp
{
    void core_local::create()
    {
        entries = (core_local**) mm::pmm_allocate_pre_smp();
        for (std::size_t i = 0; i < boot_resource::instance().core_count(); i++)
            entries[i] = (core_local*) mm::pmm_allocate_pre_smp();
        // point entries
    }

    [[noreturn]] void initalize_smp(stivale2_smp_info* info)
    {
        disable_interrupt();
        info = (stivale2_smp_info*)((uint64_t)info | 0xffff800000000000);
        wrmsr(msr::IA32_GS_BASE, smp::core_local::gs_of(info->extra_argument));
        wrmsr(msr::IA32_GS_BASE, smp::core_local::gs_of(info->extra_argument));

        paging::install();
        smp::core_local::get().ctxbuffer = new (sync::pre_smp) context;
        smp::core_local::get().coreid = info->extra_argument;

        gdt::install_gdt();
        idt::install_idt();

        sync::printf("SMP started from core %lu\n", info->lapic_id);

        if(info->lapic_id == 0)
        {
            idt::register_idt([](uint64_t i1, uint64_t i2) {
                std::printf("function parameter i1=%lx, i2=%lx\n", i1, i2);
                std::printf("rip=%lu\n", smp::core_local::get().ctxbuffer->rip);
                std::printf("cs=%lu\n", smp::core_local::get().ctxbuffer->cs);
                std::printf("rflags=%lu\n", smp::core_local::get().ctxbuffer->rflags);
                std::printf("rsp=%lu\n", smp::core_local::get().ctxbuffer->rsp);
                std::printf("ss=%lu\n", smp::core_local::get().ctxbuffer->ss);

                for(int i = 0; i < 15; i++)
                    std::printf("reg=%lu\n", smp::core_local::get().ctxbuffer->rgp[i]);
            }, 69);
            __asm__ __volatile__("int $69");
        }

        while (1)
            __asm__ __volatile__("hlt");
    }
} // namespace smp

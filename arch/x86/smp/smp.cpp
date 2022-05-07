#include "smp.h"
#include <asm/asm_cpp.h>
#include <gdt/gdt.h>
#include <idt/handlers/handlers.h>
#include <mm/pmm.h>
#include <paging/paging.h>
#include <sync_wrappers.h>

namespace smp
{
    void core_local::create()
    {
        entries = (core_local**)mm::pmm_allocate_pre_smp();
        for (std::size_t i = 0; i < boot_resource::instance().core_count(); i++)
            entries[i] = new (mm::pmm_allocate_pre_smp()) core_local;
        // point entries
    }

    static void initalize_apic(smp::core_local& local)
    {
        uint64_t l = local.apic.get_apic_base();
        paging::map_hhdm_phys(paging::page_type::SMALL, l);
        local.apic.enable();
        sync::printf("APIC: ticks per ms: %lu\n", local.apic.calibrate());
        local.apic.set_tick(idt::register_idt(handlers::handle_timer), 10);
    }

    [[noreturn]] static void idle(uint64_t)
    {
        enable_interrupt();
        std::detail::errors::__halt();
    }

    [[noreturn]] void initalize_smp(stivale2_smp_info* info)
    {
        disable_interrupt();
        info = mm::make_virtual<stivale2_smp_info>((uint64_t)info);
        wrmsr(msr::IA32_GS_BASE, smp::core_local::gs_of(info->extra_argument));
        wrmsr(msr::IA32_KERNEL_GS_BASE, smp::core_local::gs_of(info->extra_argument));
        wrmsr(msr::IA32_PAT, 0x706050403020100);
        wrmsr(msr::IA32_EFER, rdmsr(msr::IA32_EFER) | (1 << 11));

        paging::install();
        smp::core_local& local = smp::core_local::get();
        local.ctxbuffer = &proc::get_process(0).threads[0].ctx;
        local.idt_handler_entries = new (sync::pre_smp) uintptr_t[256];
        local.idt_entries = new (sync::pre_smp) idt::idt_entry[256];
        local.coreid = info->extra_argument;

        gdt::install_gdt();
        idt::init_idt();
        idt::install_idt();

        sync::printf("SMP started from core %lu\n", info->lapic_id);

        for (std::size_t i = 0; i < 32; i++)
            if (!idt::register_idt(handlers::INTERRUPT_HANDLERS[i], i))
                std::panic("failed to allocate irq");

        proc::make_kthread(idle, 0);

        if (local.coreid == 0)
            proc::make_kthread(
                +[](uint64_t a) {
                    sync::printf("value: %lu\n", a);
                    idle(0);
                },
                12321);

        initalize_apic(smp::core_local::get());
        idle(0);
    }
} // namespace smp

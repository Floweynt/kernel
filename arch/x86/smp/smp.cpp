#include "smp.h"
#include <asm/asm_cpp.h>
#include <gdt/gdt.h>
#include <mm/pmm.h>
#include <paging/paging.h>
#include <idt/handlers/handlers.h>
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

    static void initalize_apic(smp::core_local& local)
    {
        uint64_t l = local.apic.get_apic_base();
        paging::map_hhdm_phys(paging::page_type::SMALL, l);
        local.apic.enable();
        sync::printf("APIC: ticks per ms: %lu\n", local.apic.calibrate());
        local.apic.set_tick(idt::register_idt(handlers::handle_timer), 1000);
    }

    [[noreturn]] void initalize_smp(stivale2_smp_info* info)
    {
        disable_interrupt();
        info = mm::make_virtual<stivale2_smp_info>((uint64_t)info);
        wrmsr(msr::IA32_GS_BASE, smp::core_local::gs_of(info->extra_argument));
        wrmsr(msr::IA32_KERNEL_GS_BASE, smp::core_local::gs_of(info->extra_argument));
        wrmsr(msr::IA32_PAT, 0x706050403020100);

        paging::install();
        smp::core_local& local = smp::core_local::get();
        local.ctxbuffer = new (sync::pre_smp) context;
        local.idt_handler_entries = new (sync::pre_smp) uintptr_t[256];
        local.idt_entries = new (sync::pre_smp) idt::idt_entry[256];
        local.coreid = info->extra_argument;

        gdt::install_gdt();
        idt::init_idt();
        idt::install_idt();

        sync::printf("SMP started from core %lu\n", info->lapic_id);

        if(info->extra_argument != 0)
            while (1)
                __asm__ __volatile__("hlt");

        local.irq_allocator = mm::bitmask_allocator(local.irq_allocator_buffer, 256);

        for(std::size_t i = 0; i < 32; i++)
            if(!idt::register_idt(handlers::INTERRUPT_HANDLERS[i], i))
                std::panic("failed to allocate");

        // initalize apic
        initalize_apic(local);
        enable_interrupt();

        while (1)
            __asm__ __volatile__("hlt");
    }
} // namespace smp

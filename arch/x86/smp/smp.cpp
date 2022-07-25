// cSpell:ignore apic, hhdm, lapic, wrmsr, efer, stivale, rdmsr
#include "smp.h"
#include <asm/asm_cpp.h>
#include <gdt/gdt.h>
#include <idt/handlers/handlers.h>
#include <mm/pmm.h>
#include <paging/paging.h>
#include <sync_wrappers.h>
#include <klog/klog.h>

namespace smp
{
    void core_local::create()
    {
        entries = (core_local**)mm::pmm_allocate_pre_smp();
        for (std::size_t i = 0; i < boot_resource::instance().core_count(); i++)
            entries[i] = new (mm::pmm_allocate_pre_smp()) core_local;
        // point entries
    }

    static void initialize_apic(smp::core_local& local)
    {
        uint64_t l = local.apic.get_apic_base();
        paging::map_hhdm_phys(paging::page_type::SMALL, l);
        local.apic.enable();
        klog::log("APIC: ticks per ms: %lu\n", local.apic.calibrate());
        local.apic.set_tick(idt::register_idt(handlers::handle_timer), 20);
    }

    [[noreturn]] static void idle(uint64_t)
    {
        enable_interrupt();
        std::detail::errors::__halt();
    }

    [[noreturn]] static void smp_main(stivale2_smp_info* info)
    {
        mark_stack(debug::SMP);
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

        klog::log("SMP started\n");

        for (std::size_t i = 0; i < 32; i++)
            if (!idt::register_idt(handlers::INTERRUPT_HANDLERS[i], i))
                klog::panic("failed to allocate irq");

        local.tasks = new scheduler::task_queue;
        proc::make_kthread(idle, 0);

        proc::make_kthread(
            +[](uint64_t a) {
                sync::printf("value: %lu\n", a);
                idle(0);
            },
            local.coreid);
        initialize_apic(smp::core_local::get());
        idle(0);
    }

    // used for proper frame pointer generation
    static void main_wrapper(stivale2_smp_info* s) { smp_main(s); }

    [[noreturn]] void init(stivale2_struct_tag_smp* smp)
    {
        boot_resource::instance().mark_smp_start();
        std::printf("init_smp(): bootstrap_processor_id=%u\n", smp->bsp_lapic_id);
        std::printf("  cpus: %lu\n", smp->cpu_count);

        std::size_t index;

        for (std::size_t i = 0; i < smp->cpu_count; i++)
        {
            std::printf("initalizing core: %lu\n", i);
            smp->smp_info[i].extra_argument = i;

            if (smp->smp_info[i].lapic_id == smp->bsp_lapic_id)
            {
                index = i;
                continue;
            }

            // remember that stack grows down
            smp->smp_info[i].target_stack = (uint64_t)mm::pmm_allocate() + paging::PAGE_SMALL_SIZE;

            smp::core_local::get(i).pagemap = (paging::page_table_entry*)mm::pmm_allocate();

            std::memcpy(smp::core_local::get(i).pagemap + 256, smp::core_local::get(index).pagemap + 256,
                        256 * sizeof(paging::page_table_entry));

            smp->smp_info[i].goto_address = (uint64_t)smp::main_wrapper;
        }

        // initialize myself too!
        smp::smp_main(&smp->smp_info[index]);
    }
} // namespace smp

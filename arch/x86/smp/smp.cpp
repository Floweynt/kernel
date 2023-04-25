// cSpell:ignore apic, hhdm, lapic, wrmsr, efer, stivale, rdmsr
#include "smp.h"
#include "kinit/limine.h"
#include "paging/paging_entries.h"
#include "process/process.h"
#include <asm/asm_cpp.h>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <gdt/gdt.h>
#include <idt/handlers/handlers.h>
#include <idt/idt.h>
#include <klog/klog.h>
#include <mm/mm.h>
#include <mm/pmm.h>
#include <paging/paging.h>
#include <process/context.h>
#include <sync_wrappers.h>

namespace smp
{
    void core_local::create(core_local* cpu0)
    {
        entries = new core_local*[boot_resource::instance().core_count()];
        entries[0] = cpu0;
        for (std::size_t i = 1; i < boot_resource::instance().core_count(); i++)
        {
            entries[i] = new core_local;
        }
    }

    namespace
    {
        void initialize_apic(smp::core_local& local)
        {
            std::uint64_t base = local.apic.get_apic_base();
            paging::map_hhdm_phys(paging::page_type::SMALL, base);
            local.apic.enable();
            klog::log("APIC: ticks per ms: %lu\n", local.apic.calibrate());
            local.apic.set_tick(idt::register_idt(idt::idt_builder(handlers::handle_timer)), 20);
        }

        [[noreturn]] void idle(std::uint64_t /*unused*/ = 0)
        {
            enable_interrupt();
            std::halt();
        }

        void run_init()
        {
            if (smp::core_local::get().core_id != 0)
            {
                return;
            }

            auto init_pid = proc::make_process();
            klog::log("init process pid: %d", init_pid);
            //proc::get_process(init_pid).make_thread();
        }

        [[noreturn]] void smp_main(limine_smp_info* info)
        {
            mark_stack(debug::SMP);
            disable_interrupt();

            std::uintptr_t cr3 = info->extra_argument & ~0xfff;
            std::uintptr_t core_id = info->extra_argument & 0xfff;

            write_cr3(mm::make_physical(cr3));

            wrmsr(msr::IA32_GS_BASE, smp::core_local::gs_of(core_id));
            wrmsr(msr::IA32_KERNEL_GS_BASE, smp::core_local::gs_of(core_id));

            // TODO: figure out why this doesn't work in a KVM env
            // wrmsr(msr::IA32_PAT, 0x706050403020100);

            wrmsr(msr::IA32_EFER, rdmsr(msr::IA32_EFER) | (1 << 11));

            smp::core_local& local = smp::core_local::get();

            local.core_id = core_id;
            local.current_thread = nullptr;
            local.ctxbuffer = new proc::context;
            local.idt_handler_entries = new std::uintptr_t[256];
            local.idt_entries = new idt::idt_entry[256];

            local.gdt.set_ist(&local.ist);
            gdt::install_gdt();
            gdt::reload_gdt_smp();
            idt::init_idt();
            idt::install_idt();

            klog::log("SMP started\n");

            /*
            for (std::size_t i = 0; i < 32; i++)
            {
                if (!idt::register_idt(idt::idt_builder(handlers::INTERRUPT_HANDLERS[i]), i))
                {
                    klog::panic("failed to allocate irq");
                }
            }
            */

            auto idle_task = proc::make_kthread(idle);
            auto& idle_th = proc::get_thread(proc::task_id{idle_task, 0});
            idle_th.state = proc::thread_state::IDLE;

            proc::make_kthread_args(
                +[](std::uint64_t arg) {
                    static std::size_t last_interrupt_count = 0;
                    klog::log("debugging task value 1: %lu\n", arg);

                    while (true)
                    {
                        if (last_interrupt_count != smp::core_local::get().timer_tick_count && smp::core_local::get().timer_tick_count % 250 == 0)
                        {
                            last_interrupt_count = smp::core_local::get().timer_tick_count;
                            klog::log("ping\n");
                        }
                    }
                    idle();
                },
                local.core_id);

            initialize_apic(smp::core_local::get());
            local.scheduler.set_idle(&idle_th);
            idle();
        }

        // used for proper frame pointer generation
        void main_wrapper(limine_smp_info* smp) { smp_main(smp); }
    } // namespace

    [[noreturn]] void init(limine_smp_response* smp)
    {
        boot_resource::instance().mark_smp_start();
        std::printf("init_smp(): bootstrap_processor_id=%u\n", smp->bsp_lapic_id);
        std::printf("  cpus: %lu\n", smp->cpu_count);

        std::size_t core_id = 0;
        std::size_t bsp_index = 0;
        for (std::size_t i = 0; i < smp->cpu_count; i++)
        {
            if (smp->bsp_lapic_id == smp->cpus[i]->lapic_id)
            {
                bsp_index = i;
                smp->cpus[i]->extra_argument = (std::uintptr_t)smp::core_local::get(0).pagemap;
                continue;
            }

            core_id++;
            klog::log("initalizing extra smp core: %lu\n", i);

            // remember that stack grows down

            smp->cpus[i]->extra_argument =
                (std::uintptr_t)(smp::core_local::get(core_id).pagemap = (paging::page_table_entry*)mm::pmm_allocate()) | core_id;

            paging::sync_page_tables(core_id, 0);
            stdext::direct_atomic_store((std::uint64_t*)&smp->cpus[i]->goto_address, (std::uintptr_t)smp::main_wrapper, std::memory_order_seq_cst);
        }

        // initialize myself too!
        klog::log("bootstraped smp from bootstrap core!\n");
        smp::smp_main(smp->cpus[bsp_index]);
    }
} // namespace smp

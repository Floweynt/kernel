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
#include <utility>
#include <vfs/vfs.h>
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
            SPINLOCK_SYNC_BLOCK;
            std::uint64_t base = local.apic.get_apic_base();
            paging::map_hhdm_phys(paging::page_type::SMALL, base);
            local.apic.enable();
            klog::log("APIC: ticks per ms: %lu\n", local.apic.calibrate());
            local.apic.set_tick(idt::register_idt(idt::idt_builder(handlers::handle_timer).ist(1)), 20);
        }

        [[noreturn]] void idle(std::uint64_t /*unused*/ = 0)
        {
            enable_interrupt();
            std::halt();
        }

        inline constexpr std::uint8_t simple_init_instr[] = {0xcd, 0x80, 0xeb, 0xfe};

        void run_init()
        {
            if (smp::core_local::get().core_id != 0)
            {
                return;
            }

            auto* code_buf = mm::pmm_allocate();
            auto* stack_buf = mm::pmm_allocate();
            auto* page_buf = mm::pmm_allocate();

            if (code_buf == nullptr)
            {
                klog::panic("failed to allocate memory for init code");
            }

            if (stack_buf == nullptr)
            {
                klog::panic("failed to allocate memory for init stack");
            }

            if (page_buf == nullptr)
            {
                klog::panic("failed to allocate memory for init page tables");
            }

            auto init_pid = proc::make_process();
            klog::log("init process pid: %u\n", init_pid);

            paging::copy_kernel_page_tables(as_ptr(page_buf), smp::core_local::get().pagemap);
            auto init_tid = proc::get_process(init_pid).make_thread(proc::context_builder(proc::context_builder::USER, 0x2000000)
                                                                        .set_flag(cpuflags::IF)
                                                                        .set_stack(0x40000000 + paging::PAGE_SMALL_SIZE)
                                                                        .set_cr3(as_uptr(page_buf))
                                                                        .build(),
                                                                    0);

            klog::log("init process tid: %u\n", init_tid);

            paging::page_table_entry* ptr = as_ptr(proc::get_process(init_pid).get_thread(init_tid)->ctx.cr3);

            paging::map_page_for(ptr, paging::SMALL, 0x2000000, mm::make_physical(code_buf),
                                 paging::page_prop{
                                     .rw = false,
                                     .us = true,
                                     .x = true,
                                 },
                                 false);

            paging::map_page_for(ptr, paging::SMALL, 0x40000000, mm::make_physical(stack_buf),
                                 paging::page_prop{
                                     .rw = true,
                                     .us = true,
                                     .x = false,
                                 },
                                 false);

            std::memcpy(code_buf, simple_init_instr, sizeof(simple_init_instr));
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

            // TODO: check non-null IST
            local.ist.ist1 = as_uptr(mm::pmm_allocate()) + paging::PAGE_SMALL_SIZE;
            local.ist.ist2 = as_uptr(mm::pmm_allocate()) + paging::PAGE_SMALL_SIZE;
            local.ist.ist3 = as_uptr(mm::pmm_allocate()) + paging::PAGE_SMALL_SIZE;
            local.ist.ist4 = as_uptr(mm::pmm_allocate()) + paging::PAGE_SMALL_SIZE;
            local.ist.ist5 = as_uptr(mm::pmm_allocate()) + paging::PAGE_SMALL_SIZE;
            local.ist.ist6 = as_uptr(mm::pmm_allocate()) + paging::PAGE_SMALL_SIZE;
            local.ist.ist7 = as_uptr(mm::pmm_allocate()) + paging::PAGE_SMALL_SIZE;

            local.gdt.set_ist(&local.ist);
            gdt::install_gdt();
            gdt::reload_gdt_smp();
            idt::init_idt();
            idt::install_idt();

            klog::log("SMP started\n");

            for (std::size_t i = 0; i < 32; i++)
            {
                if (!idt::register_idt(idt::idt_builder(handlers::INTERRUPT_HANDLERS[i]).ist(1), i))
                {
                    klog::panic("failed to allocate irq for cpu exceptions");
                }
            }

            idt::register_idt(idt::idt_builder(+[](std::uint64_t  /*idt*/, std::uint64_t  /*err*/) {
                                  klog::log("Hello world from user space! (actually this is kernel space, this is a syscall)\n");
                              })
                                  .dpl(0x3)
                                  .ist(1),
                              0x80);

            auto idle_task = proc::make_kthread(idle);
            auto& idle_th = proc::get_thread(proc::task_id{idle_task, 0});
            idle_th.state = proc::thread_state::IDLE;

            proc::make_kthread_args(
                +[](std::uint64_t arg) {
                    static std::size_t last_interrupt_count = 0;
                    klog::log("debugging task value: %lu\n", arg);
                    while (true)
                    {
                        auto& local = smp::core_local::get();
                        // klog::log("alive\n");
                        if (last_interrupt_count + 50 < local.timer_tick_count)
                        {
                            last_interrupt_count = local.timer_tick_count;
                            klog::log("ping\n");
                        }
                    }
                },
                local.core_id);

            initialize_apic(smp::core_local::get());
            local.scheduler.set_idle(&idle_th);

            run_init();
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
                as_uptr(smp::core_local::get(core_id).pagemap = (paging::page_table_entry*)mm::pmm_allocate()) | core_id;

            paging::sync_page_tables(core_id, 0);
            stdext::direct_atomic_store(as_ptr<uint64_t>(&smp->cpus[i]->goto_address), as_uptr(smp::main_wrapper), std::memory_order_seq_cst);
        }

        // initialize myself too!
        klog::log("bootstraped smp from bootstrap core!\n");
        smp::smp_main(smp->cpus[bsp_index]);
    }
} // namespace smp

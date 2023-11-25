// cSpell:ignore apic, hhdm, lapic, wrmsr, efer, stivale, rdmsr
#include "kinit/limine.h"
#include "process/process.h"
#include <asm/asm_cpp.h>
#include <atomic>
#include <bitbuilder.h>
#include <cstddef>
#include <cstdint>
#include <fs/impl/fpk.h>
#include <fs/vfs.h>
#include <gdt/gdt.h>
#include <idt/handlers/handlers.h>
#include <idt/idt.h>
#include <init_test.h>
#include <klog/klog.h>
#include <misc/cast.h>
#include <misc/kassert.h>
#include <misc/pointer.h>
#include <mm/mm.h>
#include <mm/paging/paging.h>
#include <mm/paging/paging_entries.h>
#include <process/context.h>
#include <smp/smp.h>
#include <sync/spinlock.h>
#include <sync_wrappers.h>
#include <user/elf_load.h>
#include <user/syscall/sys_io.h>
#include <utility>

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
        void wait_sync_action(auto callback)
        {
            static lock::spinlock lock;
            static bool flag = true;
            lock.lock();
            if (flag)
            {
                callback();
                flag = false;
            }
            lock.release();
        }

        void initialize_apic(smp::core_local& local)
        {
            SPINLOCK_SYNC_BLOCK;
            std::uint64_t base = local.apic.get_apic_base();
            paging::map_hhdm_page(paging::page_type::SMALL, base);
            local.apic.enable();
            klog::log("APIC: ticks per ms: %lu", local.apic.calibrate());
            local.apic.set_tick(idt::register_idt(idt::idt_builder(handlers::handle_timer).ist(1)), 20);
        }

        [[noreturn]] void idle(std::uint64_t /*unused*/ = 0)
        {
            enable_interrupt();
            std::halt();
        }

        void test_vfs()
        {
            // testing!
            auto initramfs = load(initramfs_fpk, initramfs_fpk_len);
            vfs::mount_on(vfs::get_root(), initramfs->get_root());
            auto vnode = vfs::lookup(vfs::get_root(), "bin/init");
        }

        void run_init()
        {
            if (smp::core_local::get().core_id != 0)
            {
                return;
            }

            test_vfs();

            auto init_pid = proc::make_process();
            klog::log("init process pid: %u", init_pid);
            auto init_tid = proc::get_process(init_pid).make_thread({}, 0);
            klog::log("init process tid: %u", init_pid);
            user::load_elf(a_out, *proc::get_process(init_pid).get_thread(init_tid));
        }

        void make_idle()
        {
            auto idle_task = proc::make_kthread(idle);
            auto& idle_th = proc::get_thread(proc::task_id{idle_task, 0});
            idle_th.state = proc::thread_state::IDLE;
            smp::core_local::get().scheduler.set_idle(&idle_th);
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

            // enable no-execute bit
            wrmsr(msr::IA32_EFER, rdmsr(msr::IA32_EFER) | (1 << 11));

            smp::core_local& local = smp::core_local::get();

            local.core_id = core_id;
            local.current_thread = nullptr;
            local.ctxbuffer = new proc::context;
            local.idt_handler_entries = new std::uintptr_t[256];
            local.idt_entries = new idt::idt_entry[256];

            local.ist.ist1 = as_uptr(mm::allocate_stack());
            local.ist.ist2 = as_uptr(mm::allocate_stack());
            local.ist.ist3 = as_uptr(mm::allocate_stack());
            local.ist.ist4 = as_uptr(mm::allocate_stack());
            local.ist.ist5 = as_uptr(mm::allocate_stack());
            local.ist.ist6 = as_uptr(mm::allocate_stack());
            local.ist.ist7 = as_uptr(mm::allocate_stack());

            local.gdt.set_ist(&local.ist);
            gdt::install_gdt();
            gdt::reload_gdt_smp();
            idt::init_idt();
            idt::install_idt();

            klog::log("core init done");

            for (std::size_t i = 0; i < 32; i++)
            {
                expect(idt::register_idt(idt::idt_builder(handlers::INTERRUPT_HANDLERS[i]).ist(1), i), "failed to allocate irq for cpu exceptions");
            }

            idt::register_idt(idt::idt_builder(+[](std::uint64_t /*idt*/, std::uint64_t /*err*/) {
                                  debug::log_register(smp::core_local::get().ctxbuffer);
                                  klog::log("Hello world from user space! (actually this is kernel space, this is a syscall)");
                              })
                                  .dpl(0x3)
                                  .ist(1),
                              0x80);

            wait_sync_action([]() { expect(proc::make_process() == 0, "kernel proc should be pid=0"); });

            make_idle();

            proc::make_kthread_args(
                +[](std::uint64_t arg) {
                    thread_local std::size_t last_interrupt_count = 0;
                    klog::log("debugging task value: %lu", arg);
                    while (true)
                    {
                        auto& local = smp::core_local::get();
                        if (last_interrupt_count + 50 < local.timer_tick_count)
                        {
                            last_interrupt_count = local.timer_tick_count;
                            klog::log("ping");
                        }
                    }
                },
                core_id);
            proc::make_kthread_args(
                +[](std::uint64_t arg) {
                    klog::log("I got an argument and I don't know what it's for but here it is: %lu", arg);
                    auto prev_msg = 0;
                    while (true)
                    {
                        auto encoded_keypress = inb(0x60);
                        if (encoded_keypress == prev_msg)
                            continue;
                        else
                            prev_msg = encoded_keypress;
                        // klog::log("Encoded Keypress: %02X, %02X", encoded_keypress, prev_msg);
                        char us_qwerty_translation[] = {0,
                                                        0x1B,
                                                        '1',
                                                        '2',
                                                        '3',
                                                        '4',
                                                        '5',
                                                        '6',
                                                        '7',
                                                        '8',
                                                        '9',
                                                        '0',
                                                        '-',
                                                        '=',
                                                        0x08,
                                                        0x09,
                                                        'Q',
                                                        'W',
                                                        'E',
                                                        'R',
                                                        'T',
                                                        'Y',
                                                        'U',
                                                        'I',
                                                        'O',
                                                        'P',
                                                        '[',
                                                        ']',
                                                        0x0D,
                                                        0x11,
                                                        'A',
                                                        'S',
                                                        'D',
                                                        'F',
                                                        'G',
                                                        'H',
                                                        'J',
                                                        'K',
                                                        'L',
                                                        ';',
                                                        '\'',
                                                        '`',
                                                        0x0E,
                                                        '\\',
                                                        'Z',
                                                        'X',
                                                        'C',
                                                        'V',
                                                        'B',
                                                        'N',
                                                        'M',
                                                        ',',
                                                        '.',
                                                        '/',
                                                        0x0F,
                                                        '*',
                                                        0x13,
                                                        ' ',
                                                        0x02,
                                                        /* function keys (F1-F10) here */ 0,
                                                        0,
                                                        0,
                                                        0,
                                                        0,
                                                        0,
                                                        0,
                                                        0,
                                                        0,
                                                        0,
                                                        /*Numlock and Scroll Lock:*/ 0,
                                                        0,
                                                        '7',
                                                        '8',
                                                        '9',
                                                        '-',
                                                        '4',
                                                        '5',
                                                        '6',
                                                        '+',
                                                        '1',
                                                        '2',
                                                        '3',
                                                        '0',
                                                        '.',
                                                        /* three gaps */ 0,
                                                        0,
                                                        0,
                                                        /* F11, F12 */ 0,
                                                        0};
                        // 0x1B = ESCAPE; 0x08 = BACKSPACE; 0x09 = HORIZONAL TAB, used to represent TAB
                        // 0x0D = CARRIAGE RETURN, used to represent ENTER
                        // 0x11 = DEVICE CONTROL 1, used to represent LEFT CONTROL; 0x0E = SHIFT OUT, used to represent LEFT SHIFT
                        // 0x0F = SHIFT IN, used to represent RIGHT SHIFT; 0x13 = DEVICE CONTROL 3, used to represent LEFT ALT
                        // 0x02 = START OF TEXT, used to represent CAPS LOCK
                        // TODO: Handle F1-F12, Numlock and Scroll Lock, multimedia keys
                        // also Home, Page Up, etc
                        // TODO eventually: distinguish Keypad / (0xE0, 0x35) and enter (0xE0, 0x1C) from normal, and RIGHT CTRL and ALT from LEFT
                        char decoded_keypress = 0;
                        bool is_pressed = true; // false means released

                        if (encoded_keypress < (sizeof(us_qwerty_translation) / sizeof(us_qwerty_translation[0])))
                        {
                            decoded_keypress = us_qwerty_translation[encoded_keypress];
                            is_pressed = true;
                        }
                        else if (encoded_keypress - 0x80 >= 0 &&
                                 encoded_keypress - 0x80 < (sizeof(us_qwerty_translation) / sizeof(us_qwerty_translation[0])))
                        {
                            decoded_keypress = us_qwerty_translation[encoded_keypress - 0x80];
                            is_pressed = false;
                        }
                        if (decoded_keypress != 0)
                        {
                            // THIS IS A TEMPORARY SWITCH CASE FOR DEBUGGING, TODO: DELETE IT AND ACTUALLY HANDLE STUFF
                            char output_str[15] = "";
                            switch (decoded_keypress)
                            {
                            case 0x1B:
                                std::strcpy(output_str, "ESCAPE");
                                break;
                            case 0x08:
                                std::strcpy(output_str, "BACKSPACE");
                                break;
                            case 0x09:
                                std::strcpy(output_str, "TAB");
                                break;
                            case 0x0D:
                                std::strcpy(output_str, "ENTER");
                                break;
                            case 0x11:
                                std::strcpy(output_str, "CTRL");
                                break;
                            case 0x0E:
                                std::strcpy(output_str, "LEFT SHIFT");
                                break;
                            case 0x0F:
                                std::strcpy(output_str, "RIGHT SHIFT");
                                break;
                            case 0x13:
                                std::strcpy(output_str, "ALT");
                                break;
                            case 0x02:
                                std::strcpy(output_str, "CAPS LOCK");
                                break;
                            default:
                                std::strcpy(output_str, (char[2]){decoded_keypress, '\0'});
                                break;
                            }
                            klog::log("Detected Keyboard Input: %s %s", output_str, is_pressed ? "pressed" : "released");
                        }
                    }
                },
                0);
            initialize_apic(smp::core_local::get());

            run_init();
            idle();
        }

        // used for proper frame pointer generation
        void main_wrapper(limine_smp_info* smp) { smp_main(smp); }
    } // namespace

    [[noreturn]] void init(limine_smp_response* smp)
    {
        boot_resource::instance().mark_smp_start();
        std::printf("smp::init(): bootstrap_processor_id=%u\n", smp->bsp_lapic_id);
        std::printf("  cpus: %lu\n", smp->cpu_count);

        for (std::size_t i = 0; i < 256; i++)
        {
            auto& entry = smp::core_local::get().pagemap[256 + i];

            if (!entry)
            {
                entry = paging::make_page_pointer(mm::make_physical(expect_nonnull(mm::pmm_allocate_clean(), "pmm allocate failed")), {
                                                                                                                                          .rw = true,
                                                                                                                                          .us = true,
                                                                                                                                          .x = true,
                                                                                                                                      });
            }
        }

        std::size_t core_id = 0;
        std::size_t bsp_index = 0;
        for (std::size_t i = 0; i < smp->cpu_count; i++)
        {
            if (smp->bsp_lapic_id == smp->cpus[i]->lapic_id)
            {
                bsp_index = i;
                smp->cpus[i]->extra_argument = as_uptr(smp::core_local::get(0).pagemap);
                continue;
            }

            core_id++;
            klog::log("initalizing extra smp core: %lu", i);

            // remember that stack grows down

            smp->cpus[i]->extra_argument =
                as_uptr(smp::core_local::get(core_id).pagemap = as_ptr<paging::page_table_entry>(mm::pmm_allocate_clean())) | core_id;

            paging::sync_page_tables(core_id, 0);
            std::direct_atomic_store_n(as_ptr<std::uint64_t>(&smp->cpus[i]->goto_address), as_uptr(smp::main_wrapper), std::memory_order_seq_cst);
        }

        // initialize myself too!
        klog::log("bootstraped smp from bootstrap core!");
        smp::smp_main(smp->cpus[bsp_index]);
    }
} // namespace smp

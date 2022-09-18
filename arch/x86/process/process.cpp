#include "process.h"
#include "paging/paging.h"
#include <cstdint>
#include <mm/pmm.h>
#include <smp/smp.h>
#include <sync/spinlock.h>

extern "C" std::uint64_t __save_ctx_for_reschedule();

namespace proc
{
    static process processes[1];

    process& get_process(std::uint32_t pid) { return processes[pid]; }

    std::uint32_t make_process() { return -1u; }

    std::uint32_t make_kthread(kthread_ip_t th, std::uint64_t extra) { return make_kthread(th, extra, smp::core_local::get().core_id); }

    std::uint32_t make_kthread(kthread_ip_t th_main, std::uint64_t extra, std::size_t core)
    {
        SPINLOCK_SYNC_BLOCK;

        auto& proc = processes[0];
        std::size_t id = proc.thread_allocator.allocate();
        if (id == -1ul)
            return -1u;

        std::uint32_t id32 = id;

        thread* th = proc.threads[id] = new thread({0, id32});
        auto& ctx = proc.threads[id]->ctx;

        proc.threads[id]->state = proc::thread_state::RUNNING;

        ctx.cs = 8;
        ctx.rflags = 0x202; // idk, StaticSega on osdev discord told me to use this
        ctx.rip = (std::uint64_t)th_main;
        ctx.ss = 0;
        ctx.rsp = (std::uintptr_t)mm::pmm_allocate() + paging::PAGE_SMALL_SIZE;
        ctx.rgp[context::RDI] = extra;

        // insert into a queue
        smp::core_local::get(core).scheduler.add_task(th);
        return id;
    }

    void suspend_self()
    {
        if (__save_ctx_for_reschedule()) {}
    }
} // namespace proc

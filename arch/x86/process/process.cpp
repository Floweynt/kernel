#include "process.h"
#include "paging/paging.h"
#include "process/context.h"
#include <cstdint>
#include <mm/pmm.h>
#include <smp/smp.h>
#include <sync/spinlock.h>

extern "C" std::uint64_t __save_ctx_for_reschedule();

namespace proc
{
    static process processes[1];

    std::uint32_t process::make_thread(std::uintptr_t fp, void* sp, std::uint64_t args, std::size_t core)
    {
        SPINLOCK_SYNC_BLOCK;

        std::size_t id = thread_allocator.allocate();
        if (id == -1ul)
            return -1u;

        std::uint32_t id32 = id;

        thread* th = new thread({id32, pid});
        threads[id] = th;
        auto& ctx = threads[id]->ctx;

        threads[id]->state = proc::thread_state::RUNNING;

        ctx.rgp[proc::context::RDI] = args;
        ctx.cs = 8;
        ctx.rflags = cpuflags::FLAGS | cpuflags::IF;
        ctx.rip = fp;
        ctx.ss = 0;
        ctx.rsp = (std::uintptr_t)sp;

        smp::core_local::get(core).scheduler.add_task(th);
        return id;
    }

    process& get_process(std::uint32_t pid) { return processes[pid]; }

    std::uint32_t make_process() { return -1u; }

    void suspend_self()
    {
        if (__save_ctx_for_reschedule()) {}
    }

    std::uint32_t make_kthread_args(kthread_fn_args_t th, std::uint64_t extra)
    {
        return get_process(0).make_thread((std::uintptr_t)th,
                                          (void*)((std::uintptr_t)mm::pmm_allocate() + paging::PAGE_SMALL_SIZE), extra,
                                          smp::core_local::get().core_id);
    }

} // namespace proc

#include "process.h"
#include "paging/paging.h"
#include "process/context.h"
#include <cstdint>
#include <mm/pmm.h>
#include <smp/smp.h>
#include <sync/spinlock.h>

extern "C" auto __save_ctx_for_reschedule() -> std::uint64_t;

namespace proc
{
    static process processes[1];

    auto process::make_thread(std::uintptr_t entry, void* stack_ptr, std::uint64_t args, std::size_t core) -> std::uint32_t
    {
        SPINLOCK_SYNC_BLOCK;

        std::size_t id = thread_allocator.allocate();
        if (id == -1UL)
        {
            return -1u;
        }

        std::uint32_t id32 = id;

        auto* th = new thread({id32, pid});
        threads[id] = th;
        auto& ctx = threads[id]->ctx;

        threads[id]->state = proc::thread_state::RUNNING;

        ctx.rgp[proc::context::RDI] = args;
        ctx.cs = 8;
        ctx.rflags = cpuflags::FLAGS | cpuflags::IF;
        ctx.rip = entry;
        ctx.ss = 0;
        ctx.rsp = (std::uintptr_t)stack_ptr;

        smp::core_local::get(core).scheduler.add_task(th);
        return id;
    }

    auto get_process(std::uint32_t pid) -> process& { return processes[pid]; }

    auto make_process() -> std::uint32_t { return -1U; }

    void suspend_self()
    {
        if (__save_ctx_for_reschedule()) {}
    }

    auto make_kthread_args(kthread_fn_args_t thread_fn, std::uint64_t extra) -> std::uint32_t
    {
        return get_process(0).make_thread((std::uintptr_t)thread_fn, (void*)((std::uintptr_t)mm::pmm_allocate() + paging::PAGE_SMALL_SIZE), extra,
                                          smp::core_local::get().core_id);
    }

} // namespace proc

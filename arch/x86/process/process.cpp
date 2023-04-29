#include "process.h"
#include "paging/paging.h"
#include "process/context.h"
#include <cast.h>
#include <cstdint>
#include <gsl/pointer>
#include <klog/klog.h>
#include <mm/pmm.h>
#include <paging/paging_entries.h>
#include <smp/smp.h>
#include <sync/spinlock.h>

extern "C" auto __save_ctx_for_reschedule() -> std::uint64_t;

namespace proc
{
    static process processes[2];

    auto process::make_thread(const context& inital_context, std::size_t core) -> std::uint32_t
    {
        SPINLOCK_SYNC_BLOCK;

        std::size_t tid = thread_allocator.allocate();
        if (tid == -1UL)
        {
            return -1U;
        }

        std::uint32_t id32 = tid;

        gsl::owner<thread*> th = new thread({id32, pid});
        threads[tid] = th;
        th->ctx = inital_context;
        th->state = proc::thread_state::RUNNING;

        smp::core_local::get(core).scheduler.add_task(th);
        return tid;
    }

    auto get_process(std::uint32_t pid) -> process& { return processes[pid]; }

    // TODO: allocate process
    auto make_process() -> std::uint32_t { return 1U; }

    void suspend_self()
    {
        if (__save_ctx_for_reschedule()) {}
    }

    auto make_kthread_args(kthread_fn_args_t thread_fn, std::uint64_t extra, std::size_t core) -> std::uint32_t
    {
        auto* stack = mm::pmm_allocate();
        auto* pages = mm::pmm_allocate();

        if (stack == nullptr || pages == nullptr)
        {
            klog::panic("failed to allocate buffers for kernel thread stack and page table");
        }

        paging::copy_kernel_page_tables(as_ptr(pages), smp::core_local::get(core).pagemap);

        return get_process(0).make_thread(context_builder(context_builder::KERNEL, as_uptr(thread_fn))
                                              .set_reg(context::RDI, extra)
                                              .set_flag(cpuflags::IF)
                                              .set_stack(as_uptr(stack) + paging::PAGE_SMALL_SIZE)
                                              .set_cr3(as_uptr(pages))
                                              .build(),
                                          core);
    }

    auto make_kthread_args(kthread_fn_args_t thread_fn, std::uint64_t extra) -> std::uint32_t
    {
        return make_kthread_args(thread_fn, extra, smp::core_local::get().core_id);
    }
} // namespace proc

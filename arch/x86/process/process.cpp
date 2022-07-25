#include "process.h"
#include <mm/pmm.h>
#include <smp/smp.h>
#include <sync/spinlock.h>

namespace proc
{
    static process processes[1];

    process& get_process(uint32_t pid)
    {
        return processes[pid];
    }

    uint32_t create_process()
    {
        return -1u;
    }

    uint32_t make_kthread(kthread_ip_t th, uint64_t extra)
    {
        return make_kthread(th, extra, smp::core_local::get().coreid);
    }

    uint32_t make_kthread(kthread_ip_t th, uint64_t extra, std::size_t core)
    {
        SPINLOCK_SYNC_BLOCK;

        auto& proc = processes[0];
        uint32_t id = proc.thread_allocator.allocate();
        auto& c = proc.threads[id].ctx;

        proc.threads[id].state = proc::thread_state::RUNNING;

        c.cs = 8;
        c.rflags = 0x202; // idk, StaticSega on osdev discord told me to use this
        c.rip = (uint64_t) th;
        c.ss = 0;
        c.rsp = (uint64_t) mm::pmm_allocate() + 4096;
        c.rgp[context::RDI] = extra;

        // insert into a queue
        smp::core_local::get(core).tasks->push(task_id{id, 0});
        return id;
    }
}

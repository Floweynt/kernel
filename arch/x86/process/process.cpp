#include "process.h"
#include <mm/pmm.h>
#include <smp/smp.h>

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
        make_kthread(th, extra, smp::core_local::get().coreid);
    }

    uint32_t make_kthread(kthread_ip_t th, uint64_t extra, std::size_t core)
    {
        static uint32_t current_idx = 0;
        context& c = processes[0].threads[current_idx++].ctx;
        c.cs = 8;
        c.rflags = 0x202; // idk, StaticSega on osdev discord told me to use this
        c.rip = (uint64_t) th;
        c.ss = 0;
        c.rsp = (uint64_t) mm::pmm_allocate() + 4096;
        c.rgp[context::RDI] = extra;

        // insert into a queue
        smp::core_local::get(core).tasks.push(task_id{current_idx - 1, 0});

        return current_idx - 1;
    }
}

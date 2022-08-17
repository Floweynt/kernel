#ifndef __ARCH_X86_PROCESS_H__
#define __ARCH_X86_PROCESS_H__
#include <context/context.h>
#include <mm/mm.h>
#include <cstddef>
#include <utils/id_allocator.h>

namespace proc
{
    enum class thread_state
    {
        RUNNING,
        WAITING
    };

    struct thread
    {
        context ctx;
        uintptr_t cr3;
        thread_state state;
    };

    class process
    {
    public:
        inline static constexpr std::size_t MAX_THREADS = 16;
        inline static constexpr std::size_t MAX_PROCESS = 1;

        id_allocator<MAX_THREADS> thread_allocator;
        thread threads[MAX_THREADS];
    };

    struct task_id
    {
        uint32_t thread;
        uint32_t proc;

        static task_id from_int(uint64_t v) { return { uint32_t(v & 0xffffffff), uint32_t(v >> 32) }; }
        constexpr operator uint64_t() { return ((uint64_t)proc << 32) | thread; }
    };

    process& get_process(uint32_t pid);
    uint32_t create_process();

    using kthread_ip_t = void (*)(uint64_t);
    uint32_t make_kthread(kthread_ip_t th, uint64_t extra);
    uint32_t make_kthread(kthread_ip_t th, uint64_t extra, std::size_t core);
}

#endif

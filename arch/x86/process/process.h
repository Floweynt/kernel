#ifndef __ARCH_X86_PROCESS_H__
#define __ARCH_X86_PROCESS_H__
#include "context.h"
#include <cstddef>
#include <mm/mm.h>
#include <utils/id_allocator.h>

namespace proc
{
    enum class thread_state : std::uint8_t
    {
        RUNNING,
        WAITING,
        IDLE // reserved how for the idle process
    };

    struct task_id
    {
        std::uint32_t thread;
        std::uint32_t proc;

        static task_id from_int(std::uint64_t v) { return {std::uint32_t(v & 0xffffffff), std::uint32_t(v >> 32)}; }
        constexpr operator std::uint64_t() { return ((std::uint64_t)proc << 32) | thread; }
        constexpr bool valid() { return thread != 0xffffffff; }
    };

    inline constexpr auto NIL_TID = task_id{0xffffffff, 0xffffffff};

    constexpr bool operator==(const task_id& lhs, const task_id& rhs)
    {
        return lhs.thread == rhs.thread && rhs.proc == lhs.proc;
    }

    constexpr bool operator!=(const task_id& lhs, const task_id& rhs) { return !(lhs == rhs); }

    struct thread
    {
        context ctx;
        std::uintptr_t cr3;
        // scheduler information
        std::size_t sched_index;
        std::size_t latest_scheduled_tick;
        const task_id id;
        thread_state state;
        constexpr thread(const task_id id) : id(id) {}
    };

    class process
    {
    public:
        inline static constexpr std::size_t MAX_THREADS = 16;
        inline static constexpr std::size_t MAX_PROCESS = 1;

        id_allocator<MAX_THREADS> thread_allocator;
        thread* threads[MAX_THREADS];
    };

    process& get_process(std::uint32_t pid);
    std::uint32_t make_process();

    constexpr thread& get_thread(task_id id) { return *get_process(id.proc).threads[id.thread]; }

    using kthread_ip_t = void (*)(std::uint64_t);
    std::uint32_t make_kthread(kthread_ip_t th, std::uint64_t extra);
    std::uint32_t make_kthread(kthread_ip_t th, std::uint64_t extra, std::size_t core);

    void suspend_self();
} // namespace proc

#endif

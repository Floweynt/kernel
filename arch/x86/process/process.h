#pragma once

#include "context.h"
#include <asm/asm_cpp.h>
#include <cstddef>
#include <cstdint>
#include <mm/mm.h>
#include <mm/pmm.h>
#include <paging/paging.h>
#include <sync/spinlock.h>
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

        static auto from_int(std::uint64_t value) -> task_id { return {std::uint32_t(value & 0xffffffff), std::uint32_t(value >> 32)}; }
        constexpr operator std::uint64_t() const { return ((std::uint64_t)proc << 32) | thread; }
        [[nodiscard]] constexpr auto valid() const -> bool { return thread != 0xffffffff; }
    };

    inline constexpr auto NIL_TID = task_id{0xffffffff, 0xffffffff};

    constexpr auto operator==(const task_id& lhs, const task_id& rhs) -> bool { return lhs.thread == rhs.thread && rhs.proc == lhs.proc; }

    constexpr auto operator!=(const task_id& lhs, const task_id& rhs) -> bool { return !(lhs == rhs); }

    struct thread
    {
        context ctx{};
        std::uintptr_t cr3{};
        // scheduler information
        // std::size_t sched_index;
        // std::size_t latest_scheduled_tick;
        const task_id id;
        thread_state state{};
        constexpr thread(task_id task_id) : id(task_id) {}
    };

    class process
    {
    public:
        inline static constexpr std::size_t MAX_THREADS = 16;
        inline static constexpr std::size_t MAX_PROCESS = 1;

    private:
        id_allocator<MAX_THREADS> thread_allocator;
        thread* threads[MAX_THREADS];
        std::uint32_t pid = 0;

    public:
        auto make_thread(const context& inital_context, std::size_t core) -> std::uint32_t;
        auto get_thread(std::uint32_t tid) -> thread* { return threads[tid]; }
    };

    auto get_process(std::uint32_t pid) -> process&;
    auto make_process() -> std::uint32_t;

    constexpr auto get_thread(task_id tid) -> thread& { return *get_process(tid.proc).get_thread(tid.thread); }

    using kthread_fn_t = void (*)(std::uint64_t);
    using kthread_fn_args_t = void (*)(std::uint64_t);

     auto make_kthread_args(kthread_fn_args_t thread_fn, std::uint64_t extra, std::size_t core) -> std::uint32_t;
     auto make_kthread_args(kthread_fn_args_t thread_fn, std::uint64_t extra) -> std::uint32_t;
    inline auto make_kthread(kthread_fn_t thread_fn) -> std::uint32_t { return make_kthread_args(thread_fn, 0); }
    inline auto make_kthread(kthread_fn_t thread_fn, std::size_t core) -> std::uint32_t { return make_kthread_args(thread_fn, 0, core); }

    void suspend_self();
} // namespace proc

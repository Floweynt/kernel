// cSpell:ignore apic, stivale
#pragma once

#include <apic/apic.h>
#include <process/context.h>
#include <cstddef>
#include <gdt/gdt.h>
#include <idt/idt.h>
#include <paging/paging.h>
#include <process/scheduler/scheduler.h>
#include <utils/id_allocator.h>

namespace smp
{
    class core_local
    {
        inline static core_local** entries = nullptr;

    public:
        // the following 3 members of this struct MUST never be moved without first changing idt.S
        proc::context* ctxbuffer;
        std::uintptr_t* idt_handler_entries;
        idt::idt_entry* idt_entries;

        // a unique value from [0, smp_count) representing this core
        std::size_t core_id;

        // lapic
        apic::local_apic apic;
        id_allocator<256> irq_allocator;
        paging::page_table_entry* pagemap;

        // gdt
        gdt::gdt_entries gdt;
        gdt::interrupt_stack_table ist;

        // scheduler stuff
        proc::thread* current_thread;
        scheduler::scheduler scheduler;

        // misc
        std::size_t timer_tick_count{};

        inline static auto get() -> core_local&
        {
            core_local* addr = nullptr;
            asm volatile("movq %%gs:0, %0" : "=r"(addr) :);
            return *addr;
        }

        inline static auto get_pointer() -> core_local*
        {
            core_local* addr = nullptr;
            asm volatile("movq %%gs:0, %0" : "=r"(addr) :);
            return addr;
        }

        static void create(core_local* cpu0);
        static inline auto exists() -> bool { return entries != nullptr && get_pointer() != nullptr; }
        inline static auto get(std::size_t core) -> core_local& { return *entries[core]; }

        inline static auto gs_of(std::size_t core) -> std::uint64_t { return (std::uint64_t)&entries[core]; }

        core_local() = default;
        core_local(const core_local&) = delete;
        core_local(core_local&&) = delete;
        auto operator=(const core_local&) -> core_local& = delete;
        auto operator=(core_local&&) -> core_local& = delete;
    };

    [[noreturn]] void init(limine_smp_response* smp);
} // namespace smp

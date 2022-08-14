// cSpell:ignore apic, stivale
#ifndef __ARCH_X86_SMP_SMP_H__
#define __ARCH_X86_SMP_SMP_H__
#include <apic/apic.h>
#include <context/context.h>
#include <cstddef>
#include <gdt/gdt.h>
#include <idt/idt.h>
#include <kinit/stivale2.h>
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
        context* ctxbuffer;
        uintptr_t* idt_handler_entries;
        idt::idt_entry* idt_entries;

        // a unique value from [0, smp_count) representing this core
        std::size_t coreid;

        // lapic
        apic::local_apic apic;
        id_allocator<256> irq_allocator;
        paging::page_table_entry* pagemap;

        // gdt
        gdt::gdt_entries gdt;
        gdt::interrupt_stack_table ist;

        // scheduler stuff
        proc::task_id current_tid;
        scheduler::task_queue* tasks;

        inline static core_local& get()
        {
            core_local* addr;
            asm volatile("movq %%gs:0, %0" : "=r"(addr) :);
            return *addr;
        }

        inline static core_local* get_pointer()
        {
            core_local* addr;
            asm volatile("movq %%gs:0, %0" : "=r"(addr) :);
            return addr;
        }

        static void create(core_local* cpu0);
        static inline bool exists() { return entries != nullptr && get_pointer() != nullptr; }
        inline static core_local& get(std::size_t core) { return *entries[core]; }

        inline static uint64_t gs_of(std::size_t core) { return (uint64_t)&entries[core]; }
    };

    struct init_data
    {
        std::size_t coreid;
        paging::page_table_entry* cr3;
    };
    static_assert(sizeof(init_data) < paging::PAGE_SMALL_SIZE);

    [[noreturn]] void init(stivale2_struct_tag_smp*);
} // namespace smp

#endif

// cSpell:ignore apic, stivale
#ifndef __ARCH_X86_SMP_SMP_H__
#define __ARCH_X86_SMP_SMP_H__
#include <context/context.h>
#include <cstddef>
#include <idt/idt.h>
#include <kinit/stivale2.h>
#include <apic/apic.h>
#include <paging/paging_entries.h>
#include <process/scheduler/scheduler.h>
#include <utils/id_allocator.h>
namespace smp
{
    class core_local
    {
        inline static core_local** entries = nullptr;
    public:
        context* ctxbuffer;
        uintptr_t* idt_handler_entries;
        idt::idt_entry* idt_entries;
        std::size_t coreid;
        apic::local_apic apic;
        id_allocator<256> irq_allocator;
        paging::page_table_entry* pagemap;
        
        // scheduler stuff
        proc::task_id current_tid;
        scheduler::task_queue* tasks;

        inline static core_local& get()
        {
            core_local* addr;
            __asm__ __volatile__("movq %%gs:0, %0" : "=r"(addr) :);
            return *addr;
        }

        inline static core_local* get_pointer()
        {
            core_local* addr;
            __asm__ __volatile__("movq %%gs:0, %0" : "=r"(addr) :);
            return addr;
        }

        static void create();
        static inline bool exists()
        {
            return entries != nullptr && get_pointer() != nullptr;
        }
        inline static core_local& get(std::size_t core) { return *entries[core]; }

        inline static uint64_t gs_of(std::size_t core) { return (uint64_t)&entries[core]; }
    };

    static_assert(sizeof(core_local) < 4096, "core_local buffer is larger than expected");

    [[noreturn]] void init(stivale2_struct_tag_smp*);
} // namespace smp

#endif

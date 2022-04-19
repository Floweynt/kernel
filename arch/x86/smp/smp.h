#ifndef __ARCH_X86_SMP_SMP_H__
#define __ARCH_X86_SMP_SMP_H__
#include <context/context.h>
#include <cstddef>
#include <idt/idt.h>
#include <kinit/stivale2.h>
#include <paging/paging_entries.h>

namespace smp
{
    class core_local
    {
        inline static core_local** entries;

    public:
        context* ctxbuffer;
        paging::page_table_entry* pagemap;
        uintptr_t idt_handler_entries[256];

        inline static core_local& get()
        {
            core_local* addr;
            __asm__ __volatile__("movq %%gs:0, %0" : "=r"(addr) :);
            return *addr;
        }

        static void create();
        inline static core_local& get(std::size_t core) { return *entries[core]; }

        inline static uint64_t gs_of(std::size_t core) { return (uint64_t)&entries[core]; }
    };

    [[noreturn]] void initalize_smp(stivale2_smp_info*);
} // namespace smp

#endif

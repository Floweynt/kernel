#ifndef __ARCH_X86_SMP_SMP_H__
#define __ARCH_X86_SMP_SMP_H__
#include <paging/paging_entries.h>

namespace smp
{
    class core_local
    {
        static core_local** entries;
    public:
        paging::page_table_entry* pagemap;

        static core_local& get()
        {
            core_local* addr;
            __asm__ __volatile__("movq gs:0, %0" : "=r"(addr) :);
            return *addr;
        }
    };
}

#endif

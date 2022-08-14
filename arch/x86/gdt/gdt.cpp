#include "gdt.h"
#include <smp/smp.h>
#include <asm/asm_cpp.h>
#include <utils/utils.h>

namespace gdt
{
    static uint64_t gdt[] = {0, 0x00af9b000000ffff, 0x00af93000000ffff};

    void install_gdt()
    {
        utils::packed_tuple<uint16_t, uint64_t> d(sizeof(gdt), (uint64_t)gdt);
        lgdt(&d);

        asm volatile goto("pushq $8\n"
                          "push %0\n"
                          "lretq\n"
                          :
                          :
                          :
                          : handle_segments);
    handle_segments:
        asm volatile("movw $16, %%ax\n"
                     "movw %%ax, %%es\n"
                     "movw %%ax, %%ss\n"
                     "movw %%ax, %%ds\n"
                     :
                     :
                     : "%rax");
    }

    void reload_gdt_smp()
    {
        utils::packed_tuple<uint16_t, uint64_t> d(sizeof(gdt_entries), (uint64_t)&smp::core_local::get().gdt);
        lgdt(&d);
        ltr(40);
    }
} // namespace gdt

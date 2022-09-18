#include "gdt.h"
#include <asm/asm_cpp.h>
#include <smp/smp.h>
#include <utils/utils.h>

namespace gdt
{
    static std::uint64_t gdt[] = {0, 0x00af9b000000ffff, 0x00af93000000ffff};

    void install_gdt()
    {
        utils::packed_tuple<std::uint16_t, std::uint64_t> desc(sizeof(gdt), (std::uint64_t)gdt);
        asm volatile("lgdtq %0" : : "m"(desc));
        asm volatile goto("pushq $8\n"
                          "push $%0\n"
                          "lretq\n"
                          :
                          :
                          :
                          : install_gdt_handle_segments);
    install_gdt_handle_segments:
        asm volatile("movw $16, %%ax\n"
                     "movw %%ax, %%es\n"
                     "movw %%ax, %%ss\n"
                     "movw %%ax, %%ds\n"

                     :
                     :
                     : "%rax");
        return;
    }

    void reload_gdt_smp()
    {
        utils::packed_tuple<std::uint16_t, std::uint64_t> desc(sizeof(gdt_entries), (std::uint64_t)&smp::core_local::get().gdt);

        asm volatile("lgdtq %0" : : "m"(desc));
        asm volatile goto("pushq $8\n"
                          "push $%0\n"
                          "lretq\n"
                          :
                          :
                          :
                          : reload_gdt_smp_handle_segments);
    reload_gdt_smp_handle_segments:
        asm volatile("movw $16, %%ax\n"
                     "movw %%ax, %%es\n"
                     "movw %%ax, %%ss\n"
                     "movw %%ax, %%ds\n"

                     :
                     :
                     : "%rax");

        ltr(40);
    }
} // namespace gdt

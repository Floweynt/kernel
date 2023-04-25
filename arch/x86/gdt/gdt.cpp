#include "gdt.h"
#include <asm/asm_cpp.h>
#include <smp/smp.h>
#include <utils/utils.h>

namespace gdt
{
    static std::uint64_t gdt[] = {0, 0x00af9b000000ffff, 0x00af93000000ffff, 0x00affb000000ffff, 0x00aff3000000ffff};
    using gdt_desc = utils::packed_tuple<std::uint16_t, std::uint64_t>;
    void reload_gdt()
    {
        asm volatile goto("pushq $8\n"
                          "push $%0\n"
                          "lretq\n"
                          :
                          :
                          :
                          : reload_gdt_handle_segments);
    reload_gdt_handle_segments:
        asm volatile("movw $16, %%ax\n"
                     "movw %%ax, %%es\n"
                     "movw %%ax, %%ss\n"
                     "movw %%ax, %%ds\n"
                     :
                     :
                     : "%rax");
    }

    void install_gdt()
    {
        gdt_desc desc(sizeof(gdt), as_uptr(gdt));
        asm volatile("lgdtq %0" : : "m"(desc));
        reload_gdt();
    }

    void reload_gdt_smp()
    {
        gdt_desc desc(sizeof(gdt_entries), as_uptr(&smp::core_local::get().gdt));
        asm volatile("lgdtq %0" : : "m"(desc));
        reload_gdt();
        ltr(40);
    }
} // namespace gdt

#include "idt.h"
#include <asm/asm_cpp.h>
#include <smp/smp.h>

extern char idt_entry_start[];

namespace idt
{
    struct [[gnu::packed]] idt_descriptor
    {
        uint16_t size;
        uint64_t offset;
    };

    void init_idt()
    {
        smp::core_local& local = smp::core_local::get();
        for (int i = 0; i < 256; i++)
        {
            uint64_t handler = (uint64_t)idt_entry_start + i * 0x10;
            local.idt_entries[i].offset_low = (uint64_t)handler;
            local.idt_entries[i].offset_mid = (uint64_t)handler >> 16;
            local.idt_entries[i].offset_high = ((uint64_t)handler) >> 32;
        }
    }

    void install_idt()
    {
        smp::core_local& local = smp::core_local::get();
        idt_descriptor descriptor = {.size = sizeof(idt_entry) * 256, .offset = (uint64_t)local.idt_entries};
        lidt((void*)&descriptor);
    }

    bool register_idt(interrupt_handler handler, std::size_t num, uint8_t type, uint8_t dpl)
    {
        smp::core_local& local = smp::core_local::get();
        local.idt_handler_entries[num] = (uintptr_t)handler;
        bool ret = local.irq_allocator.allocate(num);
        smp::core_local::get().idt_entries[num].flags = 0x8000 | ((uint16_t)dpl << 13) | ((uint16_t)type << 8);
        return ret;
    }

    std::size_t register_idt(interrupt_handler handler, uint8_t type, uint8_t dpl)
    {
        smp::core_local& local = smp::core_local::get();
        std::size_t num = local.irq_allocator.allocate();
        if (num == -1ul)
            return -1ul;

        local.idt_handler_entries[num] = (uintptr_t)handler;
        smp::core_local::get().idt_entries[num].flags = 0x8000 | ((uint16_t)dpl << 13) | ((uint16_t)type << 8);
        return num;
    }
} // namespace idt

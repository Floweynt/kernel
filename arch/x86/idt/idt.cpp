#include "idt.h"
#include <asm/asm_cpp.h>
#include <smp/smp.h>
#include <utils/utils.h>

extern char idt_entry_start[];

namespace idt
{
    void init_idt()
    {
        smp::core_local& local = smp::core_local::get();
        for (int i = 0; i < 256; i++)
        {
            std::uint64_t handler = (std::uint64_t)idt_entry_start + i * 0x10;
            local.idt_entries[i].offset_low = (std::uint64_t)handler;
            local.idt_entries[i].offset_mid = (std::uint64_t)handler >> 16;
            local.idt_entries[i].offset_high = ((std::uint64_t)handler) >> 32;
        }
    }

    void install_idt()
    {
        smp::core_local& local = smp::core_local::get();
        utils::packed_tuple<std::uint16_t, std::uint64_t> d(sizeof(idt_entry) * 256, (std::uint64_t)local.idt_entries);
        asm volatile("lidtq %0" : : "m"(d));
    }

    bool register_idt(const idt_builder& entry, std::size_t num)
    {
        smp::core_local& local = smp::core_local::get();
        if(!local.irq_allocator.allocate(num))
            return false;
        local.idt_handler_entries[num] = (std::uintptr_t) entry.cb();
        local.idt_entries[num].flags = entry.flag();
        return true;
    }

    std::size_t register_idt(const idt_builder& entry)
    {
        smp::core_local& local = smp::core_local::get();
        std::size_t num = local.irq_allocator.allocate();
        if (num == -1ul)
            return -1ul;

        local.idt_handler_entries[num] = (std::uintptr_t) entry.cb();
        local.idt_entries[num].flags = entry.flag();
        return num;
    }
} // namespace idt

#include "idt.h"
#include <asm/asm_cpp.h>
#include <cstdint>
#include <smp/smp.h>
#include <utils/utils.h>

extern char idt_entry_start[];

extern "C" void _handle_irq_common(std::uint64_t int_no, std::uint64_t errno)
{
    ((idt::interrupt_handler)smp::core_local::get().idt_handler_entries[int_no])(int_no, errno);
}

namespace idt
{
    inline constexpr auto IDT_ENTRIES = 256;
    void init_idt()
    {
        smp::core_local& local = smp::core_local::get();
        for (int i = 0; i < IDT_ENTRIES; i++)
        {
            std::uint64_t handler = as_uptr(idt_entry_start) + i * 0x10;
            local.idt_entries[i].offset_low = (std::uint64_t)handler;
            local.idt_entries[i].offset_mid = (std::uint64_t)handler >> 16;
            local.idt_entries[i].offset_high = ((std::uint64_t)handler) >> 32;
        }
    }

    void install_idt()
    {
        utils::packed_tuple<std::uint16_t, std::uintptr_t> desc(sizeof(idt_entry) * IDT_ENTRIES, as_uptr(smp::core_local::get().idt_entries));
        asm volatile("lidtq %0" : : "m"(desc));
    }

    auto register_idt(const idt_builder& entry, std::size_t num) -> bool
    {
        smp::core_local& local = smp::core_local::get();
        if (!local.irq_allocator.allocate(num))
        {
            return false;
        }
        local.idt_handler_entries[num] = as_uptr(entry.get_handler());
        local.idt_entries[num].flags = entry.flag();
        return true;
    }

    auto register_idt(const idt_builder& entry) -> std::size_t
    {
        smp::core_local& local = smp::core_local::get();
        std::size_t num = local.irq_allocator.allocate();
        if (num == -1UL)
        {
            return -1UL;
        }

        local.idt_handler_entries[num] = as_uptr(entry.get_handler());
        local.idt_entries[num].flags = entry.flag();
        return num;
    }
} // namespace idt

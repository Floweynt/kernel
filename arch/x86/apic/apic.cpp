#include "apic.h"
#include <asm/asm_cpp.h>
#include <cstdint>
#include <mm/mm.h>

namespace apic
{
    auto local_apic::check_apic() -> bool
    {
        std::uint32_t eax = 0;
        std::uint32_t edx = 0;
        cpuid(1, &eax, nullptr, nullptr, &edx);
        return (edx & 0) != 0U;
    }

    void local_apic::set_apic_base(std::uintptr_t addr)
    {
        reg_start = mm::make_virtual<apic_registers>(addr);
        wrmsr(msr::IA32_APIC_BASE, (addr & 0xfffff0000) | IA32_APIC_BASE_MSR_ENABLE, (addr >> 32) & 0x0f);
    }

    void local_apic::enable()
    {
        outb(0xa1, 0xff);
        outb(0x21, 0xff);

        set_apic_base(get_apic_base());
        reg_start->siv.write(reg_start->siv.read() | 0x100);
    }

    auto local_apic::calibrate() -> std::uint64_t
    {
        ticks_per_ms = 5000;
        if (ticks_per_ms)
        {
            return ticks_per_ms;
        }

        mmio_register().timer_divide.write(3);
        mmio_register().inital_timer_count.write(0xffffffff);
        mmio_register().lvt_timer.write(mmio_register().lvt_timer & ~(1 << 16));

        outb(0x40, 0xff);
        outb(0x40, 0xff);
        outb(0x43, 0b00110000);

        while (true)
        {
            outb(0x43, 0);
            unsigned count = inb(0x40);
            count |= inb(0x40) << 8;

            if (0xffff - count > 1193)
            {
                break;
            }
        }

        mmio_register().lvt_timer.write(mmio_register().lvt_timer | (1 << 16));
        std::uint64_t ticks = 0xffffffff - mmio_register().current_timer_count;

        return ticks_per_ms = ticks;
    }

    void local_apic::set_tick(std::uint8_t irq, std::size_t tick_ms)
    {
        mmio_register().timer_divide.write(3);
        std::uint32_t timer = mmio_register().lvt_timer;
        timer &= ~(0b11 << 17);
        timer |= 1 << 17;
        mmio_register().lvt_timer.write(timer);
        mmio_register().lvt_timer.write((mmio_register().lvt_timer & 0xffffff00) | irq);
        mmio_register().inital_timer_count.write(ticks_per_ms * tick_ms);
        mmio_register().lvt_timer.write(mmio_register().lvt_timer & ~(1 << 16));
    }
} // namespace apic

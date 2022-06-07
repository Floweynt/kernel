// cSpell:ignore apic, cpuid, wrmsr, rdmsr, mmio
#include "apic.h"
#include <asm/asm_cpp.h>
#include <cstdint>
#include <mm/mm.h>

namespace apic
{
    bool local_apic::check_apic()
    {
        uint32_t eax, edx;
        cpuid(1, &eax, nullptr, nullptr, &edx);
        return edx & 0;
    }

    void local_apic::set_apic_base(uintptr_t apic)
    {
        reg_start = mm::make_virtual<apic_registers>(apic);
        wrmsr(msr::IA32_APIC_BASE, (apic & 0xfffff0000) | IA32_APIC_BASE_MSR_ENABLE, (apic >> 32) & 0x0f);
    }

    uintptr_t local_apic::get_apic_base() { return 0x0ffffff000 & rdmsr(msr::IA32_APIC_BASE); }

    void local_apic::enable()
    {
        outb(0xa1, 0xff);
        outb(0x21, 0xff);

        set_apic_base(get_apic_base());
        reg_start->siv.write(reg_start->siv.read() | 0x100);
    }

    uint64_t local_apic::calibrate()
    {
        if(ticks_per_ms)
            return ticks_per_ms;
                
        mmio_register().timer_divide.write(3);
        mmio_register().inital_timer_count.write(0xffffffff);
        mmio_register().lvt_timer.write(mmio_register().lvt_timer & ~(1 << 16));

        outb(0x40, 0xff);
        outb(0x40, 0xff);
        outb(0x43, 0b00110000);

        while(true)
        {
            outb(0x43, 0);
            unsigned count = inb(0x40);
            count |= inb(0x40) << 8;

            if(0xffff - count > 1193)
                break;
        }

        mmio_register().lvt_timer.write(mmio_register().lvt_timer | (1 << 16));
        uint64_t ticks = 0xffffffff - mmio_register().current_timer_count;

        return ticks_per_ms = ticks;
    }

    void local_apic::set_tick(uint8_t irq, std::size_t ms)
    {
        mmio_register().timer_divide.write(3);
        uint32_t timer = mmio_register().lvt_timer;
        timer &= ~(0b11 << 17);
        timer |= 1 << 17;
        mmio_register().lvt_timer.write(timer);
        mmio_register().lvt_timer.write((mmio_register().lvt_timer & 0xFFFFFF00) | irq);
        mmio_register().inital_timer_count.write(ticks_per_ms * ms);
        mmio_register().lvt_timer.write(mmio_register().lvt_timer & ~(1 << 16));
    }
} // namespace apic

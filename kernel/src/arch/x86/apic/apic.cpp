#include <apic/apic.h>
#include <apic/apic_flag_builder.h>
#include <asm/asm_cpp.h>
#include <cstdint>
#include <klog/klog.h>
#include <mm/mm.h>
#include <sync/spinlock.h>

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
        wrmsr(msr::IA32_APIC_BASE, (addr & MASK_LAPIC_BASE_ADDR) | IA32_APIC_BASE_MSR_ENABLE, (addr >> 32) & MASK_LAPIC_BASE_ADDR_HI);
    }

    void local_apic::enable()
    {
        outb(ioports::PIC_SLAVE_DATA, 0xff);
        outb(ioports::PIC_MASTER_DATA, 0xff);

        set_apic_base(get_apic_base());
        reg_start->siv.write(reg_start->siv.read() | SIV_APIC_SOFTWARE_ENABLE);
    }

    namespace
    {
        inline constexpr auto TIMER_INIT_VALUE = 0xffff;
        inline constexpr auto TIMER_AFTER_MS = 1193;

        void write_pit_timer(std::uint16_t ticks)
        {
            outb(ioports::PIT_MODE_COMMAND, 0b00110000);
            outb(ioports::PIT_DATA_CHAN_0, ticks);
            outb(ioports::PIT_DATA_CHAN_0, ticks >> 8);
        }

        auto read_pit_timer() -> std::uint16_t
        {
            outb(ioports::PIT_MODE_COMMAND, 0);
            unsigned count = inb(ioports::PIT_DATA_CHAN_0);
            count |= inb(ioports::PIT_DATA_CHAN_0) << 8;
            return count;
        }

        void wait_ms()
        {
            write_pit_timer(TIMER_INIT_VALUE);

            while (true)
            {
                auto count = read_pit_timer();

                if (TIMER_INIT_VALUE - count > TIMER_AFTER_MS)
                {
                    break;
                }
            }
        }
    } // namespace

    auto local_apic::calibrate() -> std::uint64_t
    {
        if (ticks_per_ms)
        {
            return ticks_per_ms;
        }
        auto lvt_timer_reg = mmio_register().lvt_timer.read();

        mmio_register().timer_divide.write(3);
        mmio_register().inital_timer_count.write(~0U);
        mmio_register().lvt_timer.write(build_lvt_timer(lvt_timer_mode::ONE_SHOT, false, false, 0));

        wait_ms();

        mmio_register().lvt_timer.write(lvt_timer_reg);
        std::uint64_t ticks = ~0U - mmio_register().current_timer_count;

        return ticks_per_ms = ticks;
    }

    void local_apic::set_tick(std::uint8_t irq, std::size_t tick_ms)
    {
        mmio_register().timer_divide.write(3);
        mmio_register().inital_timer_count.write(ticks_per_ms * tick_ms);
        mmio_register().lvt_timer.write(build_lvt_timer(lvt_timer_mode::PERIODIC, false, false, irq));
    }
} // namespace apic

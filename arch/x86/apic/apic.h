// cSpell:ignore apic, mmio, cmci, lapic, apit
#ifndef __ARCH_X86_APIC_APIC_H__
#define __ARCH_X86_APIC_APIC_H__
#include <asm/asm_cpp.h>
#include <cstddef>
#include <cstdint>
#include <mmio/register.h>

namespace apic
{
    /// \brief Local APIC wrapper class
    /// Handles the control, initialization, and operation of the local apic.
    /// The MMIO registers used in this class are referenced from https://wiki.osdev.org/APIC#Local_APIC_registers
    class local_apic
    {
    public:
        using register_rw = mmio::register_rw<uint32_t, 16>;
        using register_rdonly = mmio::register_rdonly<uint32_t, 16>;
        using register_wronly = mmio::register_wronly<uint32_t, 16>;
        using register_reserved = mmio::register_reserved<uint32_t, 16>;

        static inline constexpr auto IA32_APIC_BASE_MSR_ENABLE = 0x800;

        struct apic_registers
        {
            register_reserved r0[2];
            register_rw id;
            register_rdonly version;
            register_reserved r1[4];
            register_rw task_priority;
            register_rdonly arbitration_priority;
            register_rdonly processor_priority;
            register_wronly eoi;
            register_rdonly remote_read;
            register_rw local_destination;
            register_rw destination_format;
            register_rw siv;
            register_rdonly in_service[8];
            register_rdonly trigger_mode[8];
            register_rdonly irq_register[8];
            register_rdonly error_status;
            register_reserved r2[6];
            register_rw cmci;
            register_rw interrupt_command[2];
            register_rw lvt_timer;
            register_rw lvt_thermal;
            register_rw lvt_performance_monitoring_counters;
            register_rw lvt_lint[2];
            register_rw lvt_error;
            register_rw inital_timer_count;
            register_rdonly current_timer_count;
            register_reserved r3[4];
            register_rw timer_divide;
            register_reserved r4;
        };

    private:
        apic_registers* reg_start = nullptr;
        uint64_t ticks_per_ms;

    public:
        /// \brief Check for the presense of an LAPIC on the current core
        ///
        static bool check_apic();

        /// \brief Enables the LAPIC
        /// Enables the APIC by setting the APIC base, disabling old PIC and setting the spurious interrupt vector
        void enable();

        /// \brief Disables the LAPIC
        ///
        void disable();

        /// \brief Calibrates the Advanced Programmable Interrupt Timer
        /// \return The ticks per millisecond
        ///
        /// This computes the APIT ticks per millisecond, and stores it into a cached value
        /// A call to this function should occur before any call to set_tick()
        uint64_t calibrate();

        /// \brief Sets the amount of ticks before a timer interrupt
        /// \param irq The irq vector to use
        /// \param ms The number of milliseconds per timer interrupt
        /// The APIT ticks are computed by using the value from calibrate() * \p ms. Interrupts are generated at the IRQ
        /// defined by \p irq, which requires that the core's IDT contain an entry for the specified vector
        void set_tick(uint8_t irq, std::size_t ms);

        /// \brief Sets the base address for the APIC
        /// \param addr The new base address
        void set_apic_base(uintptr_t addr);

        /// \brief Obtains the APIC base address
        ///
        uintptr_t get_apic_base();

        /// \brief Obtains a reference to the MMIO registers for the LAPIC
        //
        constexpr apic_registers& mmio_register() { return *reg_start; }

        /// \brief Sets the "end of interrupt" field in the LAPIC
        //
        inline void end() { mmio_register().eoi.write(0); }
    };
} // namespace apic

#endif

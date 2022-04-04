#ifndef __ARCH_X86_APIC_APIC_H__
#define __ARCH_X86_APIC_APIC_H__
#include <asm/asm_cpp.h>
#include <cstddef.h>
#include <cstdint.h>
#include <mmio/register.h>

namespace apic
{
    class local_apic
    {
    public:
        using register_rw = register_rw<uint32_t, 16>;
        using register_rdonly = register_rdonly<uint32_t, 16>;
        using register_wronly = register_wronly<uint32_t, 16>;
        using register_reserved = register_reserved<uint32_t, 16>;

        static inline constexpr auto IA32_APIC_BASE_MSR = 0x1B;
        static inline constexpr auto IA32_APIC_BASE_MSR_BSP = 0x100;
        static inline constexpr auto IA32_APIC_BASE_MSR_ENABLE = 0x800;

        struct apic_registers
        {
            register_reserved r0[2];
            register_rw id;
            register_rdonly version;
            register_reserved r1[3];
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
            register_rw timer;
            register_rw thermal;
            register_rw performance_monitoring_counters;
            register_rw lint[2];
            register_rw lvt_error;
            register_rw inital_timer_count;
            register_rdonly current_timer_count;
            register_reserved r3[4];
            register_rw timer_divide;
            register_reserved r4;
        };

    private:
        apic_registers* reg_start = nullptr;

    public:
        static bool check_apic();

        void enable();
        void disable();

        void set_apic_base(uintptr_t apic);
        uintptr_t get_apic_base();

        constexpr apic_registers& mmio_register() { return *reg_start; }
    };
} // namespace apic

#endif

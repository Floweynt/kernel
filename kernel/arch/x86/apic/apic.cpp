#include "apic.h"
/*
namespace apic
{
    bool check_apic()
    {
        uint32_t eax, edx;
        cpuid(1, &eax, nullptr, nullptr, &edx);
        return edx & CPUID_FEAT_EDX_APIC;
    }

    void cpu_set_apic_base(uintptr_t apic)
    {
        uint32_t edx = 0;
        uint32_t eax = (apic & 0xfffff0000) | IA32_APIC_BASE_MSR_ENABLE;

    #ifdef __PHYSICAL_MEMORY_EXTENSION__
       edx = (apic >> 32) & 0x0f;
    #endif

       wrmsr(IA32_APIC_BASE_MSR, IA32_APIC_BASE_MSR_ENABLE);
    }

    uintptr_t cpu_get_apic_base()
    {
        uint32_t eax, edx;
        rdmsr(IA32_APIC_BASE_MSR, &eax, nullptr, nullptr, &edx);
    }

    void enable_apic() {
        cpu_set_apic_base(cpu_get_apic_base());
        write_reg(0xF0, ReadRegister(0xF0) | 0x100);
    }
}
*/

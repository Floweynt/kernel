#include "apic.h"
#include <asm/asm_cpp.h>
#include <cstdint>

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
        // apply memory-map offset
        reg_start = (apic_registers*)(apic + 0xffff800000000000);
        wrmsr(IA32_APIC_BASE_MSR, (apic & 0xfffff0000) | IA32_APIC_BASE_MSR_ENABLE, (apic >> 32) & 0x0f);
    }

    uintptr_t local_apic::get_apic_base() { return 0x0ffffff000 & rdmsr(IA32_APIC_BASE_MSR); }

    void local_apic::enable()
    {
        outb(0xa1, 0xff);
        outb(0x21, 0xff);

        set_apic_base(get_apic_base());
        reg_start->siv.write(reg_start->siv.read() | 0x100);
    }
} // namespace apic

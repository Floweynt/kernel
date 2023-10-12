#include <asm/asm_cpp.h>
#include <gdt/gdt.h>
#include <klog/klog.h>
#include <misc/cast.h>
#include <user/syscall/syscall_setup.h>

extern "C" void _syscall_entry();

namespace user::syscall
{
    void init()
    {
        // enable syscall/sysret
        wrmsr(msr::IA32_EFER, rdmsr(msr::IA32_EFER) | 1);
        wrmsr(msr::IA32_LSTAR, as_uptr(_syscall_entry));
        wrmsr(msr::IA32_STAR, gdt::KERNEL_CS << 32);
    }
} // namespace user::syscall

#include <cstdint>

inline void cpuid(int code, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d)
{
    uint32_t tmp = 0;
    if (a == nullptr)
        a = &tmp;
    if (b == nullptr)
        b = &tmp;
    if (c == nullptr)
        c = &tmp;
    if (d == nullptr)
        d = &tmp;
    asm volatile("cpuid" : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d) : "a"(code));
}

#define READ_CR(CR)                                                                                                         \
    inline uint64_t read_cr##CR()                                                                                           \
    {                                                                                                                       \
        uint64_t val;                                                                                                       \
        asm volatile("mov %%cr" #CR "%0" : "=r"(val));                                                                      \
        return val;                                                                                                         \
    }
#define WRITE_CR(CR)                                                                                                        \
    inline void write_cr##CR(uint64_t val) { asm volatile("mov %0, %%cr" #CR : : "r"(val)); }

READ_CR(0)
// READ_CR(1)
READ_CR(2)
READ_CR(3)
READ_CR(4)
// READ_CR(5)
// READ_CR(6)
// READ_CR(7)
// READ_CR(8)

WRITE_CR(0)
// READ_CR(1)
WRITE_CR(2)
WRITE_CR(3)
WRITE_CR(4)
// READ_CR(5)
// READ_CR(6)
// READ_CR(7)
// READ_CR(8)

inline void invlpg(void* m) { asm volatile("invlpg (%0)" : : "b"(m) : "memory"); }

inline void disable_interrupt() { __asm__ __volatile__("cli"); }
inline void enable_interrupt() { __asm__ __volatile__("sti"); }

namespace msr
{
    inline constexpr uint64_t IA32_EFER = 0xc0000080;
    inline constexpr uint64_t IA32_FS_BASE = 0xc0000100;
    inline constexpr uint64_t IA32_GS_BASE = 0xc0000101;
    inline constexpr uint64_t IA32_KERNEL_GS_BASE = 0xc0000102;
} // namespace msr

inline void wrmsr(uint64_t msr, uint64_t value)
{
    uint32_t low = value & 0xFFFFFFFF;
    uint32_t high = value >> 32;
    asm volatile("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}

inline void wrmsr(uint64_t msr, uint32_t a, uint32_t d) { __asm__ __volatile__("wrmsr" : : "c"(msr), "a"(a), "d"(d)); }

inline uint64_t rdmsr(uint64_t msr)
{
    uint32_t low, high;
    asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

inline void setstack(uint64_t sp) { asm volatile("mov %0, %%rsp" : : "r"(sp)); }

inline void ljmp(void* addr) { asm volatile("push %0\nret" : : "r"(addr)); }

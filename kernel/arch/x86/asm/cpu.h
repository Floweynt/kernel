#ifndef __ARCH_X86_ASM_CPU_H__
#define __ARCH_X86_ASM_CPU_H__
#include <cstdint>

inline void cpuid(int code, uint64_t* a, uint64_t* d)
{
    asm volatile ( "cpuid" : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx" );
}

#define READ_CR(CR) \
    inline uint64_t read_cr ## CR () \
    { \
        uint64_t val; \
        asm volatile ("mov %%cr" #CR, %0" : "=r"(val) ); \
        return val; \
    }

READ_CR(0)
// READ_CR(1)
READ_CR(2)
READ_CR(3)
READ_CR(4)
//READ_CR(5)
//READ_CR(6)
//READ_CR(7)
//READ_CR(8)

inline void invlpg(void* m)
{
    asm volatile("invlpg (%0)" : : "b"(m) : "memory");
}

inline void wrmsr(uint64_t msr, uint64_t value)
{
	uint32_t low = value & 0xFFFFFFFF;
	uint32_t high = value >> 32;
	asm volatile("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}

static inline uint64_t rdmsr(uint64_t msr)
{
	uint32_t low, high;
	asm volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
	return ((uint64_t)high << 32) | low;
}

#endif

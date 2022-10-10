#include <cstdint>

/// \brief `cpuid` instruction wrapper
/// \param code The value passed in `%eax` for `cpuid`
/// \param[out] a The value to store the `%eax` register
/// \param[out] b The value to store the `%ebx` register
/// \param[out] c The value to store the `%ecx` register
/// \param[out] d The value to store the `%edx` register
/// Wrapper for the <a href="https://en.wikipedia.org/wiki/CPUID">cpuid</a> instruction. The value specified in \p code will
/// be written into the `cpuid` instruction, which is then executed and loaded into a, b, c, d parameters from `%e<param>x`.
/// If the argument passed in is null, the corresponding register's value will be thrown away.
inline void cpuid(std::uint32_t code, std::uint32_t* a, std::uint32_t* b, std::uint32_t* c, std::uint32_t* d)
{
    std::uint32_t tmp = 0;
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

/// \brief `cpuid` extended features instruction wrapper
/// \param feature The value passed in `%ecx` for `cpuid`
/// \param[out] b The value to store the `%ebx` register
/// \param[out] c The value to store the `%ecx` register
/// \param[out] d The value to store the `%edx` register
/// \return The largest value available for this cpuid leaf
///
/// Wrapper for the <a href="https://en.wikipedia.org/wiki/CPUID#EAX=7,_ECX=0:_Extended_Features">cpuid extend features</a>
/// leaf. The value specified in \p feature will be written into the `%ecx` register for the, along with setting `%eax` to 7
/// The `cpuid` instruction is then executed and loaded into b, c, d parameters from `%e<param>x`.
/// The arguments to this instruction wrapper must not be null.
inline std::uint32_t cpuid_ext(std::uint32_t feature, std::uint32_t* b, std::uint32_t* c, std::uint32_t* d)
{
    std::uint32_t max;
    asm volatile("cpuid" : "=a"(max), "=b"(*b), "=c"(*c), "=d"(*d) : "a"(7), "c"(feature));
    return max;
}

#define READ_CR(CR)                                                                                                         \
    inline std::uint64_t read_cr##CR()                                                                                      \
    {                                                                                                                       \
        std::uint64_t val;                                                                                                  \
        asm volatile("mov %%cr" #CR "%0" : "=r"(val));                                                                      \
        return val;                                                                                                         \
    }
#define WRITE_CR(CR)                                                                                                        \
    inline void write_cr##CR(std::uint64_t val) { asm volatile("mov %0, %%cr" #CR : : "r"(val)); }

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

/// \brief Wrapper for the `invlpg` instruction
/// \param m The address passed into invlpg
inline void invlpg(void* m) { asm volatile("invlpg (%0)" : : "b"(m) : "memory"); }

/// \brief Wrapper for the `cli` instruction
///
inline void disable_interrupt() { asm volatile("cli"); }

/// \brief Wrapper for the `sti` instruction
///
inline void enable_interrupt() { asm volatile("sti"); }

namespace msr
{
    inline constexpr std::uint64_t IA32_APIC_BASE = 0x1b;
    inline constexpr std::uint64_t IA32_EFER = 0xc0000080;
    inline constexpr std::uint64_t IA32_FS_BASE = 0xc0000100;
    inline constexpr std::uint64_t IA32_GS_BASE = 0xc0000101;
    inline constexpr std::uint64_t IA32_KERNEL_GS_BASE = 0xc0000102;
    inline constexpr std::uint64_t IA32_PAT = 0x277;
} // namespace msr

/// \brief Wrapper for the `wrmsr` instruction
/// \param msr The msr to write to
/// \param value The value to write
inline void wrmsr(std::uint64_t msr, std::uint64_t value)
{
    std::uint32_t low = value & 0xFFFFFFFF;
    std::uint32_t high = value >> 32;
    asm volatile("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}

/// \brief Wrapper for the `wrmsr` instruction, split into 2 parts
/// \param msr The msr to write to
/// \param a The a register passed to `wrmsr`
/// \param d The d register passed to `wrmsr`
inline void wrmsr(std::uint64_t msr, std::uint32_t a, std::uint32_t d)
{
    asm volatile("wrmsr" : : "c"(msr), "a"(a), "d"(d));
}

/// \brief Wrapper for the `wrmsr` instruction
/// \param msr The msr to read from
/// \return The value contained in the msr specified in \p msr
inline std::uint64_t rdmsr(std::uint64_t msr)
{
    std::uint32_t low, high;
    asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((std::uint64_t)high << 32) | low;
}

/// \brief Sets the stack pointer `%rsp`
/// \param sp The new stack pointer address
inline void setstack(std::uintptr_t sp) { asm volatile("mov %0, %%rsp" : : "r"(sp)); }

/// \brief Preforms a long jump
/// \param addr The address to jump to
[[noreturn]] inline void ljmp(void* addr)
{
    asm volatile("push %0\nret" : : "r"(addr));
    __builtin_unreachable();
}

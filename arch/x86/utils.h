#ifndef __ARCH_X86_UTILS_H__
#define __ARCH_X86_UTILS_H__
#include <cstdint>
#include <type_traits>

#define GET_OFFSET_FROM_START(ptr) (reinterpret_cast<char*>(ptr) - 0xFFFFFFFF80000000)

#define GET_PHYSICAL_POINTER(ptr) GET_PHYSICAL_POINTER_ADDR(ptr, get_bootloader_packet()->loaded_address)
#define GET_PHYSICAL_POINTER_ADDR(ptr, addr)                                                                                \
    (reinterpret_cast<std::decay_t<decltype(ptr)>>(GET_OFFSET_FROM_START(ptr) + addr))
#define TO_CANONICAL(ptr) (reinterpret_cast<std::decay_t<decltype(ptr)>>(0xFFFF000000000000 | (uint64_t)ptr))

template <uint8_t n>
uint64_t align(uint64_t t)
{
    return (t + (1 << n) - 1) & (~(1 << n));
}

long long ceil_logbase2(long long x)
{
    static const unsigned long long t[6] = {0xFFFFFFFF00000000ull, 0x00000000FFFF0000ull, 0x000000000000FF00ull,
                                            0x00000000000000F0ull, 0x000000000000000Cull, 0x0000000000000002ull};

    int y = (((x & (x - 1)) == 0) ? 0 : 1);
    int j = 32;
    int i;

    for (i = 0; i < 6; i++)
    {
        int k = (((x & t[i]) == 0) ? 0 : j);
        y += k;
        x >>= k;
        j >>= 1;
    }
    return y;
}

constexpr auto KERNEL_SPACE_CS = 0x8;
constexpr auto KERNEL_SPACE_DS = 0x10;

#endif

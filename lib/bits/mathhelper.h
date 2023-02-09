#ifndef __NOSTDLIB_BITS_MATHHELPER_H__
#define __NOSTDLIB_BITS_MATHHELPER_H__
#include "../include/cstdint"

namespace std
{
    template <typename A, typename B>
    constexpr A div_roundup(A a, B b)
    {
        return (a + b - 1) / b;
    }

    template <uint8_t n>
    uint64_t align(uint64_t t)
    {
        return (t + (1 << n) - 1) & (~(1 << n));
    }

    constexpr unsigned int ceil_logbase2(long long x)
    {
        constexpr unsigned long long t[6] = {0xFFFFFFFF00000000ull, 0x00000000FFFF0000ull, 0x000000000000FF00ull,
                                             0x00000000000000F0ull, 0x000000000000000Cull, 0x0000000000000002ull};

        int y = (((x & (x - 1)) == 0) ? 0 : 1);
        int j = 32;
        int k = 0;

        k = (((x & t[0]) == 0) ? 0 : j);
        y += k;
        x >>= k;
        j >>= 1;

        k = (((x & t[1]) == 0) ? 0 : j);
        y += k;
        x >>= k;
        j >>= 1;

        k = (((x & t[2]) == 0) ? 0 : j);
        y += k;
        x >>= k;
        j >>= 1;

        k = (((x & t[3]) == 0) ? 0 : j);
        y += k;
        x >>= k;
        j >>= 1;

        k = (((x & t[4]) == 0) ? 0 : j);
        y += k;
        x >>= k;
        j >>= 1;

        k = (((x & t[5]) == 0) ? 0 : j);
        y += k;
        x >>= k;
        j >>= 1;

        return y;
    }
} // namespace std

#endif

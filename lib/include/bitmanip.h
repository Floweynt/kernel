#ifndef __UTILS_H__
#define __UTILS_H__
#include "../bits/mathhelper.h"
#include "cstddef"
#include "cstdint" // IWYU pragma: keep
#include "cstring" // IWYU pragma: keep

namespace std
{
    template <uint8_t Bit>
    constexpr void set_bit(uint64_t& flag, bool val = true)
    {
        flag &= ~(1UL << Bit);
        flag |= uint64_t{val} << Bit;
    }

    constexpr void set_bit(uint64_t& flag, bool val, uint8_t bit)
    {
        flag &= ~(1UL << bit);
        flag |= uint64_t{static_cast<uint64_t>(val)} << bit;
    }

    template <uint8_t Bit>
    constexpr auto get_bit(uint64_t flag) -> bool
    {
        return flag & (1UL << Bit);
    }

    constexpr auto get_bit(uint64_t flag, uint8_t bit) -> bool { return (flag & (1UL << bit)) != 0U; }

    template <uint8_t Start, uint8_t End>
    constexpr auto get_bits(uint64_t flag) -> uint64_t
    {
        return flag & (((1UL << (uint64_t)(End - Start + 1)) - 1) << Start);
    }

    constexpr auto get_bits(uint64_t flag, uint8_t start, uint8_t end) -> uint64_t
    {
        return flag & (((1UL << (uint64_t)(end - start + 1)) - 1) << start);
    }

    constexpr void set_bits(uint64_t& flag, uint64_t to_set, uint8_t start, uint8_t end)
    {
        flag &= ~(((1UL << (uint64_t)(end - start + 1)) - 1) << start);
        flag |= (((1UL << (uint64_t)(end - start + 1)) - 1) << start) & to_set;
    }

    template <typename T>
    auto make_bitarray(std::size_t bits) -> T*
    {
        std::size_t count = div_roundup(bits, sizeof(T) * 8);
        T* buf = new T[count];
        memset(buf, 0xff, bits / 8);
        if (bits % 8)
        {
            ((uint8_t*)buf)[bits / 8] = (1ul << (bits % 8)) - 1;
        }
        return buf;
    }

    template <typename T>
    auto make_bitarray(std::size_t bits, void* buffer) -> T*
    {
        T* buf = (T*)buffer;
        memset(buf, 0xff, bits / 8);
        if (bits % 8)
        {
            ((uint8_t*)buf)[bits / 8] = (1ul << (bits % 8)) - 1;
        }
        return (T*)buffer;
    }
} // namespace std

#endif

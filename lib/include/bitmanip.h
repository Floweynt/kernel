#ifndef __UTILS_H__
#define __UTILS_H__
#include "cstdint"
#include "cstddef"
#include "cstring"
#include "utility"

namespace std
{
    template <uint8_t i>
    constexpr inline void set_bit(uint64_t& v, bool s)
    {
        v &= ~(1ULL << i);
        v |= uint64_t{s} << i;
    }

    constexpr inline void set_bit(uint64_t& v, bool s, uint8_t i)
    {
        v &= ~(1ULL << i);
        v |= uint64_t{s} << i;
    }

    template <uint8_t i>
    constexpr inline bool get_bit(uint64_t v)
    {
        return v & (1ULL << i);
    }

    constexpr inline bool get_bit(uint64_t v, uint8_t i) { return v & (1ULL << i); }

    template <uint8_t start, uint8_t end>
    constexpr inline uint64_t get_bits(uint64_t v)
    {
        return v & (((1ULL << (uint64_t)(end - start + 1)) - 1) << start);
    }

    constexpr inline uint64_t get_bits(uint64_t v, uint8_t start, uint8_t end)
    {
        return v & (((1ULL << (uint64_t)(end - start + 1)) - 1) << start);
    }

    constexpr inline void set_bits(uint64_t& v, uint64_t to_set, uint8_t start, uint8_t end)
    {
        v &= ~(((1ULL << (uint64_t)(end - start + 1)) - 1) << start);
        v |= (((1ULL << (uint64_t)(end - start + 1)) - 1) << start) & to_set;
    }

    template<typename T>
    T* make_bitarray(std::size_t bits)
    {
        std::size_t count = div_roundup(bits, sizeof(T) * 8);
        T* buf = new T[count];
        memset(buf, 0xff, bits / 8);
        if(bits % 8)
            ((uint8_t*) buf)[bits / 8] = (1ul << (bits % 8)) - 1;
        return buf;
    }

    template<typename T>
    T* make_bitarray(std::size_t bits, void* b)
    {
        T* buf = (T*) b;
        memset(buf, 0xff, bits / 8);
        if(bits % 8)
            ((uint8_t*) buf)[bits / 8] = (1ul << (bits % 8)) - 1;
        return (T*) b;
    }

} // namespace std

#endif

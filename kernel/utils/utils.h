#include <cstdint>
#ifndef __UTILS_H__
#define __UTILS_H__

template <uint8_t i>
constexpr inline void set_bit(uint64_t &v, bool s)
{
	v &= ~(1ULL << i);
	v |= uint64_t{s} << i;
}

template <uint8_t i>
constexpr inline bool get_bit(uint64_t v)
{
	return v & (1ULL << i);
}

template<uint8_t start, uint8_t end>
constexpr inline uint64_t get_bits(uint64_t v)
{
    return v & (((1ULL << (uint64_t)(end - start + 1)) - 1) << start);
}

template<uint8_t start, uint8_t end>
constexpr inline void set_bits(uint64_t& v, uint64_t to_set)
{
	v &= ~(((1ULL << (uint64_t)(end - start + 1)) - 1) << start);
	v |= (((1ULL << (uint64_t)(end - start + 1)) - 1) << start) & to_set;
}
#endif

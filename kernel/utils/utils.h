#include <cstdint>

template <uint8_t i>
void set_bit(uint64_t &v, bool s)
{
	v &= ~(1ULL << i);
	v |= uint64_t{s} << i;
}

template <uint8_t i>
bool get_bit(uint64_t v)
{
	return v & (1ULL << i);
}

template<uint8_t start, uint8_t end>
uint64_t get_bits(uint64_t v)
{
    return v & (((1ULL << (uint64_t)(end - start + 1)) - 1) << start);
}

template<uint8_t start, uint8_t end>
void set_bits(uint64_t& v, uint64_t to_set)
{
	v &= ~(((1ULL << (uint64_t)(end - start + 1)) - 1) << start);
	v |= (((1ULL << (uint64_t)(end - start + 1)) - 1) << start) & to_set;
}

#define offsetof(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))

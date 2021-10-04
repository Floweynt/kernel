#ifndef __ARCH_X86_UTILS_H__
#define __ARCH_X86_UTILS_H__
#include <type_traits>
#include "kinit/kinit.h"

#define PLACE_AT_START __attribute__((section(".place.in.front")))
#define GET_OFFSET_FROM_START(ptr) \
    (reinterpret_cast<char*>(ptr) - 0xFFFFFFFF80000000)

#define GET_PHYSICAL_POINTER(ptr) GET_PHYSICAL_POINTER_ADDR(ptr, get_bootloader_packet()->loaded_address)
#define GET_PHYSICAL_POINTER_ADDR(ptr, addr) \
    (reinterpret_cast<std::decay_t<decltype(ptr)>>(GET_OFFSET_FROM_START(ptr) + addr))
#define TO_CANONICAL(ptr) \
    (reinterpret_cast<std::decay_t<decltype(ptr)>>(0xFFFF000000000000 | (uint64_t)ptr))

template <uint8_t n>
uint64_t align(uint64_t t)
{
    return (t + (1 << n) - 1) & (~(1 << n));
}

constexpr auto KERNEL_SPACE_CS = 0x8;
constexpr auto KERNEL_SPACE_DS = 0x10;

#endif

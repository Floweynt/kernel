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

inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

inline void outw(uint16_t port, uint16_t val)
{
    asm volatile ( "outw %0, %1" : : "a"(val), "Nd"(port) );
}

inline void outl(uint16_t port, uint32_t val)
{
    asm volatile ( "outl %0, %1" : : "a"(val), "Nd"(port) );
}

inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    asm volatile ( "inw %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

inline uint32_t inl(uint16_t port)
{
    uint32_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}
inline void io_wait(void)
{
    outb(0x80, 0);
}

#endif

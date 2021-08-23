#ifndef __ARCH_X86_UTILS_H__
#define __ARCH_X86_UTILS_H__
#include <type_traits>
#include "kinit.h"

#define GET_OFFSET_FROM_START(ptr) \
    (reinterpret_cast<char*>(ptr) - 0xFFFFFFFF80000000)
#define GET_PHYSICAL_POINTER(ptr) GET_PHYSICAL_POINTER_ADDR(get_bootloader_packet()->loaded_address)
#define GET_PHYSICAL_POINTER_ADDR(ptr, addr) \
    (reinterpret_cast<std::decay_t<decltype(ptr)>>(GET_OFFSET_FROM_START(ptr) + addr))
#endif

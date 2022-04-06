#ifndef __ARCH_INTERFACE_PROPERTIES_H__
#define __ARCH_INTERFACE_PROPERTIES_H__

#include <cstddef>
#include <cstdint>

template <size_t n, size_t... params>
class constant_properties
{
    template <size_t curr, size_t... rest>
    constexpr void initalize()
    {
        data[curr / 8] |= (1 << (curr % 8));
        if constexpr (sizeof...(rest) != 0)
            initalize<rest...>();
    }

    static constexpr size_t get_size()
    {
        if constexpr (n % 8 == 0)
            return n / 8;
        return n / 8 + 1;
    }

    uint8_t data[get_size()];

public:
    constexpr constant_properties()
    {
        if constexpr (sizeof...(params) != 0)
            initalize<params...>();
    }

    constexpr bool test(size_t prop) { return data[prop / 8] & (1 << (prop % 8)); }
};

enum arch_lib_support
{
    HAS_MEMCPY = 0,
    SIZE
};

#include <arch/x86/prop.h>

#endif
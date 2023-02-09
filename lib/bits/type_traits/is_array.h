#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_ARRAY_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_ARRAY_H__
#include "utils.h"
#include "../../include/cstddef"

namespace std
{
#if __has_builtin(__is_array) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(array)
#else
    template <typename T>
    struct is_array : false_type
    {
    };
    template <typename T>
    struct is_array<T[]> : true_type
    {
    };
    template <typename T, size_t N>
    struct is_array<T[N]> : true_type
    {
    };
    template <typename T>
    inline constexpr bool is_array_v = is_array<T>::value;
#endif
} // namespace std

#endif

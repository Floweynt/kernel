#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_BOUNDED_ARRAY_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_BOUNDED_ARRAY_H__
#include "utils.h"
#include "../../include/cstddef"

namespace std
{
#if __has_builtin(__is_bounded_array) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(bounded_array)
#else
    template <class T>
    struct is_bounded_array : false_type
    {
    };

    template <class T, size_t N>
    struct is_bounded_array<T[N]> : true_type
    {
    };

    template<typename T>
    inline constexpr bool is_bounded_array_v = is_bounded_array<T>::value;
#endif
} // namespace std

#endif


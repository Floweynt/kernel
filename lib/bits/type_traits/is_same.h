#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_SAME_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_SAME_H__
#include "utils.h"

namespace std
{
#if __has_builtin(__is_same) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait2(same) 
#else
    template <typename T, typename U>
    struct is_same : false_type
    {
    };

    template <typename T>
    struct is_same<T, T> : true_type
    {
    };

    template <typename T, typename U>
    inline constexpr bool is_same_v = is_same<T, U>::value;

#endif
} // namespace std

#endif

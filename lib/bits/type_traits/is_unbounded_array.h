#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_UNBOUNDED_ARRAY_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_UNBOUNDED_ARRAY_H__
#include "utils.h"
#include "../../include/cstddef"

namespace std
{
#if __has_builtin(__is_unbounded_array) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(unbounded_array)
#else
    template <typename T>
    struct is_unbounded_array : false_type
    {
    };

    template <typename T>
    struct is_unbounded_array<T[]> : true_type
    {
    };

    template<typename T>
    inline constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;

#endif
} // namespace std

#endif



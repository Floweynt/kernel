#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_ARITHMETIC_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_ARITHMETIC_H__
#include "is_floating_point.h"
#include "is_integral.h"
#include "utils.h"

namespace std
{
#if __has_builtin(__is_arithmetic) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(arithmetic);
#else
    template <class T>
    struct is_arithmetic : bool_constant<is_integral_v<T> || is_floating_point_v<T>>
    {
    };
#endif
} // namespace std

#endif


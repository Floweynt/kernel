#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_FLOATING_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_FLOATING_H__
#include "utils.h"
#include "cv.h"
#include "is_same.h"

namespace std
{
#if __has_builtin(__is_floating_point) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(floating_point)
#else
    template <typename T>
    struct is_floating_point : matches_any<is_same, remove_cv_t<T>, float, double, long double>
    {
    };
    template <typename T>
    inline constexpr bool is_floating_point_v = is_floating_point<T>::value;
#endif
}

#endif

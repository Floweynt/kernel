#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_INTEGRAL_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_INTEGRAL_H__
#include "utils.h"
#include "is_same.h"
#include "cv.h"

namespace std
{
#if __has_builtin(__is_integral) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
   __nostd_type_traits_intrinsic_trait(integral) 
#else
    template <typename T>
    struct is_integral
        : matches_any<is_same, remove_cv_t<T>, bool, char, char32_t, wchar_t, short, int, long, long long>
    {
    };
    template <typename T>
    inline constexpr bool is_integral_v = is_integral<T>::value;

#endif
}

#endif

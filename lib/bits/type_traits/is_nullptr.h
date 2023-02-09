#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_NULLPTR_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_NULLPTR_H__
#include "cv.h"
#include "is_same.h"
#include "../../include/cstddef"

namespace std
{
#if __has_builtin(__is_nullptr) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    template <typename T>
    struct is_null_pointer : bool_constant<__is_nullptr(T)>
    {
    };

    template <typename T>
    inline constexpr bool is_null_pointer_v = __is_nullptr(T);
#else
    template <typename T>
    struct is_null_pointer : is_same<nullptr_t, remove_cv_t<T>>
    {
    };
    template <typename T>
    inline constexpr bool is_null_pointer_v = is_same_v<T, nullptr_t>;
#endif
}

#endif

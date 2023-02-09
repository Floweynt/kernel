#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_FUNDAMENTAL_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_FUNDAMENTAL_H__
#include "is_arithmetic.h"
#include "is_nullptr.h"
#include "is_void.h"
#include "utils.h"

namespace std
{
#if __has_builtin(__is_fundamental) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(fundamental);
#else
    template <class T>
    struct is_fundamental : bool_constant<is_arithmetic_v || is_void_v<T> || is_null_pointer_v<T>>
    {
    };
#endif
} // namespace std

#endif

#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_SCALAR_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_SCALAR_H__
#include "is_arithmetic.h"
#include "is_enum.h"
#include "is_member_pointer.h"
#include "is_nullptr.h"
#include "pointer.h"
#include "utils.h"

namespace std
{
#if __has_builtin(__is_scalar) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(scalar);
#else
    template <typename T>
    struct is_scalar : bool_constant<is_arithmetic_v<T> || is_enum_v<T> || is_pointer_v<T> ||
                                     is_member_pointer_v<T> || is_null_pointer_v<T>>
    {
    };
#endif
} // namespace std

#endif

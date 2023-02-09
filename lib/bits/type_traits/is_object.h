#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_OBJECT_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_OBJECT_H__

#include "is_array.h"
#include "is_class.h"
#include "is_scalar.h"
#include "is_union.h"
#include "utils.h"

namespace std
{
#if __has_builtin(__is_object) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(object);
#else
    template <typename T>
    struct is_object : integral_constant<bool, is_scalar_v<T> || is_array_v<T> || is_union_v<T> || is_class_v<T>>
    {
    };
#endif
} // namespace std

#endif

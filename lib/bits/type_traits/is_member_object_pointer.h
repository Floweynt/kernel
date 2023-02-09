#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_MEMBER_OBJECT_POINTER_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_MEMBER_OBJECT_POINTER_H__
#include "is_function.h"
#include "utils.h"
#include "is_member_function_pointer.h"
#include "is_member_pointer.h"

namespace std
{
#if __has_builtin(__is_member_object_pointer) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(member_object_pointer)
#else
    template <class T>
    struct is_member_object_pointer
        : integral_constant<bool, is_member_pointer_v<T> && !is_member_function_pointer_v<T>>
    {
    };
#endif
}

#endif


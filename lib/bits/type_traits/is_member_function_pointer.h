#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_MEMBER_FUNCTION_POINTER_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_MEMBER_FUNCTION_POINTER_H__
#include "utils.h"
#include "is_function.h"

namespace std
{
#if __has_builtin(__is_member_function_pointer) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(member_function_pointer)
#else
    template <class T>
    struct is_member_function_pointer_helper : false_type
    {
    };

    template <class T, class U>
    struct is_member_function_pointer_helper<T U::*> : is_function<T>
    {
    };

    template <class T>
    struct is_member_function_pointer : is_member_function_pointer_helper<remove_cv_t<T>>
    {
    };
#endif
}

#endif


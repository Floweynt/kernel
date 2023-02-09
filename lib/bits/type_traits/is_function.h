#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_FUNCTION_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_FUNCTION_H__
#include "cv.h"
#include "ref.h"
#include "utils.h"

namespace std
{
#if __has_builtin(__is_function) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(function)
#else
    template <class T>
    struct is_function : std::bool_constant<!std::is_const<const T>::value && !std::is_reference<T>::value>
    {
    };
#endif
} // namespace std

#endif

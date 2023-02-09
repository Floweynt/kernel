#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_VOID_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_VOID_H__
#include "utils.h"
#include "cv.h"

namespace std
{
#if __has_builtin(__is_void) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(void);
#else
    template<typename T> struct is_void : is_same<void, std::remove_cv_t<T>> {};

    template<typename T>
    inline constexpr bool is_void_v = is_void<T>::value;
#endif
}
#endif

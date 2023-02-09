#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_CLASS_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_CLASS_H__
#include "is_union.h"

namespace std
{
#if __has_builtin(__is_class) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(class)
#else
    namespace detail
    {
        template <typename T>
        bool_constant<!is_union<T>::value> test(int T::*);
        template <typename T>
        false_type test(...);
    } // namespace detail

    template <typename T>
    struct is_class : decltype(detail::test<T>(nullptr)) {};
    template <typename T>
    inline constexpr bool is_class_v = is_class<T>::value;
#endif
}

#endif

#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_UNSIGNED_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_UNSIGNED_H__

#include "utils.h"
#include "is_arithmetic.h"

namespace std
{
#if __has_builtin(__is_unsigned) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(unsigned);
#else
    namespace detail
    {
        template <typename T, bool = is_arithmetic<T>::value>
        struct is_unsigned : integral_constant<bool, T(0) < T(-1)>
        {
        };

        template <typename T>
        struct is_unsigned<T, false> : false_type
        {
        };
    } // namespace detail

    template <typename T>
    struct is_unsigned : detail::is_unsigned<T>::type
    {
    };
#endif
} // namespace std

#endif

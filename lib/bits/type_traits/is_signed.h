#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_SIGNED_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_SIGNED_H__

#include "utils.h"
#include "is_arithmetic.h"

namespace std
{
#if __has_builtin(__is_signed) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(signed);
#else
    namespace detail
    {
        template <typename T, bool = is_arithmetic<T>::value>
        struct is_signed : integral_constant<bool, T(-1) < T(0)>
        {
        };

        template <typename T>
        struct is_signed<T, false> : false_type
        {
        };
    } // namespace detail

    template <typename T>
    struct is_signed : detail::is_signed<T>::type
    {
    };
#endif
} // namespace std

#endif

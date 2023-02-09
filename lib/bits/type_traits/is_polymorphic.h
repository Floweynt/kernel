#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_POLYMORPHIC_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_POLYMORPHIC_H__
#include "utils.h"

namespace std
{
#if __has_builtin(__is_polymorphic) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(polymorphic);
#else
    namespace detail
    {
        template <class T>
        true_type detect_is_polymorphic(decltype(dynamic_cast<const volatile void*>(static_cast<T*>(nullptr))));
        template <class T>
        false_type detect_is_polymorphic(...);

    } // namespace detail

    template <class T>
    struct is_polymorphic : decltype(detail::detect_is_polymorphic<T>(nullptr))
    {
    };
#endif
} // namespace std

#endif

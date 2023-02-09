#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_CONVERTIBLE_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_CONVERTIBLE_H__
#include "is_union.h"
#include "is_void.h"
#include "../declval.h"

namespace std
{
    // TODO: implement
#if __has_builtin(__is_convertible)
    __nostd_type_traits_intrinsic_trait2(convertible);
#else
    static_assert(false, "boo");
#endif

    template <typename T, typename U>
    struct is_nothrow_convertible : std::bool_constant<std::is_void_v<T> && std::is_void_v<U>>
    {
    };

    template <typename T, typename U>
    requires requires
    {
        static_cast<U (*)()>(nullptr);
        {
            std::declval<void (&)(U) noexcept>()(std::declval<T>())
        }
        noexcept;
    }
    struct is_nothrow_convertible<T, U> : std::true_type
    {
    };

    template<typename T, typename U>
    inline constexpr bool is_nothrow_convertible_v = is_nothrow_convertible<T,  U>::value;

} // namespace std

#endif


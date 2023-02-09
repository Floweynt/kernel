#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_ASSIGNABLE_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_ASSIGNABLE_H__
#include "is_enum.h"
#include "utils.h"
#include "../declval.h"
#include "../../include/cstddef"
namespace std
{
#if __has_builtin(__is_assignable)
    template <typename T, typename U>
    struct is_assignable : bool_constant<__is_assignable(T, U)>
    {
    };
    template <typename T, typename U>
    inline constexpr bool is_assignable_v = __is_assignable(T, U);
#else
    static_assert(false, "required builtin does not exist for std::is_assignable<T>");
#endif

    template <typename T, typename U>
#if __has_builtin(__is_trivially_assignable)
    struct is_trivially_assignable : bool_constant<__is_assignable(T, U) && __is_trivially_assignable(T, U)>
    {
    };
    template <typename T, typename U>
    inline constexpr bool is_trivially_assignable_v = __is_assignable(T, U) && __is_trivially_assignable(T, U);
#else
    static_assert(false, "required builtin does not exist for std::is_trivially_assignable<T>");
#endif

#if __has_builtin(__is_nothrow_assignable) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    template <typename T, typename U>
    struct is_nothrow_assignable : bool_constant<__is_nothrow_assignable(T, U)>
    {
    };
    template <typename T, typename U>
    inline constexpr bool is_nothrow_assignable_v = __is_nothrow_assignable(T, U);
#else
    namespace detail
    {
        template <typename T, typename U>
        bool is_nothrow_assignable_helper()
        {
            if constexpr (is_assignable<T, U>::value)
                return nowthrow(declval<T>() = declval<U>());
            else
                return false;
        }
    } // namespace detail

    template <typename T, typename U>
    struct is_nothrow_assignable : bool_constant<detail::is_nothrow_assignable_helper<T, U>()>
    {
    };

    template <typename T, typename U>
    inline constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<T, U>::value;
#endif

} // namespace std

#endif


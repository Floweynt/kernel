#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_CONSTRUCTIBLE_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_CONSTRUCTIBLE_H__
#include "is_enum.h"
#include "utils.h"
#include "../../include/cstddef"
namespace std
{
#if __has_builtin(__is_constructible)
    template <typename T, typename... Args>
    struct is_constructible : bool_constant<__is_constructible(T, Args...)>
    {
    };
    template <typename T, typename... Args>
    inline constexpr bool is_constructible_v = __is_constructible(T, Args...);
#else
    static_assert(false, "required builtin does not exist for std::is_constructible<T>");
#endif

    template <typename T, typename... Args>
#if __has_builtin(__is_trivially_constructible)
    struct is_trivially_constructible
        : bool_constant<__is_constructible(T, Args...) && __is_trivially_constructible(T, Args...)>
    {
    };
    template <typename T, typename... Args>
    inline constexpr bool is_trivially_constructible_v = __is_constructible(T, Args...) &&
                                                         __is_trivially_constructible(T, Args...);
#else
    static_assert(false, "required builtin does not exist for std::is_trivially_constructible<T>");
#endif

#if __has_builtin(__is_nothrow_constructible) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    template <typename T, typename... Args>
    struct is_nothrow_constructible : bool_constant<__is_nothrow_constructible(T, Args...)>
    {
    };
    template <typename T, typename... Args>
    inline constexpr bool is_nothrow_constructible_v = __is_nothrow_constructible(T, Args...);
#else
    namespace detail
    {
        template <typename T, typename... Args>
        bool is_nothrow_constructible_helper()
        {
            if constexpr (is_constructible<T, Args...>::value)
                return nowthrow(T(declval<Args>()...));
            else
                return false;
        }
    } // namespace detail

    template <typename T, typename... Args>
    struct is_nothrow_constructible : bool_constant<detail::is_nothrow_constructible_helper<T, Args...>()> {};

    template <typename T, typename... Args>
    inline constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<T, Args...>::value;
#endif

} // namespace std

#endif


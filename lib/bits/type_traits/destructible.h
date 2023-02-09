#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_DESTRUCTIBLE_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_DESTRUCTIBLE_H__
#include "is_enum.h"
#include "utils.h"
#include "../../include/cstddef"
namespace std
{
#if __has_builtin(__is_destructible) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    template <typename T>
    struct is_destructible : bool_constant<__is_destructible(T)>
    {
    };
    template <typename T>
    inline constexpr bool is_destructible_v = __is_destructible(T);
#else

    namespace detail
    {
        template <typename T>
        struct is_destructible_helper
        {
            template <typename T1, typename = decltype(declval<T1&>().~T1())>
            static true_type test(int);

            template <typename>
            static false_type test(...);

            using type = decltype(test<T>(0));
        };
    } // namespace detail

    template <typename T>
    struct is_destructible : detail::is_destructible_helper<T>::type
    {
    };

    template <typename T>
    inline constexpr bool is_destructible_v = is_destructible<T>::value;
#endif

#if __has_builtin(__is_trivially_destructible)
    template <typename T>
    struct is_trivially_destructible : bool_constant<__is_trivially_destructible(T)>
    {
    };
    template <typename T, typename... Args>
    inline constexpr bool is_trivially_destructible_v = is_destructible_v<T>&& __is_trivially_destructible(T);
#else
    static_assert(false, "required builtin does not exist for std::is_trivially_destructible<T>");
#endif

#if __has_builtin(__is_nothrow_destructible) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    template <typename T>
    struct is_nothrow_destructible : bool_constant<__is_nothrow_destructible(T)>
    {
    };
    template <typename T>
    inline constexpr bool is_nothrow_destructible_v = __is_nothrow_destructible(T);
#else
    namespace detail
    {
        template <typename T>
        constexpr bool is_nothrow_destructible_helper()
        {
            if constexpr (is_destructible_helper<T>::type::value)
                return noexcept(declval<T&>().~T1());
            else
                return false;
        }
    } // namespace detail

    template <typename T, typename... Args>
    struct is_nothrow_destructible : bool_constant<detail::is_nothrow_destructible_helper<T, Args...>()>
    {
    };

    template <typename T, typename... Args>
    inline constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<T, Args...>::value;
#endif
} // namespace std

#endif


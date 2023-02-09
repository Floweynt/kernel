#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_SWAP_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_SWAP_H__
#include "utils.h"

namespace std
{
    namespace detail
    {
        template <typename T, typename U>
        struct is_swappable_helper
        {
            template <typename T1, typename U1, typename = decltype(swap(declval<T1&>(), declval<U1&>()))>
            static true_type test(int);

            template <typename>
            static false_type test(...);

            using type = decltype(test<T, U>(0));
        };

        template <typename T, typename U>
        constexpr bool is_nothrow_swappable_helper()
        {
            if constexpr (is_swappable_helper<T, U>::type::value)
                return noexcept(swap(declval<T&>(), declval<U&>()))&& noexcept(swap(declval<U&>(), declval<T&>()));
            else
                return false;
        }
    } // namespace detail

    template <typename T>
    using is_swappable = detail::is_swappable_helper<T, T>;
    template <typename T, typename U>
    using is_swappable_with = conjunction<detail::is_swappable_helper<T, U>, detail::is_swappable_helper<U, T>>;
    template <typename T>
    using is_nothrow_swappable = bool_constant<detail::is_nothrow_swappable_helper<T, T>()>;
    template <typename T, typename U>
    using is_nothrow_swappable_with = bool_constant<detail::is_nothrow_swappable_helper<T, U>()>;

    template <typename T>
    inline constexpr bool is_swappable_v = is_swappable<T>::value;
    template <typename T, typename U>
    inline constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;
    template <typename T>
    inline constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;
    template <typename T, typename U>
    inline constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<T, U>::value;
} // namespace std

#endif

#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_SCOPED_ENUM_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_SCOPED_ENUM_H__
#include "utils.h"
#include "is_enum.h"
#include "../../include/cstddef"
namespace std
{
#if __has_builtin(__is_scoped_enum) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(scoped_enum)
#else
    namespace detail
    {
        namespace
        {
            template <typename T>
            auto test_sizable(int) -> decltype(sizeof(T), true_type{});
            template <typename>
            auto test_sizable(...) -> false_type;

            template <typename T>
            auto test_nonconvertible_to_int(int)
                -> decltype(static_cast<false_type (*)(int)>(nullptr)(declval<T>()));
            template <typename>
            auto test_nonconvertible_to_int(...) -> true_type;

            template <typename T>
            constexpr bool is_scoped_enum_impl =
                conjunction_v<decltype(test_sizable<T>(0)), decltype(test_nonconvertible_to_int<T>(0))>;
        }
    }

    template <typename>
    struct is_scoped_enum : false_type
    {
    };

    template <typename E> requires is_enum_v<E>
    struct is_scoped_enum<E> : bool_constant<detail::is_scoped_enum_impl<E>>
    {
    };

    template <typename E>
    inline constexpr bool is_scoped_enum_v = is_scoped_enum<E>::value;
#endif
} // namespace std

#endif


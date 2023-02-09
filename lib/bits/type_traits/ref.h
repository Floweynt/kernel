#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_REF_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_REF_H__

#include "utils.h"

namespace std
{
    template <typename T>
    struct remove_reference
    {
        using type = T;
    };
    template <typename T>
    struct remove_reference< T&>
    {
        using type = T;
    };
    template <typename T>
    struct remove_reference< T&&>
    {
        using type = T;
    };

    template <typename T>
    using remove_reference_t = typename remove_reference<T>::type;

    namespace detail
    {
        template <typename T>
        auto try_add_lvalue_reference(int) -> type_identity<T&>;
        template <typename T>
        auto try_add_lvalue_reference(...) -> type_identity<T>;
        template <typename T>
        auto try_add_rvalue_reference(int) -> type_identity<T&&>;
        template <typename T>
        auto try_add_rvalue_reference(...) -> type_identity<T>;
    } // namespace detail

    template <typename T>
    struct add_lvalue_reference : decltype(detail::try_add_lvalue_reference<T>(0))
    {
    };
    template <typename T>
    using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

    template <typename T>
    struct add_rvalue_reference : decltype(detail::try_add_rvalue_reference<T>(0))
    {
    };
    template <typename T>
    using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

#if __has_builtin(__is_lvalue_reference) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(lvalue_reference)
#else
    template<typename T>
    struct is_lvalue_reference : false_type {};

    template<typename T>
    struct is_lvalue_reference<T&> : true_type {};

    template<typename T>
    inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;
#endif

#if __has_builtin(__is_rvalue_reference) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(rvalue_reference)
#else
    template<typename T>
    struct is_rvalue_reference : false_type {};

    template<typename T>
    struct is_rvalue_reference<T&&> : true_type {};

    template<typename T>
    inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;
#endif

#if __has_builtin(__is_reference) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(reference)
#else
    template<typename T>
    struct is_reference : false_type {};

    template<typename T>
    struct is_reference<T&> : true_type {};

    template<typename T>
    struct is_reference<T&&> : true_type {};

    template<typename T>
    inline constexpr bool is_reference_v = is_reference<T>::value;

#endif
} // namespace std

#endif

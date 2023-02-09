#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_REMOVE_CV_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_REMOVE_CV_H__

#include "utils.h"

namespace std
{
    template <typename T>
    struct remove_cv
    {
        using type = T;
    };
    template <typename T>
    struct remove_cv<const T>
    {
        using type = T;
    };
    template <typename T>
    struct remove_cv<volatile T>
    {
        using type = T;
    };
    template <typename T>
    struct remove_cv<const volatile T>
    {
        using type = T;
    };
    template <typename T>
    using remove_cv_t = typename remove_cv<T>::type;

    template <typename T>
    struct remove_const
    {
        using type = T;
    };
    template <typename T>
    struct remove_const<const T>
    {
        using type = T;
    };
    template <typename T>
    using remove_const_t = typename remove_const<T>::type;

    template <typename T>
    struct remove_volatile
    {
        using type = T;
    };
    template <typename T>
    struct remove_volatile<volatile T>
    {
        using type = T;
    };
    template <typename T>
    using remove_volatile_t = typename remove_volatile<T>::type;

    template <typename T>
    struct add_cv
    {
        using type = const volatile T;
    };
    template <typename T>
    struct add_const
    {
        using type = const T;
    };
    template <typename T>
    struct add_volatile
    {
        using type = volatile T;
    };
    template <typename T>
    using add_cv_t = typename add_cv<T>::type;
    template <typename T>
    using add_const_t = typename add_const<T>::type;
    template <typename T>
    using add_volatile_t = typename add_volatile<T>::type;


#if __has_builtin(__is_const) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(const);
#else
    template<typename T>
    struct is_const : false_type {};

    template<typename T>
    struct is_const<const T> : true_type {};

    template<typename T>
    inline constexpr bool is_const_v = is_const<T>::value;
#endif

#if __has_builtin(__is_volatile) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(volatile);
#else
    template<typename T>
    struct is_volatile : false_type {};

    template<typename T>
    struct is_volatile<volatile T> : true_type {};

    template<typename T>
    inline constexpr bool is_volatile_v = is_volatile<T>::value;
#endif
} // namespace std

#endif

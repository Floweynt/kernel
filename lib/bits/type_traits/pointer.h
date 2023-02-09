#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_POINTER_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_POINTER_H__

#include "ref.h"
#include "utils.h"

namespace std
{
    template <typename T>
    struct remove_pointer
    {
        using type = T;
    };
    template <typename T>
    struct remove_pointer<T*>
    {
        using type = T;
    };

    template <typename T>
    struct remove_pointer<T* const>
    {
        using type = T;
    };

    template <typename T>
    struct remove_pointer<T* volatile>
    {
        using type = T;
    };

    template <typename T>
    struct remove_pointer<T* const volatile>
    {
        using type = T;
    };

    template <typename T>
    using remove_pointer_t = typename remove_pointer<T>::type;

    namespace detail
    {
        template <class T>
        auto try_add_pointer(int) -> type_identity<remove_reference_t<T>*>;
        template <class T>
        auto try_add_pointer(...) -> type_identity<T>;
    } // namespace detail

    template <class T>
    struct add_pointer : decltype(detail::try_add_pointer<T>(0))
    {
    };

    template <typename T>
    using add_pointer_t = typename add_pointer<T>::type;

#if __has_builtin(__is_pointer) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(pointer)
#else
    template <class T>
    struct is_pointer : std::false_type
    {
    };

    template <class T>
    struct is_pointer<T*> : std::true_type
    {
    };

    template <class T>
    struct is_pointer<T* const> : std::true_type
    {
    };

    template <class T>
    struct is_pointer<T* volatile> : std::true_type
    {
    };

    template <class T>
    struct is_pointer<T* const volatile> : std::true_type
    {
    };

    template <typename T>
    inline constexpr bool is_pointer_v = is_pointer<T>::value;
#endif
} // namespace std

#endif


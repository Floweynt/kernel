#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_COMPILER_SPECIFIC_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_COMPILER_SPECIFIC_H__
#include "utils.h"

namespace std
{
#if __has_builtin(__is_trivial)
    __nostd_type_traits_intrinsic_trait(trivial);
#else
    static_assert(false, "required builtin does not exist for std::is_trivial<T>");
#endif

#if __has_builtin(__is_trivially_copyable)
    __nostd_type_traits_intrinsic_trait(trivially_copyable);
#else
    static_assert(false, "required builtin does not exist for std::is_trivially_copyable<T>");
#endif

#if __has_builtin(__is_standard_layout)
    __nostd_type_traits_intrinsic_trait(standard_layout);
#else
    static_assert(false, "required builtin does not exist for std::is_standard_layout<T>");
#endif

#if __has_builtin(__has_unique_object_representations)
    template <typename T>
    struct has_unique_object_representations : bool_constant<__has_unique_object_representations(T)>
    {
    };

    template <typename T>
    inline constexpr bool has_unique_object_representations_v = __has_unique_object_representations(T);
#else
    static_assert(false, "required builtin does not exist for std::has_unique_object_representations<T>");
#endif

#if __has_builtin(__is_empty)
    __nostd_type_traits_intrinsic_trait(empty);
#else
    static_assert(false, "required builtin does not exist for std::is_empty<T>");
#endif

#if __has_builtin(__is_abstract)
    __nostd_type_traits_intrinsic_trait(abstract);
#else
    static_assert(false, "required builtin does not exist for std::is_abstract<T>");
#endif

#if __has_builtin(__is_final)
    __nostd_type_traits_intrinsic_trait(final);
#else
    static_assert(false, "required builtin does not exist for std::is_final<T>");
#endif

#if __has_builtin(__is_aggregate)
    __nostd_type_traits_intrinsic_trait(aggregate);
#else
    static_assert(false, "required builtin does not exist for std::is_aggregate<T>");
#endif

#if __has_builtin(__has_virtual_destructor)
    template <typename T>
    struct has_virtual_destructor : bool_constant<__has_virtual_destructor(T)> {};
    template<typename T>
    inline constexpr bool has_virtual_destructor_v = __has_virtual_destructor(T);
#else
    static_assert(false, "required builtin does not exist for std::has_virtual_destructor<T>");
#endif

/*#if __has_builtin(__is_layout_compatible)
    __nostd_type_traits_intrinsic_trait(layout_compatible)
#else
    static_assert(false, "required builtin does not exist for std::is_layout_compatible<T>");
#endif*/
} // namespace std

#endif


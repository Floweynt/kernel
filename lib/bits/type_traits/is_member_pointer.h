#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_MEMBER_POINTER_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_MEMBER_POINTER_H__
#include "utils.h"
#include "cv.h"
namespace std
{
#if __has_builtin(__is_member_pointer) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait(member_pointer);
#else
    namespace detail
    {
        template <typename T>
        struct is_member_pointer_helper : false_type
        {
        };

        template <typename T, class U>
        struct is_member_pointer_helper<T U::*> : true_type
        {
        };
    } // namespace detail
    template <typename T>
    struct is_member_pointer : detail::is_member_pointer_helper< remove_cv_t<T>>
    {
    };

    template<typename T>
    inline constexpr bool is_member_pointer_v = std::is_member_pointer<T>::value;
#endif
} // namespace std

#endif

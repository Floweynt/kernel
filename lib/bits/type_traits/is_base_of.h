#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_IS_BASE_OF_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_IS_BASE_OF_H__
#include "utils.h"

namespace std
{
#if __has_builtin(__is_base_of) && !defined(__NOSTD_DISABLE_TYPE_TRAITS_BUILTIN)
    __nostd_type_traits_intrinsic_trait2(base_of)
#else
    namespace details
    {
        template <typename B>
        true_type test_pre_ptr_convertible(const volatile B*);
        template <typename>
        false_type test_pre_ptr_convertible(const volatile void*);

        template <typename, typename>
        auto test_pre_is_base_of(...) -> true_type;
        template <typename B, typename D>
        auto test_pre_is_base_of(int) -> decltype(test_pre_ptr_convertible<B>(static_cast<D*>(nullptr)));
    } // namespace details

    template <typename Base, typename Derived>
    struct is_base_of
        : integral_constant<
              bool, is_class<Base>::value &&
                        is_class<Derived>::value&& decltype(details::test_pre_is_base_of<Base, Derived>(0))::value>
    {
    };
#endif
} // namespace std

#endif

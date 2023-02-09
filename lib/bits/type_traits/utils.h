#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_UTILS_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_UTILS_H__

namespace std
{
    template <typename T, T v>
    struct integral_constant
    {
        static constexpr T value = v;
        using value_type = T;
        using type = integral_constant<T, v>;

        constexpr operator value_type() const noexcept { return value; }
        constexpr value_type operator()() const noexcept { return value; }
    };

    template <bool B>
    using bool_constant = integral_constant<bool, B>;

    using true_type = integral_constant<bool, true>;
    using false_type = integral_constant<bool, false>;

    // effectively preforms an AND operation on all T::value in Ts
    template <typename... Ts>
    struct conjunction : bool_constant<(Ts::value && ...)>
    {
    };
    template <typename... Ts>
    inline constexpr bool conjunction_v = conjunction<Ts...>::value;

    // effectively preforms an OR operation on all T::value in all Ts
    template <typename... Ts>
    struct disjunction : bool_constant<(Ts::value && ...)>
    {
    };
    template <typename... Ts>
    inline constexpr bool disjunction_v = disjunction<Ts...>::value;

    template <typename T>
    struct negation : bool_constant<!bool(T::value)>
    {
    };
    template <typename T>
    inline constexpr bool negation_v = negation<T>::value;

    template <template <typename, typename> typename P, typename T, typename... Ts>
    struct matches_any : disjunction<P<T, Ts>...>
    {
    };

    template <template <typename, typename> typename P, typename T, typename... Ts>
    inline constexpr bool matches_any_v = matches_any<P, T, Ts...>::value;

    template <typename T>
    struct type_identity
    {
        using type = T;
    };

    template <bool B, typename T, typename F>
    struct conditional
    {
        using type = T;
    };

    template <typename T, typename F>
    struct conditional<false, T, F>
    {
        using type = F;
    };

    template <bool B, typename T, typename F>
    using conditional_t = typename conditional<B, T, F>::type;

    template <typename...>
    using void_t = void;

    template <bool B, typename T = void>
    struct enable_if
    {
    };

    template <typename T>
    struct enable_if<true, T>
    {
        typedef T type;
    };

    template <bool B, typename T = void>
    using enable_if_t = typename enable_if<B, T>::type;

} // namespace std

#define __nostd_type_traits_intrinsic_trait(trait)                                                                          \
    template <typename T>                                                                                                   \
    struct is_##trait : bool_constant<__is_##trait(T)>                                                                      \
    {                                                                                                                       \
    };                                                                                                                      \
                                                                                                                            \
    template <typename T>                                                                                                   \
    inline constexpr bool is_##trait##_v = __is_##trait(T);

#define __nostd_type_traits_intrinsic_trait2(trait)                                                                         \
    template <typename T, typename U>                                                                                       \
    struct is_##trait : bool_constant<__is_##trait(T, U)>                                                                   \
    {                                                                                                                       \
    };                                                                                                                      \
                                                                                                                            \
    template <typename T, typename U>                                                                                       \
    inline constexpr bool is_##trait##_v = __is_##trait(T, U);
#endif

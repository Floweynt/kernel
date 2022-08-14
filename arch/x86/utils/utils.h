#ifndef __ARCH_X86_UTILS_H__
#define __ARCH_X86_UTILS_H__

namespace utils
{
    template <typename... Ts>
    class packed_tuple
    {
        template <typename... _Ts>
        struct [[gnu::packed]] tuple_base;

        template <typename T>
        struct [[gnu::packed]] tuple_base<T>
        {
            T value;
            constexpr explicit tuple_base(T&& arg) : value(static_cast<T&&>(arg)) {}
        };

        template <typename T, typename... _Ts>
        struct [[gnu::packed]] tuple_base<T, _Ts...>
        {
            T value;
            tuple_base<_Ts...> b;

            constexpr explicit tuple_base(T&& arg, _Ts&&... args)
                : value(static_cast<T&&>(arg)), b(static_cast<_Ts&&>(args)...)
            {
            }
        };
        tuple_base<Ts...> base;

    public:
        constexpr packed_tuple(Ts&&... args) : base(static_cast<Ts&&>(args)...) {}

        static_assert((sizeof(Ts) + ...) == sizeof(tuple_base<Ts...>));
    };

    template <typename... Ts>
    packed_tuple(Ts&&... args) -> packed_tuple<Ts...>;
} // namespace utils

#endif

#pragma once

#include <algorithm>
#include <cstddef>
namespace std
{
    template <std::size_t N>
    struct string_literal
    {
        constexpr string_literal(const char (&str)[N]) { std::copy_n(str, N, value); }

        constexpr string_literal(const char (&str)[N - 1], char ch)
        {
            std::copy_n(str, N - 2, value);
            value[N - 2] = ch;
            value[N - 1] = 0;
        }

        constexpr string_literal() { std::fill_n(value, N, 0); }

        [[nodiscard]] constexpr auto begin() const -> const char* { return value; }
        constexpr auto end() const { return begin() + N; }

        constexpr auto begin() -> char* { return value; }
        constexpr auto end() { return begin() + N; }

        constexpr auto operator[](std::size_t index) const -> char { return value[index]; }
        constexpr auto operator[](std::size_t index) -> char& { return value[index]; }

        char value[N]{};
        inline static constexpr auto size = N - 1;
    };

    template <typename T>
    struct is_strlit : std::false_type
    {
    };
    template <std::size_t N>
    struct is_strlit<string_literal<N>> : std::true_type
    {
    };

    template <typename T>
    concept strlit = is_strlit<std::decay_t<T>>::value;

} // namespace std

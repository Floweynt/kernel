#include "algorithm"
#include "concepts"
#include "cstdint"
#include "type_traits"

namespace ctcfg
{
    namespace detail
    {
        template <typename T>
        auto _declval() -> std::add_rvalue_reference_t<T>
        {
            __builtin_unreachable();
        }

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

            char value[N];
            inline static constexpr auto size = N;
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

        struct error
        {
        };

        template <typename... T>
        struct type_list
        {
            using first = error;
        };

        template <typename T, typename... Ts>
        struct type_list<T, Ts...>
        {
            using first = T;
        };

        template <typename... T1, typename... T2>
        auto operator+(type_list<T1...> /*unused*/, type_list<T2...> /*unused*/) -> type_list<T1..., T2...>
        {
            return _declval<type_list<T1..., T2...>>();
        }

        template <typename T, T v>
        struct comptime_value
        {
            using type = T;
            inline static constexpr auto value = v;
        };

        template <typename T>
        concept config_entry_concept = requires(T a) {
            {
                T::name
            } -> strlit<>;
            std::is_class_v<typename T::type>;
        };

        template <string_literal v1, string_literal v2>
        constexpr auto is_eq() -> bool
        {
            bool flag = decltype(v1)::size == decltype(v2)::size;
            auto min_len = decltype(v1)::size > decltype(v2)::size ? decltype(v1)::size : decltype(v2)::size;
            for (std::size_t i = 0; i < min_len && flag; i++)
            {
                flag = v1.value[i] == v2.value[i];
            }
            return flag;
        }

        template <string_literal v, config_entry_concept Opt>
        using check = std::conditional_t<is_eq<v, Opt::name>(), type_list<Opt>, type_list<>>;

        template <typename T, typename... Ts>
        struct extract_first
        {
            using type = T;
        };

        template <string_literal v, config_entry_concept... Opts>
        using search_result_t = typename decltype((type_list<>{} + ... + check<v, Opts>{}))::first;
    } // namespace detail

    template <typename T, T v>
    using simple_data_holder = detail::comptime_value<T, v>;

    template <detail::string_literal n, typename T>
    struct config_entry
    {
        inline static constexpr auto name = n;
        using type = T;
    };

    template <detail::config_entry_concept... Opts>
    struct config_holder
    {
        template <detail::string_literal str>
            requires(!std::is_same_v<detail::error, typename detail::search_result_t<str, Opts...>>)
        using get = typename detail::search_result_t<str, Opts...>::type;

        template <detail::string_literal str>
        inline static constexpr auto get_val = get<str>::value;

        template <detail::string_literal str>
        inline static constexpr const char* get_str = (const char*)get<str>::value;

        template <detail::string_literal str>
        inline static constexpr auto exists = !std::is_same_v<detail::error, typename detail::search_result_t<str, Opts...>>;
    };

    template <detail::string_literal n, typename T, T v>
    using simple_entry = config_entry<n, simple_data_holder<T, v>>;

    template <std::size_t N>
    struct string_literal
    {
        constexpr string_literal(const char (&str)[N]) { std::copy_n(str, N, value); }

        constexpr auto size() const { return N; }
        constexpr operator const char*() const { return value; }
        char value[N];
    };
#define __MAKE_ENTRY(_n, t)                                                                                                                          \
    template <detail::string_literal n, t v>                                                                                                         \
    struct _n : simple_entry<n, decltype(v), v>                                                                                                      \
    {                                                                                                                                                \
    };
    __MAKE_ENTRY(bool_entry, bool);
    __MAKE_ENTRY(int_entry, int);
    __MAKE_ENTRY(size_entry, std::size_t);
    __MAKE_ENTRY(string_entry, string_literal);
    __MAKE_ENTRY(address_entry, std::uintptr_t);
#undef __MAKE_ENTRY
} // namespace ctcfg

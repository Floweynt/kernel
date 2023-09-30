#pragma once

#include <cstddef>
#include <cstdint>
#include <misc/cast.h>

auto ptr_off(auto* ptr, std::size_t off) { return cast_ptr<decltype(ptr)>(cast_ptr<std::uint8_t>(ptr) + off); }

namespace detail
{
    template <typename T>
    class tag_ptr
    {
        std::uintptr_t ptr;
        inline static constexpr std::uintptr_t TAG_MASK = 0b111;
        inline static constexpr std::uintptr_t PTR_MASK = ~TAG_MASK;

    public:
        inline tag_ptr(T* ptr = nullptr, std::uint8_t tag = 0) : ptr(as_uptr(ptr) | (tag & TAG_MASK)) {}

        [[nodiscard]] constexpr auto get_tag() const -> std::uint8_t { return ptr & TAG_MASK; }
        constexpr void set_tag(std::uint8_t tag) { ptr = (ptr & PTR_MASK) | (tag & TAG_MASK); }

        [[nodiscard]] inline auto get_ptr() const -> T* { return as_ptr(ptr & PTR_MASK); }
        inline void set_ptr(T* new_ptr) { ptr = (ptr & TAG_MASK) | as_uptr(new_ptr); }
    };

    template<typename T>
    struct check_tag_ptr 
    {
        static_assert(sizeof(tag_ptr<T>) == sizeof(T*));
        using type = tag_ptr<T>;
    };
} // namespace detail

template<typename T>
using tag_ptr = typename detail::check_tag_ptr<T>::type;

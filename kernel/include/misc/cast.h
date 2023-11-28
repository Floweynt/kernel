#pragma once
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
namespace detail
{
    struct _castable_to_any_pointer
    {
        void* ptr;

        constexpr _castable_to_any_pointer(void* ptr) : ptr(ptr) {}

        template <typename T>
        operator T*()
        {
            return reinterpret_cast<T*>(ptr);
        }
    };
} // namespace detail

template <typename T>
inline auto as_ptr(std::uintptr_t ptr) -> T*
{
    return reinterpret_cast<T*>(ptr);
}

inline auto as_ptr(std::uintptr_t ptr) { return detail::_castable_to_any_pointer(reinterpret_cast<void*>(ptr)); }

inline auto as_pv(std::uintptr_t ptr) -> void* { return as_ptr<void>(ptr); }

inline auto as_uptr(auto* ptr) { return reinterpret_cast<std::uintptr_t>(ptr); }

template <typename T>
inline auto as_ptr(void* ptr) -> T*
{
    return reinterpret_cast<T*>(ptr);
}
inline auto as_ptr(void* ptr) { return detail::_castable_to_any_pointer(ptr); }

template <typename T>
inline auto as_vptr(T* ptr) -> auto
{
    if constexpr (std::is_const_v<T>)
    {
        return reinterpret_cast<const void*>(ptr);
    }
    else
    {
        return reinterpret_cast<void*>(ptr);
    }
}

inline auto as_vptr(std::uintptr_t ptr) -> void* { return reinterpret_cast<void*>(ptr); }

template <typename T>
inline auto cast_ptr(auto* ptr) -> T*
{
    return reinterpret_cast<T*>(ptr);
}

inline auto cast_ptr(auto* ptr) { return detail::_castable_to_any_pointer(as_vptr(ptr)); }

template <typename T, std::size_t N>
inline auto decay_arr(T (&arr)[N])
{
    return (T*)arr;
}

template <typename T>
inline auto decay_arr(T (&arr)[])
{
    return (T*)arr;
}

template<std::integral T>
inline auto as_signed(T t)
{
    return std::make_signed_t<T>(t);
}

template<std::integral T>
inline auto as_unsigned(T t)
{
    return std::make_unsigned_t<T>(t);
}

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

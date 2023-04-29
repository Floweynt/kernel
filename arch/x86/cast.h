#pragma once
#include <cstdint>

// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
namespace detail
{
    struct _castable_to_any_pointer
    {
        void* ptr;

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
inline auto as_vptr(auto* ptr) -> void*
{
    return reinterpret_cast<void*>(ptr);
}

inline auto as_vptr(std::uintptr_t ptr) -> void* { return reinterpret_cast<void*>(ptr); }

template <typename T>
inline auto cast_ptr(auto* ptr) -> T*
{
    return reinterpret_cast<T*>(ptr);
}

inline auto cast_ptr(auto* ptr)
{
    return detail::_castable_to_any_pointer(as_vptr(ptr));
}
// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

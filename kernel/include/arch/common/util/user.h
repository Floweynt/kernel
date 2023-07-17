#pragma once

template<typename T>
class user_pointer 
{
    T* ptr;
public:
    user_pointer() = default;
    user_pointer(T* ptr) : ptr(ptr) {}

    constexpr auto get() -> T* { return ptr; }
};

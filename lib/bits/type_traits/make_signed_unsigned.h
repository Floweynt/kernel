#pragma once

namespace std
{
#if __has_builtin(__make_signed)
template <typename T>
struct make_signed {
  using type  = __make_signed(T);
};

template<typename T>
using make_signed_t = make_signed<T>::type;

#else
    static_assert(false, "not impl w/o builtin");
#endif

#if __has_builtin(__make_unsigned)
template <typename T>
struct make_unsigned {
  using type  = __make_unsigned(T);
};

template<typename T>
using make_unsigned_t = make_unsigned<T>::type;

#else
    static_assert(false, "not impl w/o builtin");
#endif

} // namespace std

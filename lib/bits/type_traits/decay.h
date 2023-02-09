#ifndef __NOSTDLIB_BITS_TYPE_TRAITS_DECAY_H__
#define __NOSTDLIB_BITS_TYPE_TRAITS_DECAY_H__
#include "is_array.h"
#include "is_function.h"
#include "pointer.h"
#include "ref.h"
#include "remove_extents.h"

namespace std
{
    template <typename T>
    struct decay
    {
    private:
        using U = remove_reference_t<T>;
    public:
        using type = conditional_t<is_array_v<U>, remove_extent_t<U>*,
                                   conditional_t<is_function_v<U>, add_pointer_t<U>, remove_cv_t<U>>>;
    };

    template<typename T>
    using decay_t = typename decay<T>::type;
} // namespace std

#endif

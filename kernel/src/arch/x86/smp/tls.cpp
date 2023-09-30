// cSpell:ignore emutls
#include <atomic>
#include <bits/user_implement.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <klog/klog.h>
#include <new>
#include <smp/smp.h>
#include <sync/spinlock.h>

struct emutls_control
{
    std::size_t size;     /* size of the object in bytes */
    std::size_t align;    /* alignment of the object in bytes */
    std::uintptr_t index; /* data[index-1] is the object address */
    void* value;          /* null or non-zero initial value for the object */
};

namespace
{
    auto emutls_memalign_alloc(std::size_t align, std::size_t size) -> void* { return std::detail::aligned_malloc(size, align); }

    auto emutls_allocate_object(emutls_control* control) -> void*
    {
        std::size_t size = control->size;
        std::size_t align = control->align;
        if (align < sizeof(void*))
        {
            align = sizeof(void*);
        }
        if ((align & (align - 1)) != 0)
        {
            klog::panic("emutls: poorly aligned object");
        }

        void* base = emutls_memalign_alloc(align, size);

        if (control->value != nullptr)
        {
            std::memcpy(base, control->value, size);
        }
        else
        {
            std::memset(base, 0, size);
        }

        return base;
    }

    lock::spinlock emutls_mutex;
    std::size_t emutls_num_object = 0;

    auto emutls_get_index(emutls_control* control) -> std::uintptr_t
    {
        std::uintptr_t index = std::direct_atomic_load_n(&control->index, std::memory_order_acquire);
        if (!index)
        {
            lock::lock_guard g(emutls_mutex);
            index = control->index;
            if (!index)
            {
                index = ++emutls_num_object;
                std::direct_atomic_store_n(&control->index, index, std::memory_order::release);
            }
        }
        return index;
    }

    auto emutls_new_data_array_size(std::uintptr_t index) -> std::uintptr_t { return ((index + 1 + 15) & ~((std::uintptr_t)15)) - 1; }

    auto emutls_get_address_array(std::uintptr_t index) -> void**
    {
        auto& local = smp::core_local::get();
        auto* array = local.emutls_data;
        if (array == nullptr)
        {
            local.emutls_size = emutls_new_data_array_size(index);
            array = local.emutls_data = new void*[local.emutls_size];
        }
        else if (index >= local.emutls_size)
        {
            local.emutls_size = emutls_new_data_array_size(index);
            local.emutls_data = new void*[local.emutls_size];

            // copy over the old entries
            for (std::size_t i = 0; i < local.emutls_size; i++)
            {
                local.emutls_data[i] = array[i];
            }

            // free the new ones
            delete array;
        }
    
        return array;
    }
} // namespace

extern "C" auto __emutls_get_address(emutls_control* control) -> void*
{
    std::uintptr_t index = emutls_get_index(control);
    auto* array = emutls_get_address_array(index - 1);

    if (array[index - 1] == nullptr)
    {
        array[index - 1] = emutls_allocate_object(control);
    }

    return array[index - 1];
}


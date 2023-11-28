#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
namespace std
{
    template <typename T>
    class slot_vector
    {
        // we need uninitalized storage

        struct data_storage
        {
            uintptr_t data{};

            constexpr data_storage(T* ptr) { set_ptr(ptr); }
            constexpr data_storage(uint32_t prev, uint32_t next)
            {
                set_prev(prev);
                set_next(next);
            }

            constexpr ~data_storage()
            {
                if (data & 1)
                {
                    return;
                }

                // cursed typecasting
                delete (T*)((void*)data);
            }

            [[nodiscard]] constexpr auto get_ptr() const -> T* { return (T*)((void*)data); }
            [[nodiscard]] constexpr auto set_ptr(T* ptr) { data = (uintptr_t)ptr; }
            [[nodiscard]] constexpr auto is_free() -> bool { return (data & 1) != 0U; }

            [[nodiscard]] constexpr auto get_prev() const -> uint32_t { return (data >> 1) & 0x00000000'ffffffff; }
            [[nodiscard]] constexpr auto get_next() const -> uint32_t { return ((data >> 1) & 0xffffffff'00000000) >> 32; }

            constexpr void set_ptrs(uint32_t prev, uint32_t next) { data = ((((uint64_t)next << 32) | prev) << 1) | 1; }
            constexpr void set_prev(uint32_t val) { set_ptrs(val, get_next()); }
            constexpr void set_next(uint32_t val) { set_ptrs(get_prev(), val); }
        };

        std::vector<data_storage> storage;
        uintptr_t first_free_index;

        inline static constexpr uintptr_t NULL_INDEX = 0x7fff'ffff;

    public:
        constexpr slot_vector() : storage(), first_free_index(NULL_INDEX) { storage.reserve(256); }

        template <typename... Args>
        auto allocate(Args&&... args) -> size_t
        {
            size_t loc = 0;

            if (first_free_index == NULL_INDEX)
            {
                storage.emplace_back(new T(std::forward<Args>(args)...));
                loc = storage.size() - 1;
            }
            else
            {
                loc = first_free_index;
                auto next = first_free_index = storage[first_free_index].get_next();

                if (next != NULL_INDEX)
                {
                    storage[next].set_prev(NULL_INDEX);
                }

                storage[loc].set_ptr(new T(std::forward<Args>(args)...));
            }

            return loc;
        }

        template <typename... Args>
        auto allocate_at(size_t index, Args&&... args) -> bool
        {
            if (index >= storage.size())
            {
                storage.reserve(index + 1);
                while (index + 1 >= storage.size())
                {
                    storage.emplace_back(NULL_INDEX, first_free_index);
                    if (first_free_index != NULL_INDEX)
                    {
                        storage[first_free_index].set_prev(storage.size() - 1);
                    }
                    first_free_index = storage.size() - 1;
                }

                storage.emplace_back(new T(std::forward<Args>(args)...));
                // I HATE LISTS I HATE LISTS
            }
            else
            {
                if (!storage[index].is_free())
                {
                    return false;
                }

                auto prev = storage[index].get_prev();
                auto next = storage[index].get_next();

                if (prev != NULL_INDEX)
                {
                    storage[prev].set_next(next);
                }
                else
                {
                    first_free_index = next;
                }

                if (next != NULL_INDEX)
                {
                    storage[next].set_prev(prev);
                }

                storage[index].set_ptr(new T(std::forward<Args>(args)...));
            }

            return true;
        }

        auto free(size_t index)
        {
            if (storage[index].is_free())
            {
                return;
            }

            storage[index].~data_storage();

            new (&storage[index]) data_storage(NULL_INDEX, first_free_index);

            if (first_free_index != NULL_INDEX)
            {
                storage[first_free_index].set_prev(index);
            }

            first_free_index = index;
        }

        auto has(size_t index) -> bool
        {
            if (index >= storage.size())
            {
                return false;
            }

            return !storage[index].is_free();
        }

        auto operator[](size_t index) -> T& { return *storage[index].get_ptr(); }
        auto operator[](size_t index) const -> const T& { return *storage[index].get_ptr(); }

        /*
        void __debug()
        {
            return;
            std::cout << "__debug:\n";
            std::cout << std::hex << first_free_index << std::dec << '\n';
            for (const auto& i : storage)
            {
                std::cout << std::hex << i.data << std::dec << '\n';
            }
        }*/
    };
} // namespace std

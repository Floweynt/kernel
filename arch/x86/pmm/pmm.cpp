#include "pmm.h"

namespace pmm
{
    pmm_region::pmm_region(void* region_start, std::size_t len) : region_start((uint64_t*)region_start), len(len)
    {
        // TODO: cleanp detail
        if (region_start == nullptr)
        {
            region_start = data_start = nullptr;
            len = s = 0;
        }

        std::size_t metadata_size = len & ~63;
        std::size_t real_len = len - std::detail::div_roundup(metadata_size, 4096ul);
        std::memset(region_start, 0xff, len / 8);
        this->region_start[len / 64 + 1] = 1 << ((len % 64) - 1);
        s = real_len;
        data_start = this->region_start + std::detail::div_roundup(metadata_size, 4096ul) * 4096;
    }

    void* pmm_region::allocate()
    {
        if (region_start == nullptr)
            return nullptr;

        std::size_t metadata_size = len & ~63;
        for (std::size_t i = 0; i < metadata_size; i++)
        {
            lock::spinlock_guard guard { lock };
            if (region_start[i])
            {
                std::size_t bitindex = __builtin_clzll(region_start[i]);
                std::set_bit(region_start[i], false, 63 - bitindex);

                std::size_t offset = (64 * i + bitindex);
                return (void*)((uint64_t)data_start + 4096 * offset);
            }
        }
        return nullptr;
    }

    static pmm::pmm_region region[0x100];
    static std::size_t index = 0;

    void add_region(void* start, std::size_t len) { region[index++] = pmm_region(start, len); }

    void* pmm_allocate()
    {
        for (std::size_t i = 0; i < index; i++)
        {
            if (region[i].exists())
            {
                auto ptr = region[i].allocate();
                if (ptr != nullptr)
                    return ptr;
            }
        }

        return nullptr;
    }
} // namespace pmm

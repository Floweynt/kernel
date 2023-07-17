#include <debug/debug.h>
#include <config.h>
#include <cstdint>
#include <kinit/boot_resource.h>

namespace debug
{
    struct hdr
    {
        std::uint64_t header;
        std::uint64_t count;
    };

    struct entry
    {
        std::uint64_t start_addr;
        std::uint64_t len;
        std::uint64_t name;
    };

    auto sym_for(std::uint64_t address) -> debug::symbol
    {
        void* symtab = boot_resource::instance().modules().get_symbols();
        if (symtab != nullptr)
        {
            hdr* ptr = as_ptr(symtab);

            const std::size_t count = ptr->count;

            // binary search

            entry* entries = as_ptr(ptr + 1);

            std::size_t left = 0;
            std::size_t right = count;

            while (left + 1 != right)
            {
                const std::size_t mid = (left + right + 1) >> 1;

                if (entries[mid].start_addr > address)
                {
                    right = mid;
                }
                else
                {
                    left = mid;
                }
            }

            if (entries[left].start_addr + entries[left].len > address)
            {
                return debug::symbol{as_ptr<const char>(symtab) + entries[left].name, static_cast<std::uint32_t>(address - entries[left].start_addr)};
            }
            return debug::symbol{"unk", 0};
        }

        return debug::symbol{"unk", 0};
    }
} // namespace debug

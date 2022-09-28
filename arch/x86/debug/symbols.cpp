#include "debug.h"
#include <kinit/boot_resource.h>
#include <config.h>
#include <cstdint>

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

    debug::symbol sym_for(std::uint64_t address)
    {
        void* symtab = boot_resource::instance().modules().get_symbols();
        if (symtab != nullptr)
        {
            hdr* ptr = (hdr*) symtab;

            std::size_t n = ptr->count;

            // binary search

            entry* entries = (entry*)(ptr + 1);

            std::size_t l = 0;
            std::size_t r = n;

            while(l + 1 != r)
            {
                std::size_t mid = (l + r + 1) >> 1;

                if(entries[mid].start_addr > address)
                    r = mid;
                else
                    l = mid;
            }

            if(entries[l].start_addr + entries[l].len > address)
                return debug::symbol{
                    (const char*) symtab + entries[l].name,
                    static_cast<std::uint32_t>(address - entries[l].start_addr)
                };
            return debug::symbol{"unk", 0};
        }
        else
            return debug::symbol{"unk", 0};
    }
}

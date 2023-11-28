#include "bits/mathhelper.h"
#include <asm/asm_cpp.h>
#include <config.h>
#include <cstddef>
#include <cstdint>
#include <debug/debug.h>
#include <gsl/pointer>
#include <klog/klog.h>
#include <mm/mm.h>
#include <mm/paging/paging.h>
#include <new>

namespace alloc
{
    struct block_header
    {
        std::size_t size;
        block_header* back;
    };

    namespace
    {
        block_header* root = nullptr;
        block_header* last = nullptr;
        std::size_t malloced_bytes = 0;
    } // namespace
    inline constexpr auto ALIGN = 16;

    inline static auto next_of(block_header* header) -> block_header* { return as_ptr(as_uptr(header) + sizeof(block_header) + (header->size & ~1)); }

    inline static auto extend(void* buf, std::size_t size) -> std::size_t
    {
        std::size_t pages = std::div_roundup(size, paging::PAGE_SMALL_SIZE);

        for (std::size_t i = 0; i < pages; i++)
        {
            void* buf = mm::pmm_allocate();
            if (buf == nullptr)
            {
                debug::panic("cannot get memory for heap");
            }
            paging::request_page(paging::page_type::SMALL, as_uptr(buf) + i * paging::PAGE_SIZE, mm::make_physical(buf));
            invlpg(as_vptr(as_uptr(buf) + i * paging::PAGE_SMALL_SIZE));
        }

        return paging::PAGE_SMALL_SIZE * pages;
    }

    void init(void* ptr, std::size_t size) { root = last = new (ptr) block_header{(size - sizeof(block_header)) | 1, nullptr}; }

    auto get_alloced_size() -> std::size_t { return malloced_bytes; }

    auto malloc(std::size_t size) -> void*
    {
        size = (size + (ALIGN - 1)) & ~(ALIGN - 1);
        if (!size)
        {
            return nullptr;
        }

        malloced_bytes += size;

        block_header* hdr = root;
        while (hdr <= last)
        {
            // if the least significant bit == 0 : alloc
            //                              == 1 : free
            if ((hdr->size & 1) && (hdr->size & ~1) >= size)
            {
                std::size_t bsize = hdr->size & ~1;
                if (bsize - size > sizeof(block_header))
                {
                    hdr->size = size;
                    auto *next_block = new (next_of(hdr)) block_header{(bsize - size - sizeof(block_header)) | 1, hdr};

                    if (hdr == last)
                    {
                        last = next_block;
                    }
                    else
                    {
                        next_of(next_block)->back = next_block;
                    }
                }

                hdr->size &= ~1;
                return (void*)++hdr;
            }
            hdr = next_of(hdr);
        }

        // extend memory and try to do stuff???
        std::size_t new_size = extend((void*)next_of(last), size + 0x100) & ~7;
        if ((last->size & 1) == 0)
        {
            auto* u = new (next_of(last)) block_header{size & ~1, last};
            auto* l = new (next_of(u)) block_header{(new_size - size - 2 * sizeof(block_header)) | 1, u};
            last = l;
            return (void*)++u;
        }

        std::size_t bsize = ((last->size & ~1) + new_size) - sizeof(block_header) - size;
        last->size = size;
        auto* l = new (next_of(last)) block_header{bsize | 1, last};
        last->size = size;
        auto* u = last;
        last = l;
        return (void*)++u;
    }

    auto aligned_malloc(std::size_t size, std::size_t align) -> void*
    {
        if (align <= ALIGN)
        {
            return malloc(size);
        }

        klog::panic("bad");

        auto ptr = as_uptr(malloc(align + size));
        std::uintptr_t aligned_ptr = std::div_roundup(ptr, align);

        if (ptr != align)
        {
            // insert unaligned pointer back to real root
            as_ptr<std::uintptr_t>(aligned_ptr)[-1] = ptr | 1;
        }

        return as_ptr(aligned_ptr);
    }

    auto realloc(void* buf, std::size_t size) -> void*
    {
        block_header* hdr = as_ptr<block_header>(buf) - 1;
        if (hdr->size > size)
        {
            return buf;
        }

        char* src = as_ptr(buf);
        char* target = as_ptr(malloc(size + 16));
        for (std::size_t i = 0; i < hdr->size; i++)
        {
            target[i] = src[i];
        }
        return target;
    }

    void free(void* buffer)
    {
        if (buffer == nullptr)
        {
            return;
        }

        /*
        if (as_ptr<std::uintptr_t>(buffer)[-1])
        {
            buffer = as_vptr(as_ptr<std::uintptr_t>(buffer)[-1] & ~1);
        }*/

        auto* type = as_ptr<std::size_t>(buffer) - 1;
        auto* hdr = as_ptr<block_header>(buffer) - 1;

        malloced_bytes -= hdr->size;

        if (*type & 2)
        {
            hdr = as_ptr(*type);
        }

        bool is_prev = hdr->back ? hdr->back->size & 1 : false;
        bool is_next = hdr != last ? next_of(hdr)->size & 1 : false;
        block_header* prev = hdr->back;
        block_header* next = next_of(hdr);

        if (is_prev && is_next)
        {
            prev->size = (prev->size & ~1) + (hdr->size & ~1) + (next->size & ~1) + 2 * sizeof(block_header);
            prev->size |= 1;

            // update the back pointer of the block after next
            if (next_of(next) <= last)
            {
                next_of(next)->back = prev;
            }

            if (next == last)
            {
                last = prev;
            }
        }
        else if (is_prev)
        {
            prev->size = (prev->size & ~1) + (hdr->size & ~1) + sizeof(block_header);
            prev->size |= 1;

            // same logic as before
            if (next <= last)
            {
                next->back = prev;
            }

            if (hdr == last)
            {
                last = prev;
            }
        }
        else if (is_next)
        {
            hdr->size = (hdr->size & ~1) + (next->size & ~1) + sizeof(block_header);
            hdr->size |= 1;

            // same logic as before
            if (next_of(next) <= last)
            {
                next_of(next)->back = hdr;
            }

            if (next == last)
            {
                last = hdr;
            }
        }
        else
        {
            hdr->size |= 1;
        }
    }
} // namespace alloc

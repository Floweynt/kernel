#include "bits/mathhelper.h"
#include <asm/asm_cpp.h>
#include <config.h>
#include <cstddef>
#include <cstdint>
#include <debug/debug.h>
#include <klog/klog.h>
#include <mm/pmm.h>
#include <new>
#include <paging/paging.h>

namespace vmm 
{
    struct vmm_block_header
    {
        vmm_block_header* back;
        vmm_block_header* next;
        std::size_t size;
        std::uintptr_t data;
        std::size_t flag;
    };

    static vmm_block_header* root = nullptr;
    static vmm_block_header* last = nullptr;
    static std::size_t malloced_bytes = 0;

    inline constexpr auto ALIGN = paging::PAGE_SMALL_SIZE;

    inline static vmm_block_header* next_of(vmm_block_header* header) { return header->next; }

    constexpr bool is_free(vmm_block_header& header) { return std::get_bit<0>(header.flag); }
    constexpr void set_free(vmm_block_header& header, bool new_value = true) { std::set_bit<0>(header.flag, new_value); }
    constexpr std::size_t make_flags(bool is_free)
    {
        std::size_t val;
        std::set_bit<0>(val, is_free);
        return val;
    }

    std::size_t get_alloced_size() { return malloced_bytes; }

    void init()
    {
        root = last = new vmm_block_header{
            nullptr,
            nullptr,
            config::get<"vmm-size">,
            config::get<"mmap.start.vmm">,
            make_flags(true)
        } 
    }

    void* vmalloc(std::size_t size)
    {
        size = (size + (ALIGN - 1)) & ~(ALIGN - 1);
        if (!size)
            return nullptr;

        malloced_bytes += size;

        vmm_block_header* hdr = root;
        while (hdr <= last)
        {
            if (is_free(*hdr) && hdr->size >= size)
            {
                if (hdr->size - size > 0)
                {
                    hdr->size = size;
                    auto* next_block =
                        new vmm_block_header{hdr, hdr->next, hdr->size - size, hdr->data + size, make_flags(true)};

                    hdr->next = next_block;

                    if (hdr == last)
                        last = next_block;
                    else
                        next_of(next_block)->back = next_block;
                }

                set_free(*hdr, false);

                return (void*)hdr->data;
            }
            hdr = next_of(hdr);
        }
        klog::panic("virtual memory exhausted");
    }

    void vfree(void* buffer)
    {
        klog::log("warning: vfree not implemented"); 
    }

} // namespace alloc


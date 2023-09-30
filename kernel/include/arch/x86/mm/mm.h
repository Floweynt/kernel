#pragma once

#include <bitmanip.h>
#include <bitset>
#include <config.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <intrusive_list.h>
#include <kinit/boot_resource.h>
#include <misc/pointer.h>
#include <mm/paging/paging_entries.h>
#include <sync/spinlock.h>
#include <utils/utils.h>

namespace mm
{
    // address management routines
    void init();
    
    constexpr auto make_physical(std::uintptr_t virt) -> std::uintptr_t { return virt & ~config::get_val<"mmap.start.hhdm">; }

    template <typename T>
    auto make_physical(T* virt) -> std::uintptr_t
    {
        return make_physical(std::uintptr_t(virt));
    }

    inline auto make_physical_kern(std::uintptr_t virt) -> std::uintptr_t
    {
        return virt - config::get_val<"mmap.start.kernel"> + boot_resource::instance().kernel_phys_addr();
    }

    template <typename T>
    constexpr auto make_physical_kern(T* virt) -> std::uintptr_t
    {
        return make_physical_kern(std::uintptr_t(virt));
    }

    constexpr auto make_virtual(std::uintptr_t phy) -> std::uintptr_t { return phy | config::get_val<"mmap.start.hhdm">; }

    template <typename T>
    constexpr auto make_virtual(std::uintptr_t phy) -> T*
    {
        return (T*)make_virtual(phy);
    }

    inline auto make_virtual_kern(std::uintptr_t phy) -> std::uintptr_t
    {
        return phy - boot_resource::instance().kernel_phys_addr() + config::get_val<"mmap.start.kernel">;
    }

    template <typename T>
    auto make_virtual_kern(std::uint64_t phy) -> T*
    {
        return (T*)make_virtual_kern(phy);
    }

    // information about a physical page
    class page_info
    {
        // linked list stuff
        tag_ptr<page_info> prev;
        tag_ptr<page_info> next;
        lock::spinlock spinlock;
        std::uint32_t padding0;
        std::uint64_t padding1;

    public:
        [[nodiscard]] constexpr auto get_prev() const { return prev.get_ptr(); }
        [[nodiscard]] constexpr auto get_next() const { return next.get_ptr(); }
        [[nodiscard]] constexpr auto set_prev(page_info* new_prev) { prev.set_ptr(new_prev); }
        [[nodiscard]] constexpr auto set_next(page_info* new_next) { next.set_ptr(new_next); }

        enum type
        {
            FREE,
            USED,
            USED_MM,
            USED_PFN,
            USED_DMA,
            TYPE_MAX = USED_DMA
        };

        static_assert(TYPE_MAX < 8);

        [[nodiscard]] constexpr auto get_type() const { return (type)prev.get_tag(); }
        [[nodiscard]] constexpr auto set_type(type t) { return prev.set_tag(t); }
        [[nodiscard]] constexpr auto get_lock() -> auto& { return spinlock; }
    };

    // make sure that the size of the page_info is 2^n
    static_assert(__builtin_popcount(sizeof(page_info)) == 1);
    // pmm routines
    void pmm_add_region(std::uintptr_t, std::size_t);
    auto pmm_allocate() -> void*;
    INLINE auto pmm_allocate_clean() -> void* 
    {
    auto *ptr = pmm_allocate();;
        return ptr != nullptr ? std::memset(ptr, 0, paging::PAGE_SMALL_SIZE) : ptr;
    }

    auto pmm_free(void* addr);
    auto pmm_get_free_list() -> std::intrusive_list<page_info>&;
    auto pmm_stupid_allocate() -> void*;
    
    INLINE auto get_pfn(void* addr) -> page_info&
    {
        return as_ptr<page_info>(config::get_val<"mmap.start.pfn">)[make_physical(addr) / paging::PAGE_SMALL_SIZE];
    }

    INLINE auto get_pfn(std::uintptr_t addr) -> page_info&
    {
        return as_ptr<page_info>(config::get_val<"mmap.start.pfn">)[make_physical(addr) / paging::PAGE_SMALL_SIZE];
    }

    INLINE auto pfn_to_page(const page_info& info)
    {
        return make_virtual(((as_uptr(&info) - config::get_val<"mmap.start.pfn">) / sizeof(page_info)) * paging::PAGE_SMALL_SIZE);
    }

    auto vmm_allocate(std::size_t pages) -> void*;
    void vmm_free(void* pointer, std::size_t pages);
} // namespace mm

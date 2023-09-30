#include <config.h>
#include <cstdint>
#include <intrusive_list.h>
#include <kinit/limine.h>
#include <klog/klog.h>
#include <misc/kassert.h>
#include <mm/mm.h>
#include <mm/paging/paging.h>
#include <sync/spinlock.h>
#include <utils/id_allocator.h>

namespace mm
{
    inline constexpr auto PMM_COUNT = config::get_val<"pmm-count">;

    static std::intrusive_list<page_info> free_list;
    static lock::spinlock pmm_alloc_lock;

    void pmm_add_region(std::uintptr_t /*unused*/, std::size_t /*unused*/) { expect(false, "not implemented"); }

    // TODO: figure out sync
    auto pmm_allocate() -> void*
    {
        lock::spinlock_guard guard(pmm_alloc_lock);
        while (auto* ptr = free_list.get_front())
        {
            free_list.pop_front();
            lock::spinlock_guard page_access_guard(ptr->get_lock());
            ptr->set_type(page_info::USED);

            return mm::make_virtual<void>(pfn_to_page(*ptr));
        }

        return nullptr;
    }

    auto pmm_free(void* addr)
    {
        auto& pfn = get_pfn(addr);
        lock::spinlock_guard page_access_guard(pfn.get_lock());
        pfn.set_type(page_info::FREE);
        lock::spinlock_guard guard(pmm_alloc_lock);
        free_list.add_front(&pfn);
    }

    auto pmm_get_free_list() -> std::intrusive_list<page_info>& { return free_list; }
} // namespace mm

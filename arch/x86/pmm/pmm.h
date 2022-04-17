#ifndef __ARCH_X86_PMM_PMM_H__
#define __ARCH_X86_PMM_PMM_H__
#include <bitmanip.h>
#include <cstddef>
#include <cstring>
#include <utils.h>
#include <kinit/boot_resource.h>
#include <sync/spinlock.h>
namespace pmm
{
    class pmm_region
    {
        uint64_t* region_start;
        uint64_t* data_start;
        std::size_t len;
        std::size_t s;
        lock::spinlock lock;

    public:
        inline pmm_region() : pmm_region(nullptr, 0) {}
        inline pmm_region(const pmm_region&) = default;
        // len - pages
        pmm_region(void* region_start, std::size_t len);

        constexpr std::size_t size()
        {
            return region_start == nullptr ? 0 : len - std::detail::div_roundup(len, 64ul * 4096);
        }

        constexpr bool exists() { return region_start != nullptr; }

        void* allocate();
    };

    void add_region(void*, std::size_t);
    void* pmm_allocate();

    constexpr uint64_t make_physical(uint64_t virt) { return virt - 0xffff800000000000; }

    template <typename T>
    uint64_t make_physical(T* virt)
    {
        return make_physical(uint64_t(virt));
    }

    inline uint64_t make_physical_kern(uint64_t virt) { return virt - 0xffffffff80000000 + boot_resource::instance().kernel_phys_addr(); }

    template <typename T>
    constexpr uint64_t make_physical_kern(T* virt)
    {
        return make_physical_kern(uint64_t(virt));
    }

    constexpr uint64_t make_virtual(uint64_t phy) { return phy + 0xffff800000000000; }

    template <typename T>
    constexpr T* make_virtual(uint64_t phy)
    {
        return (T*)make_virtual(phy);
    }


    inline uint64_t make_virtual_kern(uint64_t phy) 
    { 
        return phy - boot_resource::instance().kernel_phys_addr() + 0xffffffff80000000; 
    }

    template <typename T>
    T* make_virtual_kern(uint64_t phy)
    {
        return (T*)make_virtual_kern(phy);
    }
} // namespace pmm

#endif

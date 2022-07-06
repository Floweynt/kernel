#ifndef __ARCH_X86_MM_MM_H__
#define __ARCH_X86_MM_MM_H__
#include <bitmanip.h>
#include <cstddef>
#include <cstring>
#include <kinit/boot_resource.h>
#include <utils/utils.h>
#include <config.h>
namespace mm
{
    class bitmask_allocator
    {
        uint64_t* buf;
        std::size_t len;
    public:
        static constexpr std::size_t metadata_size_pages(std::size_t n)
        {
            return std::detail::div_roundup((n + 63) & ~63, 4096ul);
        }

        static constexpr std::size_t metadata_size_bytes(std::size_t n) { return std::detail::div_roundup(n, 64ul) * 8; }

        inline bitmask_allocator() : bitmask_allocator(nullptr, 0) {}
        inline bitmask_allocator(const bitmask_allocator&) = default;
        bitmask_allocator(void* buf, std::size_t len);

        constexpr std::size_t size() { return len; }
        constexpr bool exists() { return len != 0; }
        constexpr void* get_buffer() { return (void*)buf; }

        constexpr bool test(std::size_t i) { return std::get_bit(buf[i / 64], 63 - i % 64); }

        std::size_t allocate(std::size_t s);
        void free(std::size_t);
    };

    constexpr uint64_t make_physical(uint64_t virt) { return virt & ~HHDM_START; }

    template <typename T>
    uint64_t make_physical(T* virt)
    {
        return make_physical(uint64_t(virt));
    }

    inline uint64_t make_physical_kern(uint64_t virt)
    {
        return virt - 0xffffffff80000000 + boot_resource::instance().kernel_phys_addr();
    }

    template <typename T>
    constexpr uint64_t make_physical_kern(T* virt)
    {
        return make_physical_kern(uint64_t(virt));
    }

    constexpr uint64_t make_virtual(uint64_t phy) { return phy | HHDM_START; }

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
} // namespace mm

#endif

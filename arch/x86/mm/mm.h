#ifndef __ARCH_X86_MM_MM_H__
#define __ARCH_X86_MM_MM_H__
#include <bitmanip.h>
#include <bitset>
#include <config.h>
#include <cstddef>
#include <cstring>
#include <kinit/boot_resource.h>
#include <utils/utils.h>
namespace mm
{
    class bitmask_allocator
    {
        std::unmanaged_dynamic_bitset bitmask;

    public:
        static constexpr std::size_t metadata_size_pages(std::size_t n) { return std::div_roundup((n + 63) & ~63, 4096ul); }

        static constexpr std::size_t metadata_size_bytes(std::size_t n) { return std::div_roundup(n, 64) * 8; }

        constexpr bitmask_allocator() : bitmask_allocator(nullptr, 0) {}
        constexpr bitmask_allocator(const bitmask_allocator&) = default;
        constexpr bitmask_allocator(void* buf, std::size_t len) : bitmask(len, buf) { bitmask.set(); }

        constexpr std::size_t size() { return bitmask.size(); }
        constexpr bool exists() { return bitmask.size() != 0; }
        constexpr void* get_buffer() { return bitmask.data(); }

        constexpr bool test(std::size_t i) { return bitmask.test(i); }

        std::size_t allocate(std::size_t s);
        constexpr void free(std::size_t s) { bitmask.reset(s); }
    };

    constexpr std::uintptr_t make_physical(std::uintptr_t virt) { return virt & ~config::get_val<"mmap.start.hhdm">; }

    template <typename T>
    std::uintptr_t make_physical(T* virt)
    {
        return make_physical(std::uintptr_t(virt));
    }

    inline std::uintptr_t make_physical_kern(std::uintptr_t virt)
    {
        return virt - 0xffffffff80000000 + boot_resource::instance().kernel_phys_addr();
    }

    template <typename T>
    constexpr std::uintptr_t make_physical_kern(T* virt)
    {
        return make_physical_kern(std::uintptr_t(virt));
    }

    constexpr std::uintptr_t make_virtual(std::uintptr_t phy) { return phy | config::get_val<"mmap.start.hhdm">; }

    template <typename T>
    constexpr T* make_virtual(std::uintptr_t phy)
    {
        return (T*)make_virtual(phy);
    }

    inline std::uintptr_t make_virtual_kern(std::uintptr_t phy)
    {
        return phy - boot_resource::instance().kernel_phys_addr() + config::get_val<"mmap.start.kernel">;
    }

    template <typename T>
    T* make_virtual_kern(std::uint64_t phy)
    {
        return (T*)make_virtual_kern(phy);
    }
} // namespace mm

#endif

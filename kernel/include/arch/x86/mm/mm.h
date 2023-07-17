#pragma once

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
        static constexpr auto metadata_size_pages(std::size_t size) -> std::size_t { return std::div_roundup((size + 63) & ~63, 4096ul); }

        static constexpr auto metadata_size_bytes(std::size_t size) -> std::size_t { return std::div_roundup(size, 64) * 8; }

        constexpr bitmask_allocator() : bitmask_allocator(nullptr, 0) {}
        constexpr bitmask_allocator(const bitmask_allocator&) = default;
        constexpr bitmask_allocator(void* buf, std::size_t len) : bitmask(len, buf) { bitmask.set(); }

        constexpr auto size() -> std::size_t { return bitmask.size(); }
        constexpr auto exists() -> bool { return bitmask.size() != 0; }
        constexpr auto get_buffer() -> void* { return bitmask.data(); }

        constexpr auto test(std::size_t index) -> bool { return bitmask.test(index); }

        auto allocate(std::size_t size) -> std::size_t;
        constexpr void free(std::size_t size) { bitmask.reset(size); }
    };

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
} // namespace mm

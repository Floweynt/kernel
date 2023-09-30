#pragma once

#include <cstdint>
#include <fd/fdhandler.h>
#include <memory>

namespace vfs
{
    class vfs;
    class vnode;
    class vfs_operations;
    class vnode_operations;

    struct io_op
    {
        // page-aligned buffer to write 
        void* buffer;
        // length, in pages?
        std::size_t length;

        enum
        {
            WRITE,
            READ,
            DONE
        } status;
    };

    class vfs_operations : public std::simple_refcountable<std::uint64_t>
    {
    public:
        virtual ~vfs_operations() = default;

        virtual auto mount_on(vfs& instance, vnode& node) -> int = 0;
    };

    class vnode_operations : public std::simple_refcountable<std::uint64_t>
    {
    public:
        virtual ~vnode_operations() = default;

        // TODO: make this function take a IO queue, in order to optimize DMA enqueue
        auto rdwr(vnode& instance, io_op& io_operations) -> int;
    };

    class vfs : public std::simple_refcountable<std::uint64_t>
    {
    public:
        enum vfs_flags
        {
            READONLY = 1 << 0,
        };

    private:
        std::refcounted<vfs> next;
        std::refcounted<vfs_operations> operations;
        std::refcounted<vnode> mounted_node;
        std::uint32_t flags;
        std::uint32_t block_size;
        std::uint64_t fs_id;

    public:
        // invokers
        auto mount_on(vnode& node) { return operations->mount_on(*this, node); }

        [[nodiscard]] constexpr auto get_block_size() const { return block_size; }
        [[nodiscard]] constexpr auto get_id() const { return fs_id; }

        [[nodiscard]] constexpr auto is_readonly() const -> bool { return (flags & READONLY) != 0; }
    };

    class vnode : public std::simple_refcountable<std::uint64_t>
    {
    public:
        enum vnode_flags
        {
            GLOBAL_FS_ROOT = (1 << 0),
            LOCAL_FS_ROOT = (1 << 1),
        };

        enum vnode_type : std::uint8_t
        {
            NO_TYPE = 0,
            REGULAR,
            DIRECTORY,
            BLOCK,
            CHARACTER,
            LINK,
            SOCKET,
            BAD,
            PIPE
        };

    private:
        std::uint32_t shared_locks;
        std::uint32_t exclusive_locks;
        vnode_type type;
        std::uint8_t __unused_pad_0;
        std::uint16_t flags;
        std::uint32_t capabilities;

        // important pointers
        std::refcounted<vfs> vfs_mounted_here;
        std::refcounted<vnode_operations> operations;
        std::refcounted<vfs> my_vfs;

    public:
        // getters
        [[nodiscard]] constexpr auto get_type() const { return type; }
        [[nodiscard]] constexpr auto get_shared_locks() const { return shared_locks; }
        [[nodiscard]] constexpr auto get_exclusive_locks() const { return exclusive_locks; }
        [[nodiscard]] constexpr auto is_global_fs_root() const -> bool { return (flags & GLOBAL_FS_ROOT) != 0; }
        [[nodiscard]] constexpr auto is_local_fs_root() const -> bool { return (flags & LOCAL_FS_ROOT) != 0; }

        // invokers
        auto rdwr(io_op& op) { return operations->rdwr(*this, op); }
    };
} // namespace vfs


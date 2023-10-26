#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>
#include <user/fd/fd.h>

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
        std::size_t length;
        std::size_t offset;

        enum
        {
            WRITE,
            READ,
            DONE
        } status;
    };

    // operations
    class vfs_operations : public std::simple_refcountable<std::uint64_t, vnode_operations>
    {
    public:
        virtual ~vfs_operations() = default;
    };

    class vnode_operations : public std::simple_refcountable<std::uint64_t, vnode_operations>
    {
    public:
        virtual ~vnode_operations() = default;

        // TODO: make this function take a IO queue, in order to optimize DMA enqueue
        virtual auto rdwr(const std::refcounted<vnode>& instance, io_op& io_operations) -> int = 0;
        virtual auto query(const std::refcounted<vnode>& instance, const std::string_view& name) -> vnode* = 0;
        virtual auto mount_on(const std::refcounted<vnode>& instance, const std::refcounted<vnode>& node) -> int = 0;
    };

    // actual data structures
    class vfs : public std::simple_refcountable<std::uint64_t, vnode>
    {
    public:
        enum vfs_flags
        {
            READONLY = 1 << 0,
        };

    protected:
        // std::refcounted<vfs> next;
        std::refcounted<vfs_operations> operations;
        std::refcounted<vnode> root;
        std::uint32_t flags;
        std::uint32_t block_size;
        std::uint64_t fs_id;

        vfs(std::refcounted<vfs_operations> ops, std::uint32_t flags, std::uint32_t block_size, std::uint64_t fs_id)
            : simple_refcountable(), operations(std::move(ops)), flags(flags), block_size(block_size), fs_id(fs_id)
        {
        }

    public:
        // invokers
        [[nodiscard]] constexpr auto get_block_size() const { return block_size; }
        [[nodiscard]] constexpr auto get_id() const { return fs_id; }
        [[nodiscard]] constexpr auto get_root() const -> const auto& { return root; }

        [[nodiscard]] constexpr auto is_readonly() const -> bool { return (flags & READONLY) != 0; }

        virtual ~vfs();
    };

    class vnode : public std::simple_refcountable<std::uint64_t, vnode>
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

    protected:
        std::uint32_t shared_locks{};
        std::uint32_t exclusive_locks{};
        vnode_type type;
        std::uint8_t __unused_pad_0{};
        std::uint16_t flags{};
        std::uint32_t capabilities{};

        // important pointers
        std::refcounted<vnode_operations> operations;
        vfs* my_vfs; // weak ref

        // TODO: use forward_list
        struct mount_entry
        {
            mount_entry* next;
            std::refcounted<vnode> mount;
        };

        mount_entry* mounts{};

        vnode(std::refcounted<vnode_operations> ops, vfs* my_vfs) : simple_refcountable(), operations(std::move(ops)), my_vfs(my_vfs) {}

    public:
        // getter
        [[nodiscard]] constexpr auto get_type() const { return type; }
        [[nodiscard]] constexpr auto get_shared_locks() const { return shared_locks; }
        [[nodiscard]] constexpr auto get_exclusive_locks() const { return exclusive_locks; }
        [[nodiscard]] constexpr auto is_global_fs_root() const -> bool { return (flags & GLOBAL_FS_ROOT) != 0; }
        [[nodiscard]] constexpr auto is_local_fs_root() const -> bool { return (flags & LOCAL_FS_ROOT) != 0; }

        // invokers
        inline auto rdwr(io_op& op) { return operations->rdwr(ref_this(), op); }
        inline auto query(const std::string_view& path) { return operations->query(ref_this(), path); }
        auto mount_on(const std::refcounted<vnode>& node) { return operations->mount_on(ref_this(), node); }

        void add_mount(const std::refcounted<vnode>& fs);
        auto get_actual_node() -> std::refcounted<vnode>;

        virtual ~vnode();
    };
} // namespace vfs

// abstract vfs operations
namespace vfs
{
    auto get_root() -> const std::refcounted<vnode>&;
    void read(const std::refcounted<vnode>& vnode, void* buffer, std::size_t offset, std::size_t size);
    void mount_on(const std::refcounted<vnode>& node, const std::refcounted<vnode>& root);
    auto lookup(const std::refcounted<vnode>& parent, const std::string_view& path) -> std::refcounted<vnode>;
} // namespace vfs

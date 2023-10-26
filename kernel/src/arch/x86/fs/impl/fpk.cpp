#include <bits/mathhelper.h>
#include <bits/utils.h>
#include <cstddef>
#include <cstdint>
#include <fs/impl/fpk.h>
#include <fs/vfs.h>
#include <klog/klog.h>
#include <memory>

// stupid file packer

namespace fpk
{
    struct node_header
    {
        std::uint16_t perms;
        std::uint16_t uid;
        std::uint16_t gid;
        std::uint16_t name_size;
        std::uint64_t size;
        std::uint64_t atime;
        std::uint64_t mtime;

        enum type
        {
            ROOT = 0x0000,
            DIR = 0x1000,
            FILE = 0x2000,
            TYPE_MASK = 0xf000,
        };

        enum perm
        {
            O_X = 00001,
            O_W = 00002,
            O_R = 00004,
            G_X = 00010,
            G_W = 00020,
            G_R = 00040,
            U_X = 00100,
            U_W = 00200,
            U_R = 00400,
            S = 01000,
            SGID = 02000,
            SUID = 04000,

            PERM_MASK = 0x0fff
        };
    };

    inline static constexpr auto MAGIC_NO = 0x3bef1ea1;

    struct file_header
    {
        std::uint32_t magic;
        std::uint16_t ver_major;
        std::uint16_t ver_minor;
        std::uint64_t root_offset;
    };
} // namespace fpk

class fpk_vnode;
class fpk_vnode_ops;
class fpk_vfs_ops;
class fpk_vfs;

static auto get_vnode_ops() -> std::refcounted<fpk_vnode_ops>;
static auto get_vfs_ops() -> std::refcounted<fpk_vfs_ops>;

class fpk_vfs : public vfs::vfs
{
    std::uint8_t* base{};
    fpk::file_header* header{};
    fpk::node_header* root_node{};
    friend class fpk_vfs_ops;
    friend class fpk_vnode_ops;

    std::unordered_map<std::uintptr_t, std::refcounted<fpk_vnode>> cache;

public:
    fpk_vfs(void* data, std::size_t size);
    auto get_vnode_from_inode(fpk::node_header* header) -> fpk_vnode*;

    ~fpk_vfs() override { klog::log("debug: ~fpk_vfs()"); }
};

class fpk_vnode : public vfs::vnode
{
    friend class fpk_vnode_ops;
    fpk::node_header* node;

public:
    fpk_vnode(fpk_vfs* vfs, fpk::node_header* node) : vnode(get_vnode_ops().as<vfs::vfs_operations>(), (vfs::vfs*)vfs), node(node)
    {
        klog::log("debug: fpk_vnode()");
    }

    ~fpk_vnode() override { klog::log("debug: ~fpk_vnode()"); }
};

fpk_vfs::fpk_vfs(void* data, std::size_t size) : vfs::vfs(get_vfs_ops(), 0, 4096, 1), base(as_ptr(data))
{
    klog::log("debug: fpk_vfs()");
    header = as_ptr(as_ptr<std::uint8_t>(data) + size - sizeof(fpk::file_header));
    root_node = as_ptr(as_ptr<std::uint8_t>(data) + header->root_offset);
    root = std::refcounted<::vfs::vnode>(get_vnode_from_inode(root_node));
}

auto fpk_vfs::get_vnode_from_inode(fpk::node_header* header) -> fpk_vnode*
{
    auto inode_id = as_uptr(header);
    if (cache.find(inode_id) == cache.end())
    {
        cache.insert(inode_id, std::refcounted(new fpk_vnode(this, header)));
    }

    return cache[inode_id].get();
}

class fpk_vfs_ops : public vfs::vfs_operations
{
public:
    ~fpk_vfs_ops() override = default;
};

class fpk_vnode_ops : public vfs::vnode_operations
{
    // std::unordered_map<std::string_view, fpk::node_header*> dir_lookcache;
public:
    ~fpk_vnode_ops() override = default;

    auto mount_on(const std::refcounted<vfs::vnode>& /*instance*/, const std::refcounted<vfs::vnode>& /*node*/) -> int override
    {
        klog::log("fpk_vnode_ops: mount_on");
        return 0;
    }

    auto rdwr(const std::refcounted<vfs::vnode>& instance, vfs::io_op& io_operations) -> int override
    {
        klog::log("fpk_vnode_ops: rwdr");
        auto inst = instance.as<fpk_vnode>();
        auto node = *inst->node;

        if (io_operations.status == vfs::io_op::READ)
        {
            if (io_operations.offset > node.size)
            {
                io_operations.status = vfs::io_op::DONE;
                // no error occurred in IO
                return 0;
            }

            std::size_t max_read_size = node.size - io_operations.offset;
            std::size_t read_size = std::min(max_read_size, io_operations.length);
            // cancer
            std::memcpy(io_operations.buffer, ((std::uint8_t*)&node) + sizeof(fpk::node_header) + node.name_size, read_size);
        }
        else
        {
            klog::panic("not implemented?");
        }

        return 0;
    }

    auto query(const std::refcounted<vfs::vnode>& instance, const std::string_view& name) -> vfs::vnode* override
    {
        klog::log("fpk_vnode_ops: query '%.*s'", (int)name.size(), name.data());

        auto inst = instance.as<fpk_vnode>();
        auto& node = *inst->node;
        auto& vfs = static_cast<fpk_vfs&>(*inst->my_vfs);

        std::uint64_t* offsets = as_ptr(std::div_roundup(as_uptr(as_ptr<std::uint8_t>(&node) + sizeof(fpk::node_header) + node.name_size), 8) * 8);

        for (std::size_t i = 0; i < node.size; i++)
        {
            fpk::node_header* subnode = as_ptr(vfs.base + offsets[i]);
            std::string_view subnode_name = std::string_view(as_ptr(subnode + 1), subnode->name_size);
            if (subnode_name == name)
            {
                return vfs.get_vnode_from_inode(subnode);
            }
        }

        return nullptr;
    }
};

static auto get_vnode_ops() -> std::refcounted<fpk_vnode_ops>
{
    static std::refcounted<fpk_vnode_ops> vnode_ops(new fpk_vnode_ops);
    return vnode_ops;
}

static auto get_vfs_ops() -> std::refcounted<fpk_vfs_ops>
{
    static std::refcounted<fpk_vfs_ops> vfs_ops(new fpk_vfs_ops);
    return vfs_ops;
}

auto load(void* data, std::size_t size) -> std::refcounted<vfs::vfs>
{
    auto vfs = std::refcounted((vfs::vfs*)new fpk_vfs(data, size));
    return vfs;
}

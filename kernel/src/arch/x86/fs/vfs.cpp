#include <fs/vfs.h>
#include <klog/klog.h>
#include <memory>
#include <string_view>

namespace vfs
{
    namespace
    {
    }

    vfs::~vfs()
    {
        // TODO: impl
    }

    void vnode::add_mount(const std::refcounted<vnode>& fs) { mounts = new mount_entry{.next = mounts, .mount = std::refcounted(fs)}; }

    auto vnode::get_actual_node() -> std::refcounted<vnode>
    {
        if (mounts != nullptr)
        {
            return mounts->mount;
        }

        return ref_this();
    }

    vnode::~vnode()
    {
        auto* mount = mounts;
        while (mount != nullptr)
        {
            auto* old = mount;
            mount = mount->next;
            delete old;
        }
    }

    void read(const std::refcounted<vnode>& vnode, void* buffer, std::size_t offset, std::size_t size)
    {
        io_op op{
            .buffer = buffer,
            .length = size,
            .offset = offset,
            .status = io_op::READ,
        };

        vnode->rdwr(op);
    }

    auto get_root() -> const std::refcounted<vnode>&
    {
        class root_vfs_ops : public vfs_operations
        {
        public:
            ~root_vfs_ops() override = default;
        };

        class root_vnode_ops : public vnode_operations
        {
            auto rdwr(const std::refcounted<vnode>& /*instance*/, io_op& /*io_operations*/) -> int override
            {
                klog::panic("vfs: rdwr called on internel vfs rootfs mountpoint, did you forget to call get_actual_node?");
            }

            auto query(const std::refcounted<vnode>& /*instance*/, const std::string_view& /*name*/) -> vnode* override
            {
                klog::panic("vfs: query called on internel vfs rootfs mountpoint, did you forget to call get_actual_node?");
            }

            auto mount_on(const std::refcounted<vnode>& /*instance*/, const std::refcounted<vnode>& /*node*/) -> int override
            {
                klog::panic("vfs: mount_on called on internel vfs rootfs mountpoint, did you forget to call get_actual_node?");
            }

        public:
            ~root_vnode_ops() override = default;
        };

        class root_vfs : public vfs
        {
        public:
            root_vfs() : vfs(std::make_refcounted<root_vfs_ops>(), 0, 0, 0) {}
        };

        class root_vnode : public vnode
        {
        public:
            root_vnode() : vnode(std::make_refcounted<root_vnode_ops>(), new root_vfs) {}
        };

        static std::refcounted<vnode> root(new root_vnode);
        return root;
    }

    void mount_on(const std::refcounted<vnode>& node, const std::refcounted<vnode>& root)
    {
        // first we tell the root we are mounting on node
        root->mount_on(node);
        // then, we actually mask
        node->add_mount(root);
    }

    static void iterate_path_component(const std::string_view& path, auto callback)
    {
        for (std::size_t i = 0; i < path.size(); i++)
        {
            // we skip /
            for (; i < path.size() && path[i] == '/'; i++)
                ;

            std::size_t start = i;
            std::size_t len = 0;
            for (; i < path.size() && path[i] != '/'; i++)
                len++;

            if (len)
            {
                callback(path.substr(start, len));
            }
        }
    }

    auto lookup(const std::refcounted<vnode>& parent, const std::string_view& path) -> std::refcounted<vnode>
    {
        std::refcounted<vnode> vn = parent;
        if (path[0] == '/')
        {
            vn = get_root();
        }

        vn = vn->get_actual_node();

        iterate_path_component(path, [&](const std::string_view& component) {
            vn = vn->query(component)->get_actual_node();
            if (vn->get_type() == vnode::LINK)
            {
                klog::panic("TODO: implement soft-links");
            }
        });

        return vn;
    }
} // namespace vfs

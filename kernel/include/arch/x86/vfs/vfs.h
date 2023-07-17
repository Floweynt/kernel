#pragma once

#include <fd/fdhandler.h>
#include <cstdint>
#include <memory>

enum vtype
{
    VNON,
    VREG,
    VDIR,
    VBLK,
    VCHR,
    VLNK,
    VSOCK,
    VBAD,
    VFIFO
};

class vfs;
class vnode;
class vfs_operations;
class vnode_operations;

class vfs_fd : public fd_data
{
};

class vfs_operations : public std::simple_refcountable<std::uint32_t>
{
public:
    virtual ~vfs_operations() = default;
    virtual auto mount_on(vfs& instance, vnode& node, const char* path) -> int = 0;
    // virtual auto vfs_swapvp(vfs& instance) -> int = 0;
};

class vnode_operations : public std::simple_refcountable<std::uint32_t>
{
public:
    auto open(vnode& instance, int flags) -> std::refcounted<vfs_fd>; // TODO: dont ignore authentication!!!
    auto close(vnode& instance, std::refcounted<vfs_fd> target_fd) -> int;
    auto rdwr(vnode& instance) -> int;
    auto ioctl(vnode& instance) -> int;
};

class vfs : public std::simple_refcountable<std::uint32_t>
{
    vfs* next;           /* next vfs in vfs list */
    vfs_operations* operations;   /* operations on vfs */
    vnode* vfs_vnodecovered; /* vnode we mounted on */
    std::uint32_t vfs_flag;  /* flags */
    std::uint32_t vfs_bsize; /* native block size */
    std::uint32_t vfs_fsid;  /* file system id */
    void* vfs_stats;         /* filesystem statistics */
public:
};

class vnode : public std::simple_refcountable<std::uint32_t>
{
    std::uint16_t v_flag;    /* vnode flags (see below) */
    std::uint16_t v_count;   /* reference count */
    std::uint16_t v_shlockc; /* count of shared locks */
    std::uint16_t v_exlockc; /* count of exclusive locks */
    vfs* v_vfsmountedhere;   /* ptr to vfs mounted here */
    vnode_operations* v_op;   /* vnode operations */
    union {
        struct socket* v_Socket; /* unix ipc */
        struct stdata* v_Stream; /* stream */
        struct page* v_Pages;    /* vnode pages list */
    } v_s;
    vfs* v_vfsp;     /* ptr to vfs we are in */
    vtype v_type;    /* vnode type */
    vtype v_rdev;    /* device (VCHR, VBLK) */
    long* v_filocks; /* File/Record locks ... */
    void* v_data;    /* private data for fs */
};


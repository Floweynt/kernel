#pragma once

#include <cstdint>

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
class vfs_functions;
class vnode_functions;


class vfs_functions
{
public:
    virtual ~vfs_functions() = default;
    virtual auto vfs_mount(vfs& instance, const char* path, void* data) -> int = 0;
    virtual auto vfs_unmount(vfs& instance) -> int = 0;
    virtual auto vfs_root(vfs& instance, vnode& node) -> int = 0;
    virtual auto vfs_statfs(vfs& instance /*, type parameter */) -> int = 0;
    virtual auto vfs_sync(vfs& instance) -> int = 0;
    virtual auto vfs_vget(vfs& instance, vnode& node /* fid_t * */) -> int = 0;
    virtual auto vfs_mountroot(vfs& instance, vnode& node /* nm */) -> int = 0;
    // virtual auto vfs_swapvp(vfs& instance) -> int = 0;
};

class vnode_functions
{
public:
    int open();
    int close();
    int rdwr();
    int ioctl();
    int select();
    int getattr();
    int setattr();
    int access();
    int lookup();
    int create();
    int remove();
    int link();
    int rename();
    int mkdir();
    int rmdir();
    int readdir();
    int symlink();
    int readlink();
    int fsync();
    int inactive();
    int lockctl();

    int fid();
    int getpage();
    int putpage();
    int map();
    int dump();
    int cmd();
    int realvp();
    int cntl();
};

class vnode;

class vfs
{
    vfs* vfs_next;           /* next vfs in vfs list */
    vfs_functions* vfs_op;   /* operations on vfs */
    vnode* vfs_vnodecovered; /* vnode we mounted on */
    int vfs_flag;            /* flags */
    int vfs_bsize;           /* native block size */
    fsid_t vfs_fsid;         /* file system id */
    void* vfs_stats;         /* filesystem statistics */
    void* vfs_data;          /* private data */

public:
};

class vnode
{
    uint16_t v_flag;       /* vnode flags (see below) */
    uint16_t v_count;      /* reference count */
    uint16_t v_shlockc;    /* count of shared locks */
    uint16_t v_exlockc;    /* count of exclusive locks */
    vfs* v_vfsmountedhere; /* ptr to vfs mounted here */
    vnode_functions* v_op; /* vnode operations */
    union {
        struct socket* v_Socket; /* unix ipc */
        struct stdata* v_Stream; /* stream */
        struct page* v_Pages;    /* vnode pages list */
    } v_s;
    vfs* v_vfsp;     /* ptr to vfs we are in */
    vtype v_type;    /* vnode type */
    dev_t v_rdev;    /* device (VCHR, VBLK) */
    long* v_filocks; /* File/Record locks ... */
    void* v_data;    /* private data for fs */
};


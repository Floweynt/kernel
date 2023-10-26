#pragma once

#include <cstdint>

// stupid file packer
namespace fpk
{
    struct node_header
    {
        uint16_t perms;
        uint16_t uid;
        uint16_t gid;
        uint16_t name_size;
        uint64_t size;
        uint64_t atime;
        uint64_t mtime;

        enum type
        {
            ROOT = 0x0000,
            DIR = 0x1000,
            FILE = 0x2000,
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
            SUID = 04000
        };
    };

    inline static constexpr auto MAGIC_NO = 0x3bef1ea1;

    struct file_header
    {
        uint32_t magic;
        uint16_t ver_major;
        uint16_t ver_minor;
        uint64_t root_offset;

        enum features
        {
            // not implemented
        };
    };
} // namespace fpk

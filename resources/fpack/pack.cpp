#include "argparse.h"
#include "format.h"
#include <bits/fs_fwd.h>
#include <cassert>
#include <cstdint>
#include <exception>
#include <fcntl.h>
#include <filesystem>
#include <fmt/core.h>
#include <iostream>
#include <span>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <system_error>
#include <type_traits>
#include <unistd.h>
#include <vector>

using namespace fpk;

namespace fs = std::filesystem;

#define throw_system_error throw std::system_error(errno, std::generic_category())

#define test_errno(fd)                                                                                                                               \
    do                                                                                                                                               \
    {                                                                                                                                                \
        if (fd < 0)                                                                                                                                  \
            throw std::system_error(errno, std::generic_category());                                                                                 \
    } while (0)

#define try_syscall(syscall)                                                                                                                         \
    do                                                                                                                                               \
    {                                                                                                                                                \
        if (syscall < 0)                                                                                                                             \
        {                                                                                                                                            \
            throw std::system_error(errno, std::generic_category());                                                                                 \
        }                                                                                                                                            \
    } while (0)

inline static constexpr auto DEFAULT_O_PERMS = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH;

class out_writer
{
    int out_fd;
    size_t count{};

    void do_write(const void* data, size_t size)
    {
        try_syscall(::lseek(out_fd, 0, SEEK_END));
        try_syscall(::write(out_fd, data, size));
        count += size;
    }

public:
    out_writer(const std::string_view& path) : out_fd(open(path.data(), O_CREAT | O_TRUNC | O_RDWR, DEFAULT_O_PERMS)) { test_errno(out_fd); }

    template <typename T>
        requires(std::is_trivial_v<T>)
    auto write(const T& data) -> out_writer&
    {
        do_write(&data, sizeof(data));
        return *this;
    }

    template <typename C>
    auto write(const C& container) -> out_writer&
    {
        do_write(container.data(), sizeof(typename C::value_type) * container.size());
        return *this;
    }

    [[nodiscard]] constexpr auto off() const { return count; }

    auto append_file(const fs::path& path, size_t size) -> out_writer&
    {
        int in_fd = open(path.c_str(), O_RDONLY);
        test_errno(in_fd);
        try_syscall(sendfile(out_fd, in_fd, nullptr, size));
        close(in_fd);
        count = lseek(out_fd, 0, SEEK_END);
        return *this;
    }

    void pad_to(size_t count)
    {
        size_t pad_val = ((off() + count - 1) / count) * count;
        write(std::vector<uint8_t>(pad_val - off()));
    }
};

namespace
{
    bool verbose = false;

    template <typename... Args>
    void write_con(const fmt::format_string<Args...>& str, Args&&... args)
    {
        if (!verbose)
        {
            return;
        }

        std::cerr << fmt::format(str, std::forward<Args>(args)...);
    }

    auto write_file(out_writer& buf, const fs::directory_entry& dirent) -> size_t;
    auto write_dir(out_writer& buf, const fs::directory_entry& dirent, bool is_root = false) -> size_t;

    auto write_dirent(out_writer& buf, const fs::directory_entry& dirent) -> size_t
    {
        if (dirent.is_directory())
        {
            return write_dir(buf, dirent);
        }
        if (dirent.is_regular_file())
        {
            return write_file(buf, dirent);
        }

        std::cerr << fmt::format("unknown file type (not dir/file) for {}, skipping", dirent.path().string());
        exit(-1);
    }

    auto do_stat(const fs::path& path) -> struct stat
    {
        struct stat info
        {
        };
        if (stat(path.c_str(), &info) < 0)
        {
            throw_system_error;
        }

        return info;
    }

    auto
    write_header_generic(out_writer& buf, const fs::directory_entry& dirent, size_t file_size, bool is_root = false) -> size_t
    {
        uint16_t perms = static_cast<uint16_t>(dirent.status().permissions() & fs::perms::mask);
        auto info = do_stat(dirent.path());
        auto fname = dirent.path().filename().string();

        assert(is_root || !fname.empty());

        buf.pad_to(8);
        auto off = buf.off();

        buf.write(node_header{.perms = perms,
                              .uid = static_cast<uint16_t>(info.st_uid),
                              .gid = static_cast<uint16_t>(info.st_gid),
                              .name_size = static_cast<uint16_t>(is_root ? 0 : fname.size()),
                              .size = file_size,
                              .atime = static_cast<uint64_t>(info.st_atim.tv_nsec / 1000),
                              .mtime = static_cast<uint64_t>(info.st_mtim.tv_nsec / 1000)});
        if (!is_root)
        {
            buf.write(fname);
        }

        return off;
    }

    auto write_file(out_writer& buf, const fs::directory_entry& dirent) -> size_t
    {
        assert(dirent.is_regular_file());
        assert(dirent.path().has_filename());

        write_con("packing file: {}\n", dirent.path().string());
        auto off = write_header_generic(buf, dirent, dirent.file_size());
        buf.append_file(dirent.path(), dirent.file_size());

        return off;
    }

    auto write_dir(out_writer& buf, const fs::directory_entry& dirent, bool is_root) -> size_t
    {
        assert(dirent.is_directory());
        assert(dirent.path().has_filename());
        write_con("packing dir: {}\n", dirent.path().string());

        std::vector<uint64_t> offsets;
        for (const auto& child : fs::directory_iterator(dirent))
        {
            offsets.push_back(write_dirent(buf, child));
        }

        auto off = write_header_generic(buf, dirent, offsets.size(), is_root);
        buf.pad_to(8);
        buf.write(offsets);
        return off;
    }
} // namespace

auto main(int argc, char** argv) -> int
{
    argparse::ArgumentParser program(argv[0], "1.0");
    program.add_description("stupid file packer");
    program.add_argument("in").help("input dir").default_value(".");
    program.add_argument("-v", "--verbose").help("control output verbosity").default_value(false).implicit_value(true);
    program.add_argument("-o", "--output").required().help("output file").metavar("out").required();

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    verbose = program.get<bool>("-v");
    auto dir = program.get<std::string>("in");
    auto output = program.get<std::string>("-o");

    try
    {
        if (!fs::is_directory(dir))
        {
            std::cerr << fmt::format("file {} isn't a directory\n", dir);
            exit(-1);
        }

        out_writer buf(output);

        auto off = write_dir(buf, fs::directory_entry(fs::canonical(dir)), true);
        buf.pad_to(8);
        buf.write(file_header{
            .magic = MAGIC_NO,
            .ver_major = 0,
            .ver_minor = 1,
            .root_offset = off,
        });
    }
    catch (std::exception& e)
    {
        std::cerr << fmt::format("failed to pack: {}\n", e.what());
        exit(-1);
    }
}


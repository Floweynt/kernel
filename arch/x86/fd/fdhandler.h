#pragma once
#include <common/util/user.h>
#include <cstddef>
#include <cstdint>
#include <memory>

class fd_data;

class fd_operations : public std::simple_refcountable<std::uint32_t> 
{
public:
    [[nodiscard]] auto read(fd_data& instance, user_pointer<const std::uint8_t> buffer, std::size_t count) const -> std::ssize_t;
    [[nodiscard]] auto write(fd_data& instance, user_pointer<std::uint8_t> buffer, std::size_t count) const -> std::ssize_t;
    [[nodiscard]] auto close(fd_data& instance) const -> std::ssize_t;

    enum seek_type 
    {
        SEEK_START,
        SEEK_CURR,
        SEEK_END
    };

    [[nodiscard]] auto seek(fd_data& instance, std::ssize_t offset, seek_type type) const -> std::ssize_t;

};

class fd_data : public std::simple_refcountable<std::uint32_t>
{
    std::refcounted<const fd_operations> operations;
public:

    inline auto read(user_pointer<const std::uint8_t> buffer, std::size_t count) { return operations->read(*this, buffer, count); }
    inline auto write(user_pointer<std::uint8_t> buffer, std::size_t count) { return operations->write(*this, buffer, count); }
    inline auto close() { return operations->close(*this); }
    inline auto seek(std::ssize_t offset, fd_operations::seek_type type) { return operations->seek(*this, offset, type); }
};

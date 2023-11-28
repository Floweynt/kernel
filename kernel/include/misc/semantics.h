#pragma once

// carry more semantics about types

#include <config.h>
#include <cstddef>
#include <cstdint>

struct size_page
{
    std::size_t size;
public:
    inline static constexpr auto PAGE_SIZE = config::get_val<"arch.page_size">;
    [[nodiscard]] auto get() const { return size; }
    [[nodiscard]] auto bytes() const { return size * PAGE_SIZE; }
    [[nodiscard]] operator std::size_t() const { return size; }
};

struct physical_address 
{
    std::uintptr_t pointer;
public:
};

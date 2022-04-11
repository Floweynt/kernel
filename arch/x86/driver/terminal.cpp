#include "terminal.h"
#include <cstring>
#include <pmm/pmm.h>
#include <paging/paging.h>
#include <asm/asm_cpp.h>

namespace driver
{
    simple_tty::simple_tty(const stivale2_struct_tag_framebuffer& buffer, tty::romfont f) : buffer(buffer), f(f)
    {
        if (this->buffer.framebuffer_addr < 0xffff800000000000)
            this->buffer.framebuffer_addr += 0xffff800000000000;
        this->buffer.framebuffer_bpp >>= 3;

        // blank screen
        for (std::size_t i = 0; i < buffer.framebuffer_height; i++)
        {
            char* current_pixel = (char*)pixel_at(x * f.width(), y * f.height() + i);
            for (std::size_t j = 0; j < buffer.framebuffer_height * buffer.framebuffer_bpp; j++)
                current_pixel[j] = 0;
        }

        std::size_t pages = std::detail::div_roundup(cols() * lines() * sizeof(screen_character), 4096ul);
        for(std::size_t i = 0; i < pages; i++)
        {
            auto p = pmm::pmm_allocate();
            paging::request_page(paging::page_type::SMALL, 0xffff900000000000 + i * 4096, (uint64_t)p, 0b00000001);
            invlpg((void*)(0xffff900000000000 + i * 4096));
        }

        screen_buffer = (screen_character*) 0xffff900000000000;
    }

    void simple_tty::scrollup()
    {
        for (std::size_t i = 0; i < cols(); i++)
            screen_buffer[i] = {0, {0, 0, 0}};
        rotate_offset++;
        simple_tty::rerender();
    }

    void simple_tty::rerender()
    {
        tty::rgb save = color;
        for (std::size_t i = 0; i < cols(); i++)
        {
            for (std::size_t j = 0; j < lines(); j++)
            {
                auto [ch, tmp] = char_at(i, j);
                color = tmp;
                render_character(ch, i, j);
            }
        }
        color = save;
    }

    void simple_tty::render_character(char c, std::size_t x, std::size_t y)
    {
        char_at(x, y) = {c, color};
        uint64_t px = (((((1ull << buffer.red_mask_size) - 1) * color.r) / 255) << buffer.red_mask_shift) |
                      (((((1ull << buffer.green_mask_size) - 1) * color.g) / 255) << buffer.green_mask_shift) |
                      (((((1ull << buffer.blue_mask_size) - 1) * color.b) / 255) << buffer.blue_mask_shift);

        unsigned char* current_char = (unsigned char*)f.char_at(c);
        uint8_t bit_index = 0;
        for (std::size_t i = 0; i < f.height(); i++)
        {
            char* current_pixel = (char*)pixel_at(x * f.width(), y * f.height() + i);

            for (std::size_t j = 0; j < f.width(); j++)
            {
                if (*current_char & (0x80 >> bit_index))
                {
                    char* pixel_buffer = (char*)&px;
                    for (int k = 0; k < buffer.framebuffer_bpp; k++)
                        current_pixel[k] = *pixel_buffer++;
                }

                current_pixel += buffer.framebuffer_bpp;
                bit_index++;
                current_char += bit_index / 8;
                bit_index %= 8;
            }
        }
    }

    void simple_tty::putc(char c)
    {
        switch (c)
        {
        case '\0':
            break;
        case '\n':
            y++;
            x = 0;
            break;
        default:
            render_character(c, x, y);
        case ' ':
            x++;
        }

        if (x == cols())
        {
            x = 0;
            y++;
        }

        if (y == lines())
            y--;
    }

    simple_tty::~simple_tty() {}
} // namespace driver

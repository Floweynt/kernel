#include "terminal.h"
#include <asm/asm_cpp.h>
#include <cstring>
#include <mm/pmm.h>
#include <paging/paging.h>
#include <config.h>

namespace driver
{
    simple_tty::simple_tty(const stivale2_struct_tag_framebuffer& buffer, tty::romfont f) : buffer(buffer), f(f)
    {
        this->buffer.framebuffer_addr = mm::make_virtual(this->buffer.framebuffer_addr);
        this->buffer.framebuffer_bpp >>= 3;

        std::size_t pages = std::detail::div_roundup(cols() * lines() * sizeof(screen_character), 4096ul);
        for (std::size_t i = 0; i < pages; i++)
        {
            auto p = mm::pmm_allocate();
            paging::request_page(paging::page_type::SMALL, SCROLLBACK_START + i * 4096, mm::make_physical(p));
        }

        screen_buffer = (screen_character*) SCROLLBACK_START;
    }

    void simple_tty::scrollup()
    {
        for (std::size_t i = 0; i < cols(); i++)
            char_at(i, 0) = {0, {0, 0, 0}};
        rotate_offset++;
        simple_tty::rerender();
    }

    void simple_tty::rerender()
    {
        tty::rgb save_fg = fg_color;
        for (std::size_t i = 0; i < cols(); i++)
        {
            for (std::size_t j = 0; j < lines(); j++)
            {
                auto [ch, tmp] = char_at(i, j);
                fg_color = tmp;
                render_character(ch, i, j);
            }
        }
        fg_color = save_fg;
    }

    void simple_tty::render_character(char c, std::size_t x, std::size_t y)
    {
        uint64_t fg = (((((1ull << buffer.red_mask_size) - 1) * fg_color.r) / 255) << buffer.red_mask_shift) |
                      (((((1ull << buffer.green_mask_size) - 1) * fg_color.g) / 255) << buffer.green_mask_shift) |
                      (((((1ull << buffer.blue_mask_size) - 1) * fg_color.b) / 255) << buffer.blue_mask_shift);
        uint64_t bg = (((((1ull << buffer.red_mask_size) - 1) * bg_color.r) / 255) << buffer.red_mask_shift) |
                      (((((1ull << buffer.green_mask_size) - 1) * bg_color.g) / 255) << buffer.green_mask_shift) |
                      (((((1ull << buffer.blue_mask_size) - 1) * bg_color.b) / 255) << buffer.blue_mask_shift);

        unsigned char* current_char = (unsigned char*)f.char_at(c);
        uint8_t bit_index = 0;
        for (std::size_t i = 0; i < f.height(); i++)
        {
            char* current_pixel = (char*)pixel_at(x * f.width(), y * f.height() + i);

            for (std::size_t j = 0; j < f.width(); j++)
            {
                uint64_t px = *current_char & (0x80 >> bit_index) ? fg : bg;
                char* pixel_buffer = (char*)&px;
                for (int k = 0; k < buffer.framebuffer_bpp; k++)
                    current_pixel[k] = *pixel_buffer++;

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
            char_at(x, y) = {c, fg_color};
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

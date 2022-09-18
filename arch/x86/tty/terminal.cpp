// cSpell:ignore scrollup stivale
#include "romfont.h"
#include <bits/user_implement.h>
#include <cctype>
#include <common/tty.h>
#include <cstring>
#include <kinit/stivale2.h>
#include <mm/pmm.h>
#include <paging/paging.h>

namespace tty
{
    struct screen_character
    {
        char ch;
        tty::rgb color;
    };

    bool is_command = false;
    bool is_text = false;
    char current_command = 0;
    std::uint8_t index = 0;
    std::uint8_t chindex = 0;
    char command_buffer[8][32];
    tty::rgb fg_color = tty::WHITE;
    tty::rgb bg_color = tty::BLACK;
    stivale2_struct_tag_framebuffer buffer;
    tty::romfont f(8, 8, (void*)font);
    screen_character* screen_buffer;
    std::size_t rotate_offset;
    std::size_t x;
    std::size_t y;

    inline void* pixel_at(std::size_t x, std::size_t y)
    {
        return (void*)(buffer.framebuffer_addr + y * buffer.framebuffer_pitch + x * buffer.framebuffer_bpp);
    }

    inline std::size_t lines() { return buffer.framebuffer_height / f.height(); }

    inline std::size_t cols() { return buffer.framebuffer_width / f.width(); }

    inline screen_character& char_at(std::size_t i, std::size_t j)
    {
        j = (rotate_offset + j) % lines();
        return screen_buffer[i * lines() + j];
    }

    void init(const stivale2_struct_tag_framebuffer& buf)
    {
        buffer = buf;
        static constexpr auto SCROLLBACK_START = config::get_val<"mmap.start.scrollback">;
        // Obtains the start of the framebuffer, in a mapped portion of virtual memory
        buffer.framebuffer_addr = mm::make_virtual(buffer.framebuffer_addr);
        // Conver to bytes
        buffer.framebuffer_bpp >>= 3;

        // Obtains the approx amount of pages required to map the character buffer
        std::size_t pages = std::detail::div_roundup(cols() * lines() * sizeof(screen_character), paging::PAGE_SMALL_SIZE);

        // Allocate required virtual memory
        for (std::size_t i = 0; i < pages; i++)
        {
            auto p = mm::pmm_allocate();
            paging::request_page(paging::page_type::SMALL, SCROLLBACK_START + i * paging::PAGE_SMALL_SIZE,
                                 mm::make_physical(p));
        }

        screen_buffer = (screen_character*)SCROLLBACK_START;
    }
    void handle_color1()
    {
        switch (*((std::uint64_t*)command_buffer[0]))
        {
        case 0x72:
        case 0x646572:
            fg_color = tty::RED;
            break;
        case 0x67:
        case 0x6e65657267:
            fg_color = tty::GREEN;
            break;
        case 0x62:
        case 0x65756c62:
            fg_color = tty::BLUE;
            break;
        case 0x42:
        case 0x6b63616c62:
            fg_color = tty::BLACK;
            break;
        case 0x57:
        case 0x6574696877:
            fg_color = tty::WHITE;
            break;
        case 0x79:
        case 0x776f6c6c6579:
            fg_color = tty::YELLOW;
            break;
        case 0x63:
        case 0x6E617963:
            fg_color = tty::CYAN;
            break;
        case 0x47:
        case 0x79657267:
            fg_color = tty::GREY;
            break;
        }
    }

    void render_character(char c, std::size_t x, std::size_t y)
    {
        // obtains the 64 bit integer representing foreground and background colors
        std::uint64_t fg = (((((1ull << buffer.red_mask_size) - 1) * fg_color.r) / 255) << buffer.red_mask_shift) |
                      (((((1ull << buffer.green_mask_size) - 1) * fg_color.g) / 255) << buffer.green_mask_shift) |
                      (((((1ull << buffer.blue_mask_size) - 1) * fg_color.b) / 255) << buffer.blue_mask_shift);
        std::uint64_t bg = (((((1ull << buffer.red_mask_size) - 1) * bg_color.r) / 255) << buffer.red_mask_shift) |
                      (((((1ull << buffer.green_mask_size) - 1) * bg_color.g) / 255) << buffer.green_mask_shift) |
                      (((((1ull << buffer.blue_mask_size) - 1) * bg_color.b) / 255) << buffer.blue_mask_shift);

        unsigned char* current_char = (unsigned char*)f.char_at(c);
        std::uint8_t bit_index = 0;
        for (std::size_t i = 0; i < f.height(); i++)
        {
            char* current_pixel = (char*)pixel_at(x * f.width(), y * f.height() + i);

            for (std::size_t j = 0; j < f.width(); j++)
            {
                std::uint64_t px = *current_char & (0x80 >> bit_index) ? fg : bg;
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

    void putc(char c)
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

        if (x >= cols())
        {
            x = 0;
            y++;
        }

        if (y >= lines())
            y--;
    }

    void handle_rerender()
    {
        // loop thru all characters, and re-print them
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

    void handle_scrollup()
    {
        // Clear the bottom of the character buffer
        for (std::size_t i = 0; i < cols(); i++)
            char_at(i, 0) = {0, {0, 0, 0}};

        // Rotate the screen in order to simulate a scrolldown
        rotate_offset++;
        // Actually display the scrolled up text
        handle_rerender();
    }

    using handler_type = void (*)();
    static constexpr handler_type handlers[26][8] = {
        {},
        {},
        {handle_color1},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {handle_rerender},
        {handle_scrollup},
    };
}; // namespace tty

void std::detail::putc(char c)
{
    using namespace tty;
    if (is_command)
    {
        if (current_command == 0)
            current_command = c;
        else
        {
            if (is_text)
            {
                if (c == '\x1b')
                {
                    fg_color = tty::WHITE;
                    is_command = false;
                    is_text = false;
                    return;
                }

                tty::putc(c);
            }
            else if (c == ';')
            {
                if (std::islower(current_command))
                {
                    handler_type handler = handlers[current_command - 'a'][index];
                    if (handler)
                        handler();
                }
                is_text = true;
            }
            else if (c == ',')
                index++;
            else
            {
                if (chindex < 32 && index < 8)
                    command_buffer[index][chindex++] = c;
            }
        }
    }
    else if (c == '\x1b')
    {
        is_command = true;
        current_command = 0;
        std::memset((void*)command_buffer, 0, sizeof(command_buffer));
        index = 0;
        chindex = 0;
    }
    else
        tty::putc(c);
};

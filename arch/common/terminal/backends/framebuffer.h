#ifndef _TERM_FRAMEBUFFER_H
#define _TERM_FRAMEBUFFER_H

#include "../term.h"

#define FBTERM_FONT_GLYPHS 256

struct fbterm_char
{
    std::uint32_t c;
    std::uint32_t fg;
    std::uint32_t bg;
};

struct fbterm_queue_item
{
    std::size_t x, y;
    struct fbterm_char c;
};

struct fbterm_context
{
    struct term_context term;

    std::size_t font_width;
    std::size_t font_height;
    std::size_t glyph_width;
    std::size_t glyph_height;

    std::size_t font_scale_x;
    std::size_t font_scale_y;

    std::size_t offset_x, offset_y;

    volatile std::uint32_t* framebuffer;
    std::size_t pitch;
    std::size_t width;
    std::size_t height;
    std::size_t bpp;

    std::size_t font_bits_size;
    std::uint8_t* font_bits;
    std::size_t font_bool_size;
    bool* font_bool;

    std::uint32_t ansi_colours[8];
    std::uint32_t ansi_bright_colours[8];
    std::uint32_t default_fg, default_bg;

    std::size_t canvas_size;
    std::uint32_t* canvas;

    std::size_t grid_size;
    std::size_t queue_size;
    std::size_t map_size;

    struct fbterm_char* grid;

    struct fbterm_queue_item* queue;
    std::size_t queue_i;

    struct fbterm_queue_item** map;

    std::uint32_t text_fg;
    std::uint32_t text_bg;
    bool cursor_status;
    std::size_t cursor_x;
    std::size_t cursor_y;

    std::uint32_t saved_state_text_fg;
    std::uint32_t saved_state_text_bg;
    std::size_t saved_state_cursor_x;
    std::size_t saved_state_cursor_y;

    std::size_t old_cursor_x;
    std::size_t old_cursor_y;
};

struct term_context* fbterm_init(std::uint32_t* framebuffer, std::size_t width, std::size_t height, std::size_t pitch,
                                 std::uint32_t* canvas, std::uint32_t* ansi_colours, std::uint32_t* ansi_bright_colours,
                                 std::uint32_t* default_bg, std::uint32_t* default_fg, void* font, std::size_t font_width,
                                 std::size_t font_height, std::size_t font_spacing, std::size_t font_scale_x,
                                 std::size_t font_scale_y, std::size_t margin);

#endif

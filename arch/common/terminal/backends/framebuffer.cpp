#include "framebuffer.h"
#include "../term.h"
#include <config.h>
#include <cstdint>
#include <cstring>
#include <mm/pmm.h>
#include <paging/paging.h>

static void fbterm_save_state(struct term_context* _ctx)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;
    ctx->saved_state_text_fg = ctx->text_fg;
    ctx->saved_state_text_bg = ctx->text_bg;
    ctx->saved_state_cursor_x = ctx->cursor_x;
    ctx->saved_state_cursor_y = ctx->cursor_y;
}

static void fbterm_restore_state(struct term_context* _ctx)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;
    ctx->text_fg = ctx->saved_state_text_fg;
    ctx->text_bg = ctx->saved_state_text_bg;
    ctx->cursor_x = ctx->saved_state_cursor_x;
    ctx->cursor_y = ctx->saved_state_cursor_y;
}

static void fbterm_swap_palette(struct term_context* _ctx)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;
    std::uint32_t tmp = ctx->text_bg;
    ctx->text_bg = ctx->text_fg;
    ctx->text_fg = tmp;
}

static void plot_char(struct term_context* _ctx, struct fbterm_char* c, std::size_t x, std::size_t y)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    if (x >= _ctx->cols || y >= _ctx->rows)
    {
        return;
    }

    x = ctx->offset_x + x * ctx->glyph_width;
    y = ctx->offset_y + y * ctx->glyph_height;

    bool* glyph = &ctx->font_bool[c->c * ctx->font_height * ctx->font_width];
    for (std::size_t gy = 0; gy < ctx->glyph_height; gy++)
    {
        std::uint8_t fy = gy / ctx->font_scale_y;
        volatile std::uint32_t* fb_line = ctx->framebuffer + x + (y + gy) * (ctx->pitch / 4);
        std::uint32_t* canvas_line = ctx->canvas + x + (y + gy) * ctx->width;
        for (std::size_t fx = 0; fx < ctx->font_width; fx++)
        {
            bool draw = glyph[fy * ctx->font_width + fx];
            for (std::size_t i = 0; i < ctx->font_scale_x; i++)
            {
                std::size_t gx = ctx->font_scale_x * fx + i;
                std::uint32_t bg = c->bg == 0xffffffff ? canvas_line[gx] : c->bg;
                std::uint32_t fg = c->fg == 0xffffffff ? canvas_line[gx] : c->fg;
                fb_line[gx] = draw ? fg : bg;
            }
        }
    }
}

static void plot_char_fast(struct term_context* _ctx, struct fbterm_char* old, struct fbterm_char* c, std::size_t x,
                           std::size_t y)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    if (x >= _ctx->cols || y >= _ctx->rows)
    {
        return;
    }

    x = ctx->offset_x + x * ctx->glyph_width;
    y = ctx->offset_y + y * ctx->glyph_height;

    bool* new_glyph = &ctx->font_bool[c->c * ctx->font_height * ctx->font_width];
    bool* old_glyph = &ctx->font_bool[old->c * ctx->font_height * ctx->font_width];
    for (std::size_t gy = 0; gy < ctx->glyph_height; gy++)
    {
        std::uint8_t fy = gy / ctx->font_scale_y;
        volatile std::uint32_t* fb_line = ctx->framebuffer + x + (y + gy) * (ctx->pitch / 4);
        std::uint32_t* canvas_line = ctx->canvas + x + (y + gy) * ctx->width;
        for (std::size_t fx = 0; fx < ctx->font_width; fx++)
        {
            bool old_draw = old_glyph[fy * ctx->font_width + fx];
            bool new_draw = new_glyph[fy * ctx->font_width + fx];
            if (old_draw == new_draw)
                continue;
            for (std::size_t i = 0; i < ctx->font_scale_x; i++)
            {
                std::size_t gx = ctx->font_scale_x * fx + i;
                std::uint32_t bg = c->bg == 0xffffffff ? canvas_line[gx] : c->bg;
                std::uint32_t fg = c->fg == 0xffffffff ? canvas_line[gx] : c->fg;
                fb_line[gx] = new_draw ? fg : bg;
            }
        }
    }
}

static inline bool compare_char(struct fbterm_char* a, struct fbterm_char* b)
{
    return !(a->c != b->c || a->bg != b->bg || a->fg != b->fg);
}

static void push_to_queue(struct term_context* _ctx, struct fbterm_char* c, std::size_t x, std::size_t y)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    if (x >= _ctx->cols || y >= _ctx->rows)
    {
        return;
    }

    std::size_t i = y * _ctx->cols + x;

    struct fbterm_queue_item* q = ctx->map[i];

    if (q == nullptr)
    {
        if (compare_char(&ctx->grid[i], c))
        {
            return;
        }
        q = &ctx->queue[ctx->queue_i++];
        q->x = x;
        q->y = y;
        ctx->map[i] = q;
    }

    q->c = *c;
}

static void fbterm_revscroll(struct term_context* _ctx)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    for (std::size_t i = (_ctx->scroll_bottom_margin - 1) * _ctx->cols - 1;; i--)
    {
        struct fbterm_char* c;
        struct fbterm_queue_item* q = ctx->map[i];
        if (q != nullptr)
        {
            c = &q->c;
        }
        else
        {
            c = &ctx->grid[i];
        }
        push_to_queue(_ctx, c, (i + _ctx->cols) % _ctx->cols, (i + _ctx->cols) / _ctx->cols);
        if (i == _ctx->scroll_top_margin * _ctx->cols)
        {
            break;
        }
    }

    // Clear the first line of the screen.
    struct fbterm_char empty;
    empty.c = ' ';
    empty.fg = ctx->text_fg;
    empty.bg = ctx->text_bg;
    for (std::size_t i = _ctx->scroll_top_margin * _ctx->cols; i < (_ctx->scroll_top_margin + 1) * _ctx->cols; i++)
    {
        push_to_queue(_ctx, &empty, i % _ctx->cols, i / _ctx->cols);
    }
}

static void fbterm_scroll(struct term_context* _ctx)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    for (std::size_t i = (_ctx->scroll_top_margin + 1) * _ctx->cols; i < _ctx->scroll_bottom_margin * _ctx->cols; i++)
    {
        struct fbterm_char* c;
        struct fbterm_queue_item* q = ctx->map[i];
        if (q != nullptr)
        {
            c = &q->c;
        }
        else
        {
            c = &ctx->grid[i];
        }
        push_to_queue(_ctx, c, (i - _ctx->cols) % _ctx->cols, (i - _ctx->cols) / _ctx->cols);
    }

    // Clear the last line of the screen.
    struct fbterm_char empty;
    empty.c = ' ';
    empty.fg = ctx->text_fg;
    empty.bg = ctx->text_bg;
    for (std::size_t i = (_ctx->scroll_bottom_margin - 1) * _ctx->cols; i < _ctx->scroll_bottom_margin * _ctx->cols; i++)
    {
        push_to_queue(_ctx, &empty, i % _ctx->cols, i / _ctx->cols);
    }
}

static void fbterm_clear(struct term_context* _ctx, bool move)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    struct fbterm_char empty;
    empty.c = ' ';
    empty.fg = ctx->text_fg;
    empty.bg = ctx->text_bg;
    for (std::size_t i = 0; i < _ctx->rows * _ctx->cols; i++)
    {
        push_to_queue(_ctx, &empty, i % _ctx->cols, i / _ctx->cols);
    }

    if (move)
    {
        ctx->cursor_x = 0;
        ctx->cursor_y = 0;
    }
}

static void fbterm_enable_cursor(struct term_context* _ctx)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    ctx->cursor_status = true;
}

static bool fbterm_disable_cursor(struct term_context* _ctx)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    bool ret = ctx->cursor_status;
    ctx->cursor_status = false;
    return ret;
}

static void fbterm_set_cursor_pos(struct term_context* _ctx, std::size_t x, std::size_t y)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    if (x >= _ctx->cols)
    {
        if ((int)x < 0)
        {
            x = 0;
        }
        else
        {
            x = _ctx->cols - 1;
        }
    }
    if (y >= _ctx->rows)
    {
        if ((int)y < 0)
        {
            y = 0;
        }
        else
        {
            y = _ctx->rows - 1;
        }
    }
    ctx->cursor_x = x;
    ctx->cursor_y = y;
}

static void fbterm_get_cursor_pos(struct term_context* _ctx, std::size_t* x, std::size_t* y)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    *x = ctx->cursor_x;
    *y = ctx->cursor_y;
}

static void fbterm_move_character(struct term_context* _ctx, std::size_t new_x, std::size_t new_y, std::size_t old_x,
                                  std::size_t old_y)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    if (old_x >= _ctx->cols || old_y >= _ctx->rows || new_x >= _ctx->cols || new_y >= _ctx->rows)
    {
        return;
    }

    std::size_t i = old_x + old_y * _ctx->cols;

    struct fbterm_char* c;
    struct fbterm_queue_item* q = ctx->map[i];
    if (q != nullptr)
    {
        c = &q->c;
    }
    else
    {
        c = &ctx->grid[i];
    }

    push_to_queue(_ctx, c, new_x, new_y);
}

static void fbterm_set_text_fg(struct term_context* _ctx, std::size_t fg)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    ctx->text_fg = ctx->ansi_colours[fg];
}

static void fbterm_set_text_bg(struct term_context* _ctx, std::size_t bg)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    ctx->text_bg = ctx->ansi_colours[bg];
}

static void fbterm_set_text_fg_bright(struct term_context* _ctx, std::size_t fg)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    ctx->text_fg = ctx->ansi_bright_colours[fg];
}

static void fbterm_set_text_bg_bright(struct term_context* _ctx, std::size_t bg)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    ctx->text_bg = ctx->ansi_bright_colours[bg];
}

static void fbterm_set_text_fg_rgb(struct term_context* _ctx, std::uint32_t fg)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    ctx->text_fg = fg;
}

static void fbterm_set_text_bg_rgb(struct term_context* _ctx, std::uint32_t bg)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    ctx->text_bg = bg;
}

static void fbterm_set_text_fg_default(struct term_context* _ctx)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    ctx->text_fg = ctx->default_fg;
}

static void fbterm_set_text_bg_default(struct term_context* _ctx)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    ctx->text_bg = 0xffffffff;
}

static void draw_cursor(struct term_context* _ctx)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    std::size_t i = ctx->cursor_x + ctx->cursor_y * _ctx->cols;
    struct fbterm_char c;
    struct fbterm_queue_item* q = ctx->map[i];
    if (q != nullptr)
    {
        c = q->c;
    }
    else
    {
        c = ctx->grid[i];
    }
    std::uint32_t tmp = c.fg;
    c.fg = c.bg;
    c.bg = tmp;
    plot_char(_ctx, &c, ctx->cursor_x, ctx->cursor_y);
    if (q != nullptr)
    {
        ctx->grid[i] = q->c;
        ctx->map[i] = nullptr;
    }
}

static void fbterm_double_buffer_flush(struct term_context* _ctx)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    if (ctx->cursor_status)
    {
        draw_cursor(_ctx);
    }

    for (std::size_t i = 0; i < ctx->queue_i; i++)
    {
        struct fbterm_queue_item* q = &ctx->queue[i];
        std::size_t offset = q->y * _ctx->cols + q->x;
        if (ctx->map[offset] == nullptr)
        {
            continue;
        }
        struct fbterm_char* old = &ctx->grid[offset];
        if (q->c.bg == old->bg && q->c.fg == old->fg)
        {
            plot_char_fast(_ctx, old, &q->c, q->x, q->y);
        }
        else
        {
            plot_char(_ctx, &q->c, q->x, q->y);
        }
        ctx->grid[offset] = q->c;
        ctx->map[offset] = nullptr;
    }

    if ((ctx->old_cursor_x != ctx->cursor_x || ctx->old_cursor_y != ctx->cursor_y) || ctx->cursor_status == false)
    {
        plot_char(_ctx, &ctx->grid[ctx->old_cursor_x + ctx->old_cursor_y * _ctx->cols], ctx->old_cursor_x,
                  ctx->old_cursor_y);
    }

    ctx->old_cursor_x = ctx->cursor_x;
    ctx->old_cursor_y = ctx->cursor_y;

    ctx->queue_i = 0;
}

static void fbterm_raw_putchar(struct term_context* _ctx, std::uint8_t c)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    struct fbterm_char ch;
    ch.c = c;
    ch.fg = ctx->text_fg;
    ch.bg = ctx->text_bg;
    push_to_queue(_ctx, &ch, ctx->cursor_x++, ctx->cursor_y);
    if (ctx->cursor_x == _ctx->cols && (ctx->cursor_y < _ctx->scroll_bottom_margin - 1 || _ctx->scroll_enabled))
    {
        ctx->cursor_x = 0;
        ctx->cursor_y++;
    }
    if (ctx->cursor_y == _ctx->scroll_bottom_margin)
    {
        ctx->cursor_y--;
        fbterm_scroll(_ctx);
    }
}

static void fbterm_full_refresh(struct term_context* _ctx)
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    for (std::size_t y = 0; y < ctx->height; y++)
    {
        for (std::size_t x = 0; x < ctx->width; x++)
        {
            ctx->framebuffer[y * (ctx->pitch / sizeof(std::uint32_t)) + x] = ctx->canvas[y * ctx->width + x];
        }
    }

    for (std::size_t i = 0; i < (std::size_t)_ctx->rows * _ctx->cols; i++)
    {
        std::size_t x = i % _ctx->cols;
        std::size_t y = i / _ctx->cols;

        plot_char(_ctx, &ctx->grid[i], x, y);
    }

    if (ctx->cursor_status)
    {
        draw_cursor(_ctx);
    }
}

static void fbterm_deinit(struct term_context* _ctx, void (*_free)(void*, std::size_t))
{
    struct fbterm_context* ctx = (fbterm_context*)_ctx;

    _free(ctx->font_bits, ctx->font_bits_size);
    _free(ctx->font_bool, ctx->font_bool_size);
    _free(ctx->grid, ctx->grid_size);
    _free(ctx->queue, ctx->queue_size);
    _free(ctx->map, ctx->map_size);
    _free(ctx->canvas, ctx->canvas_size);
    _free(ctx, sizeof(struct fbterm_context));
}

struct term_context* fbterm_init(std::uint32_t* framebuffer, std::size_t width, std::size_t height, std::size_t pitch,
                                 std::uint32_t* canvas, std::uint32_t* ansi_colours, std::uint32_t* ansi_bright_colours,
                                 std::uint32_t* default_bg, std::uint32_t* default_fg, void* font, std::size_t font_width,
                                 std::size_t font_height, std::size_t font_spacing, std::size_t font_scale_x,
                                 std::size_t font_scale_y, std::size_t margin)
{
    struct fbterm_context* ctx = new fbterm_context;

    struct term_context* _ctx = (term_context*)ctx;

    std::memset(ctx, 0, sizeof(struct fbterm_context));

    ctx->cursor_status = true;

    if (ansi_colours != NULL)
    {
        std::memcpy(ctx->ansi_colours, ansi_colours, sizeof(ctx->ansi_colours));
    }
    else
    {
        ctx->ansi_colours[0] = 0x00000000; // black
        ctx->ansi_colours[1] = 0x00aa0000; // red
        ctx->ansi_colours[2] = 0x0000aa00; // green
        ctx->ansi_colours[3] = 0x00aa5500; // brown
        ctx->ansi_colours[4] = 0x000000aa; // blue
        ctx->ansi_colours[5] = 0x00aa00aa; // magenta
        ctx->ansi_colours[6] = 0x0000aaaa; // cyan
        ctx->ansi_colours[7] = 0x00aaaaaa; // grey
    }

    if (ansi_bright_colours != NULL)
    {
        std::memcpy(ctx->ansi_bright_colours, ansi_bright_colours, sizeof(ctx->ansi_bright_colours));
    }
    else
    {
        ctx->ansi_bright_colours[0] = 0x00555555; // black
        ctx->ansi_bright_colours[1] = 0x00ff5555; // red
        ctx->ansi_bright_colours[2] = 0x0055ff55; // green
        ctx->ansi_bright_colours[3] = 0x00ffff55; // brown
        ctx->ansi_bright_colours[4] = 0x005555ff; // blue
        ctx->ansi_bright_colours[5] = 0x00ff55ff; // magenta
        ctx->ansi_bright_colours[6] = 0x0055ffff; // cyan
        ctx->ansi_bright_colours[7] = 0x00ffffff; // grey
    }

    if (default_bg != NULL)
    {
        ctx->default_bg = *default_bg;
    }
    else
    {
        ctx->default_bg = 0x00000000; // background (black)
    }

    if (default_fg != NULL)
    {
        ctx->default_fg = *default_fg;
    }
    else
    {
        ctx->default_fg = 0x00aaaaaa; // foreground (grey)
    }

    ctx->text_fg = ctx->default_fg;
    ctx->text_bg = 0xffffffff;

    ctx->framebuffer = (std::uint32_t*)framebuffer;
    ctx->width = width;
    ctx->height = height;
    ctx->pitch = pitch;

#define FONT_BYTES ((font_width * font_height * FBTERM_FONT_GLYPHS) / 8)

    ctx->font_width = font_width;
    ctx->font_height = font_height;
    ctx->font_bits = new std::uint8_t[FONT_BYTES];
    std::memcpy(ctx->font_bits, font, FONT_BYTES);

    ctx->font_bits_size = FONT_BYTES;

#undef FONT_BYTES

    ctx->font_width += font_spacing;

    ctx->font_bool_size = FBTERM_FONT_GLYPHS * font_height * ctx->font_width * sizeof(bool);
    ctx->font_bool = new bool[FBTERM_FONT_GLYPHS * font_height * ctx->font_width];

    for (std::size_t i = 0; i < FBTERM_FONT_GLYPHS; i++)
    {
        std::uint8_t* glyph = &ctx->font_bits[i * font_height];

        for (std::size_t y = 0; y < font_height; y++)
        {
            // NOTE: the characters in VGA fonts are always one byte wide.
            // 9 dot wide fonts have 8 dots and one empty column, except
            // characters 0xC0-0xDF replicate column 9.
            for (std::size_t x = 0; x < 8; x++)
            {
                std::size_t offset = i * font_height * ctx->font_width + y * ctx->font_width + x;

                if ((glyph[y] & (0x80 >> x)))
                {
                    ctx->font_bool[offset] = true;
                }
                else
                {
                    ctx->font_bool[offset] = false;
                }
            }
            // fill columns above 8 like VGA Line Graphics Mode does
            for (std::size_t x = 8; x < ctx->font_width; x++)
            {
                std::size_t offset = i * font_height * ctx->font_width + y * ctx->font_width + x;

                if (i >= 0xc0 && i <= 0xdf)
                {
                    ctx->font_bool[offset] = (glyph[y] & 1);
                }
                else
                {
                    ctx->font_bool[offset] = false;
                }
            }
        }
    }

    ctx->font_scale_x = font_scale_x;
    ctx->font_scale_y = font_scale_y;

    ctx->glyph_width = ctx->font_width * font_scale_x;
    ctx->glyph_height = font_height * font_scale_y;

    _ctx->cols = (ctx->width - margin * 2) / ctx->glyph_width;
    _ctx->rows = (ctx->height - margin * 2) / ctx->glyph_height;

    ctx->offset_x = margin + ((ctx->width - margin * 2) % ctx->glyph_width) / 2;
    ctx->offset_y = margin + ((ctx->height - margin * 2) % ctx->glyph_height) / 2;

    ctx->grid_size = _ctx->rows * _ctx->cols * sizeof(struct fbterm_char);
    ctx->grid = new fbterm_char[_ctx->rows * _ctx->cols];
    for (std::size_t i = 0; i < _ctx->rows * _ctx->cols; i++)
    {
        ctx->grid[i].c = ' ';
        ctx->grid[i].fg = ctx->text_fg;
        ctx->grid[i].bg = ctx->text_bg;
    }

    ctx->queue_size = _ctx->rows * _ctx->cols * sizeof(struct fbterm_queue_item);
    ctx->queue = new fbterm_queue_item[_ctx->rows * _ctx->cols];
    ctx->queue_i = 0;

    ctx->map_size = _ctx->rows * _ctx->cols * sizeof(struct fbterm_queue_item*);
    ctx->map = new fbterm_queue_item*[_ctx->rows * _ctx->cols];

    ctx->canvas_size = ctx->width * ctx->height * sizeof(std::uint32_t);

    static constexpr auto SCROLLBACK_START = config::get_val<"mmap.start.scrollback">;
    std::size_t pages = std::div_roundup(ctx->width * ctx->height * sizeof(std::uint32_t), paging::PAGE_SMALL_SIZE);

    for (std::size_t i = 0; i < pages; i++)
    {
        auto p = mm::pmm_allocate();
        paging::request_page(paging::page_type::SMALL, SCROLLBACK_START + i * paging::PAGE_SMALL_SIZE, mm::make_physical(p));
    }

    ctx->canvas = (std::uint32_t*)SCROLLBACK_START;

    for (std::size_t i = 0; i < ctx->width * ctx->height; i++)
    {
        ctx->canvas[i] = ctx->default_bg;
    }

    _ctx->raw_putchar = fbterm_raw_putchar;
    _ctx->clear = fbterm_clear;
    _ctx->enable_cursor = fbterm_enable_cursor;
    _ctx->disable_cursor = fbterm_disable_cursor;
    _ctx->set_cursor_pos = fbterm_set_cursor_pos;
    _ctx->get_cursor_pos = fbterm_get_cursor_pos;
    _ctx->set_text_fg = fbterm_set_text_fg;
    _ctx->set_text_bg = fbterm_set_text_bg;
    _ctx->set_text_fg_bright = fbterm_set_text_fg_bright;
    _ctx->set_text_bg_bright = fbterm_set_text_bg_bright;
    _ctx->set_text_fg_rgb = fbterm_set_text_fg_rgb;
    _ctx->set_text_bg_rgb = fbterm_set_text_bg_rgb;
    _ctx->set_text_fg_default = fbterm_set_text_fg_default;
    _ctx->set_text_bg_default = fbterm_set_text_bg_default;
    _ctx->move_character = fbterm_move_character;
    _ctx->scroll = fbterm_scroll;
    _ctx->revscroll = fbterm_revscroll;
    _ctx->swap_palette = fbterm_swap_palette;
    _ctx->save_state = fbterm_save_state;
    _ctx->restore_state = fbterm_restore_state;
    _ctx->double_buffer_flush = fbterm_double_buffer_flush;
    _ctx->full_refresh = fbterm_full_refresh;
    _ctx->deinit = fbterm_deinit;

    term_context_reinit(_ctx);

    return _ctx;
}


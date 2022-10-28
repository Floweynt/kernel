#ifndef _TERM_H
#define _TERM_H

#include <cstddef>
#include <cstdint>

#define TERM_MAX_ESC_VALUES 16

#define TERM_CB_DEC 10
#define TERM_CB_BELL 20
#define TERM_CB_PRIVATE_ID 30
#define TERM_CB_STATUS_REPORT 40
#define TERM_CB_POS_REPORT 50
#define TERM_CB_KBD_LEDS 60
#define TERM_CB_MODE 70
#define TERM_CB_LINUX 80

struct term_context
{
    /* internal use */

    std::size_t tab_size;
    bool autoflush;
    bool scroll_enabled;
    bool control_sequence;
    bool csi;
    bool escape;
    bool rrr;
    bool discard_next;
    bool bold;
    bool reverse_video;
    bool dec_private;
    bool insert_mode;
    std::uint8_t g_select;
    std::uint8_t charsets[2];
    std::size_t current_charset;
    std::size_t escape_offset;
    std::size_t esc_values_i;
    std::size_t saved_cursor_x;
    std::size_t saved_cursor_y;
    std::size_t current_primary;
    std::size_t scroll_top_margin;
    std::size_t scroll_bottom_margin;
    std::uint32_t esc_values[TERM_MAX_ESC_VALUES];
    bool saved_state_bold;
    bool saved_state_reverse_video;
    std::size_t saved_state_current_charset;
    std::size_t saved_state_current_primary;

    /* to be set by backend */

    std::size_t rows, cols;
    bool in_bootloader;

    void (*raw_putchar)(struct term_context*, std::uint8_t c);
    void (*clear)(struct term_context*, bool move);
    void (*enable_cursor)(struct term_context*);
    bool (*disable_cursor)(struct term_context*);
    void (*set_cursor_pos)(struct term_context*, std::size_t x, std::size_t y);
    void (*get_cursor_pos)(struct term_context*, std::size_t* x, std::size_t* y);
    void (*set_text_fg)(struct term_context*, std::size_t fg);
    void (*set_text_bg)(struct term_context*, std::size_t bg);
    void (*set_text_fg_bright)(struct term_context*, std::size_t fg);
    void (*set_text_bg_bright)(struct term_context*, std::size_t bg);
    void (*set_text_fg_rgb)(struct term_context*, std::uint32_t fg);
    void (*set_text_bg_rgb)(struct term_context*, std::uint32_t bg);
    void (*set_text_fg_default)(struct term_context*);
    void (*set_text_bg_default)(struct term_context*);
    void (*move_character)(struct term_context*, std::size_t new_x, std::size_t new_y, std::size_t old_x, std::size_t old_y);
    void (*scroll)(struct term_context*);
    void (*revscroll)(struct term_context*);
    void (*swap_palette)(struct term_context*);
    void (*save_state)(struct term_context*);
    void (*restore_state)(struct term_context*);
    void (*double_buffer_flush)(struct term_context*);
    void (*full_refresh)(struct term_context*);
    void (*deinit)(struct term_context*, void (*)(void*, std::size_t));
    void (*callback)(struct term_context*, std::uint64_t, std::uint64_t, std::uint64_t, std::uint64_t);
};

void term_context_reinit(struct term_context* ctx);
void term_putchar(struct term_context* ctx, std::uint8_t c);

#endif

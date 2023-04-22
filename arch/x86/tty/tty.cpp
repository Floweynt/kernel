#include "romfont.h"
#include <common/terminal/backends/framebuffer.h>
#include <common/terminal/term.h>
#include <cstdint>
#include <cstring>
#include <kinit/limine.h>
#include <mm/pmm.h>
#include <paging/paging.h>

[[gnu::used]] volatile static limine_framebuffer_request framebuffer_request{
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
};

namespace tty
{
    // from everblush: https://github.com/everblush/everblush
    static std::uint32_t ansi_colors[8] = {
        0x00131a1c, 0x00e74c4c, 0x006bb05d, 0x00e59e67, 0x005b98a9, 0x00b185db, 0x0051a39f, 0x00c4c4c4,
    };

    static std::uint32_t ansi_bright_colours[8] = {
        0x00343636, 0x00c26f6f, 0x008dc776, 0x00e7ac7e, 0x007ab3c3, 0x00bb84e5, 0x006db0ad, 0x00cccccc,
    };

    static std::uint32_t default_bg = 0x00131a1c;
    static std::uint32_t default_fg = 0x00c5c8c9;
    static term_context* ctx;

    void init()
    {
        auto* fb = framebuffer_request.response->framebuffers[0];
        ctx = fbterm_init(
            +[](std::size_t size) { return std::memset(::operator new(size), 0, size); },
            +[](std::size_t size) {
                static constexpr auto SCROLLBACK_START = config::get_val<"mmap.start.scrollback">;

                std::size_t pages = std::div_roundup(size, paging::PAGE_SMALL_SIZE);

                for (std::size_t i = 0; i < pages; i++)
                {
                    auto* p = mm::pmm_allocate();
                    paging::request_page(paging::page_type::SMALL, SCROLLBACK_START + i * paging::PAGE_SMALL_SIZE, mm::make_physical(p));
                }
                return (void*)SCROLLBACK_START;
            },

            mm::make_virtual<std::uint32_t>((std::uintptr_t)fb->address), fb->width, fb->height, fb->pitch, nullptr, ansi_colors, ansi_bright_colours,
            &default_bg, &default_fg, (void*)font, 8, 8, 0, 1, 1, 0);
        ctx->full_refresh(ctx);
    }
} // namespace tty

void std::detail::putc(char ch)
{
    term_putchar(tty::ctx, ch);
    tty::ctx->double_buffer_flush(tty::ctx);
}

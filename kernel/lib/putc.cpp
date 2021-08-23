#include "memory.h"
#include "putc.h"
#include "video.h"
#include "romfont.h"
//using asni_esc_handler = void (*)(uint8_t&, uint8_t&, uint8_t&, );

// width, x, pos
// height, y, line

static int char_width = 8;
static int char_height = 16;

#define MAX_LINES (VBE_MODE_PTR->height / char_height)
#define MAX_CHARS (VBE_MODE_PTR->width / char_width)

int get_max_chars()
{
	return MAX_CHARS;
}

int get_max_lines()
{
	return MAX_LINES;
}

void render_char(const uint32_t* buffer, char ch, int x, int y)
{
	x = x * char_width;
	y = y * char_height;

	const uint8_t* char_font = romfont_8x16 + ch * 16;
	for(int i = x; i < x + char_width; i++)
	{
		for(int j = y; j < y + char_height; j++)
		{
			bool is_on = char_font[j - y] & (1 << (char_width - i + x - 1));
			*FRAMEBUFFER_AT(i, j) = is_on ? (uint32_t) -1 : 0;
		}
	}
}

void putc(char ch)
{
	static uint8_t line = 0;
	static uint8_t pos = 0;
	static uint8_t fmt = 7;
	switch (ch)
	{
	case '\n':
		line++;
		pos = 0;
		break;
	case '\b':
		pos--;
		break;
	case '\t':
		putc(' ');
		putc(' ');
		putc(' ');
		putc(' ');
		break;
	default:
		render_char(FRAMEBUFFER, ch, pos, line);
		pos++;
	};

	if(pos < 0)
	{
		line--;
		pos = MAX_CHARS - 1;
	}
	else if(pos == MAX_CHARS)
	{
		line++;
		pos = 0;
	}

	if(line < 0)
		line = 0;
	else if(line == MAX_LINES)
	{
		// scroll up
		uint32_t* fb_start = FRAMEBUFFER;
		uint32_t* fb_line_one = FRAMEBUFFER + VBE_MODE_PTR->width * char_height;
		size_t rows_to_copy = VBE_MODE_PTR->width * ((MAX_LINES - 1) * char_height);
		uint32_t* last_line_start = FRAMEBUFFER + rows_to_copy;
		size_t pixels_to_clear = VBE_MODE_PTR->width * VBE_MODE_PTR->height - rows_to_copy;

		memmove(fb_start, fb_line_one, rows_to_copy);
		memset(last_line_start, 0, pixels_to_clear);
		line = MAX_LINES - 1;
	}
}

void putstr(const char* ptr)
{
	while(*ptr)
		putc(*ptr++);
}

extern "C"
{
	void _putchar(char character)
	{
		putc(character);
	}
}

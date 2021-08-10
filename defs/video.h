#ifndef __DEF_VIDEO__
#define __DEF_VIDEO__
#include <cstdint>
#include "common.h"
#pragma pack(push, 1)

struct vbe_info
{
    // 0x0
    char signature[4];
    // 0x4
    uint16_t vbe_version;
    // 0x6 (offset), 0x8 (segment)
    farptr_t oem_string_pointer;
    // 0xA
    uint8_t capabilities[4];
    // 0xE (offset), 0x10 (segment)
    farptr_t video_mode_ptr;
    // 0x12
    uint16_t total_memory;
};

struct vbe_mode_info
{
	uint16_t attributes;
	uint8_t window_a;
	uint8_t window_b;
	uint16_t granularity;
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;
	uint16_t pitch;
	// 0x12
	uint16_t width;
	// 0x14
	uint16_t height;
	// 0x16
	uint8_t w_char;
	// 0x17
	uint8_t y_char;
	// 0x18
	uint8_t planes;
	// 0x19
	uint8_t bpp;			// bits per pixel in this mode
	// 0x1A
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
};

#pragma pack(pop)
#endif

#ifndef __ARCH_X86_KINIT_H__
#define __ARCH_X86_KINIT_H__
#include <cstdint>
#include <type_traits>
#include "utils.h"

struct bootloader_packet
{
    uint64_t loaded_address;
    uint64_t mmap_size;
    uint64_t mmap_ptr;

    uint8_t boot_device;

    uint16_t vbe_version;
    uint16_t total_memory_used;
    uint16_t width;
    uint16_t height;
    uint8_t bpp;

    uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;

    uint32_t framebuffer;
};

bootloader_packet* get_bootloader_packet();
inline uint64_t get_load_pos()
{
    return get_bootloader_packet()->loaded_address;
}

constexpr uint64_t VIRT_LOAD_POSITION = 0xFFFFFFFF80000000;
#endif

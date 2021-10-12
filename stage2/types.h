#include <cstdint>

struct bootloader_packet
{
    uint32_t loaded_address; // DONE
    uint32_t loaded_size;
    uint32_t mmap_size;
    uint32_t mmap_ptr;

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

struct mmap
{
    uint64_t base;
    uint64_t len;
    uint32_t type;
};

struct alignas(8) dap
{
    uint8_t size = 0x10;
    uint8_t zero = 0;
    uint16_t to_read = 1;
    uint16_t offset = 0x6000;
    uint16_t segment = 0;
    uint32_t start = 9;
    uint32_t unused = 0;
};

struct alignas(8) gdtr
{
    uint16_t size;
    uint32_t base;
};

struct alignas(8) gdt_entry
{
    uint16_t a;
    uint16_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t f;
};

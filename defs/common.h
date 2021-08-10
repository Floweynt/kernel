#ifndef __DEF_COMMON__
#define __DEF_COMMON__
#include <cstddef>

#pragma pack(push, 1)
struct farptr_t
{
    uint16_t offset;
    uint16_t segment;

    inline operator void*() 
    {
        return (void*)(segment << 4 + offset);
    }
};

struct real_mode_data
{
    uint8_t has_found_video;        // 0 for not found, 1 for found
    uint16_t id;                    // ID of the video (not used, only for debug)
    uint32_t vbe_info_ptr;          // pointer to a vbe_info
    uint32_t vbe_mode_info_ptr;     // pointer to a vbe_mode_info
}

#define GET_REAL_MODE_PARAMS ((real_mode_data*)0x10000)

#pragma pack(pop)
#endif

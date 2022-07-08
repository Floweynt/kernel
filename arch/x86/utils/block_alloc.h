#ifndef __ARCH_X86_UTILS_BLOCK_ALLOC_H__
#define __ARCH_X86_UTILS_BLOCK_ALLOC_H__
#include <bitmanip.h>
#include <cstddef>
#include <cstdint>

/// \brief Internal helper function for block allocation
/// \param meta_buf The metadata buffer
/// \param buf_size The size, in blocks, of the buffer
/// \param len The length of the target, in blocks
/// \return The start of id
std::size_t block_allocate(uint64_t* meta_buf, std::size_t buf_size, std::size_t len);

/// \brief Internal helper function for block allocation
/// \param meta_buf The metadata buffer
/// \param buf_size The size, in blocks, of the buffer
/// \param len The length of the target, in blocks
/// \param r The index to start at
/// \return The start of id
std::size_t block_allocate(uint64_t* meta_buf, std::size_t buf_size, std::size_t len, std::size_t& r);

/// \brief Internal helper function for block allocation
/// \param meta_buf The metadata buffer
/// \param len The length of buffer to free
/// \param index The start index
void block_free(uint64_t* meta_buf, std::size_t len, std::size_t index);

#endif

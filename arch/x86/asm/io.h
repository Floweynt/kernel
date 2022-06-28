#include <cstdint>

/// \brief Wrapper for the `out` instruction (byte)
/// \param port The I/O port to write to
/// \param val The 8-bit value to write
inline void outb(uint16_t port, uint8_t val) { asm volatile("outb %0, %1" : : "a"(val), "Nd"(port)); }

/// \brief Wrapper for the `out` instruction (word)
/// \param port The I/O port to write to
/// \param val The 16-bit value to write
inline void outw(uint16_t port, uint16_t val) { asm volatile("outw %0, %1" : : "a"(val), "Nd"(port)); }

/// \brief Wrapper for the `out` instruction (dword)
/// \param port The I/O port to write to
/// \param val The 32-bit value to write
inline void outl(uint16_t port, uint32_t val) { asm volatile("outl %0, %1" : : "a"(val), "Nd"(port)); }

/// \brief Wrapper for the `in` instruction (byte)
/// \param port The I/O port to read from
/// \return The 8-bit read
inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/// \brief Wrapper for the `in` instruction (word)
/// \param port The I/O port to read from
/// \return The 16-bit read
inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    asm volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/// \brief Wrapper for the `in` instruction (dword)
/// \param port The I/O port to read from
/// \return The 32-bit read
inline uint32_t inl(uint16_t port)
{
    uint32_t ret;
    asm volatile("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/// \brief Waits for the time it takes to preform 1 I/O port operation
///
inline void io_wait() { outb(0x80, 0); }

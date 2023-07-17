#pragma once

#include <cstdint>
#include <type_traits>

namespace elf
{
    constexpr std::uint32_t PT_NULL = 0;
    constexpr std::uint32_t PT_LOAD = 1;
    constexpr std::uint32_t PT_DYNAMIC = 2;
    constexpr std::uint32_t PT_INTERP = 3;
    constexpr std::uint32_t PT_NOTE = 4;
    constexpr std::uint32_t PT_SHLIB = 5;
    constexpr std::uint32_t PT_PHDR = 6;
    constexpr std::uint32_t PT_TLS = 7;
    constexpr std::uint32_t PT_LOOS = 0x60000000;
    constexpr std::uint32_t PT_HIOS = 0x6fffffff;
    constexpr std::uint32_t PT_LOPROC = 0x70000000;
    constexpr std::uint32_t PT_HIPROC = 0x7fffffff;
    constexpr std::uint32_t PT_GNU_EH_FRAME = (PT_LOOS + 0x474e550);
    constexpr std::uint32_t PT_GNU_STACK = (PT_LOOS + 0x474e551);
    constexpr std::uint32_t PT_GNU_RELRO = (PT_LOOS + 0x474e552);
    constexpr std::uint32_t PT_GNU_PROPERTY = (PT_LOOS + 0x474e553);

    template <typename T>
    struct elf_header
    {
        char magic[4];
        std::uint8_t wordsize;
        std::uint8_t endianness;
        std::uint8_t version;
        std::uint8_t abi;
        std::uint8_t abi_version;
        std::uint8_t pad[7]; // sakyua moment
        std::uint16_t machine;
        std::uint32_t version_ext;
        T entry;
        T phoff;
        T shoff;
        std::uint32_t flags;
        std::uint16_t ehsize;
        std::uint16_t e_phentsize;
        std::uint16_t e_phnum;
        std::uint16_t e_shentsize;
        std::uint16_t e_shnum;
        std::uint16_t e_shstrndx;
    };

    using elf64_header = elf_header<std::uint64_t>;
    using elf32_header = elf_header<std::uint32_t>;

    struct elf32_program_header
    {
        std::uint32_t type;
        std::uint32_t offset;
        std::uint32_t vaddr;
        std::uint32_t paddr;
        std::uint32_t filesz;
        std::uint32_t memsz;
        std::uint32_t flags;
        std::uint32_t align;
    };

    struct elf64_program_header
    {
        std::uint32_t type;
        std::uint32_t flags;
        std::uint64_t offset;
        std::uint64_t vaddr;
        std::uint64_t paddr;
        std::uint64_t filesz;
        std::uint64_t memsz;
        std::uint64_t align;
    };

    template <typename T>
    struct elf_section_header
    {
        std::uint32_t name;
        std::uint32_t type;
        T flags;
        T addr;
        T offset;
        T size;
        std::uint32_t link;
        std::uint32_t info;
        T addralign;
        T entsize;
    };

    using elf32_section_header = elf_section_header<std::uint32_t>;
    using elf64_section_header = elf_section_header<std::uint64_t>;

    struct elf32_dynamic
    {
        std::int32_t tag;
        std::uint32_t val;
    };

    struct elf64_dynamic
    {
        std::int64_t tag;
        std::uint64_t val;
    };
} // namespace elf

#pragma once

#include <cstdint>
#include <type_traits>

namespace elf
{
    inline static constexpr std::uint32_t PT_NULL = 0;
    inline static constexpr std::uint32_t PT_LOAD = 1;
    inline static constexpr std::uint32_t PT_DYNAMIC = 2;
    inline static constexpr std::uint32_t PT_INTERP = 3;
    inline static constexpr std::uint32_t PT_NOTE = 4;
    inline static constexpr std::uint32_t PT_SHLIB = 5;
    inline static constexpr std::uint32_t PT_PHDR = 6;
    inline static constexpr std::uint32_t PT_TLS = 7;
    inline static constexpr std::uint32_t PT_LOOS = 0x60000000;
    inline static constexpr std::uint32_t PT_HIOS = 0x6fffffff;
    inline static constexpr std::uint32_t PT_LOPROC = 0x70000000;
    inline static constexpr std::uint32_t PT_HIPROC = 0x7fffffff;
    inline static constexpr std::uint32_t PT_GNU_EH_FRAME = (PT_LOOS + 0x474e550);
    inline static constexpr std::uint32_t PT_GNU_STACK = (PT_LOOS + 0x474e551);
    inline static constexpr std::uint32_t PT_GNU_RELRO = (PT_LOOS + 0x474e552);
    inline static constexpr std::uint32_t PT_GNU_PROPERTY = (PT_LOOS + 0x474e553);

    inline static constexpr auto EI_CLASS_32 = 1;
    inline static constexpr auto EI_CLASS_64 = 2;

    inline static constexpr auto EI_DATA_LITTLE = 1;
    inline static constexpr auto EI_DATA_BIG = 2;

    inline static constexpr auto EI_VERSION_CURRENT = 1;

    inline static constexpr auto EI_OSABI_SYSV = 0;

    inline static constexpr auto ET_NONE = 0x00;   // Unknown.
    inline static constexpr auto ET_REL = 0x01;    // Relocatable file.
    inline static constexpr auto ET_EXEC = 0x02;   // Executable file.
    inline static constexpr auto ET_DYN = 0x03;    // Shared object.
    inline static constexpr auto ET_CORE = 0x04;   // Core file.
    inline static constexpr auto ET_LOOS = 0xFE00; // Reserved inclusive range. Operating system specific.
    inline static constexpr auto ET_HIOS = 0xFEFF;
    inline static constexpr auto ET_LOPROC = 0xFF00; // Reserved inclusive range. Processor specific.
    inline static constexpr auto ET_HIPROC = 0xFF00;

    inline static constexpr auto ELF_MACH_AMD64 = 0x3E;

    struct elf_ident
    {
        char magic[4];
        std::uint8_t wordsize;
        std::uint8_t endianness;
        std::uint8_t version;
        std::uint8_t abi;
        std::uint8_t abi_version;
        std::uint8_t pad[7];
    };

    template <typename T>
    struct elf_header
    {
        elf_ident ident;
        std::uint16_t type;
        std::uint16_t machine;
        std::uint32_t version;
        T entry;
        T ph_off;
        T sh_off;
        std::uint32_t flags;
        std::uint16_t eh_size;
        std::uint16_t ph_ent_size;
        std::uint16_t ph_num;
        std::uint16_t sh_ent_size;
        std::uint16_t sh_num;
        std::uint16_t sh_str_ndx;
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

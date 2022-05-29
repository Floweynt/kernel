#ifndef __ARCH_COMMON_ELF_ELF_H__
#define __ARCH_COMMON_ELF_ELF_H__
#include <cstdint>
#include <type_traits>

namespace elf
{
    constexpr uint32_t PT_NULL = 0;
    constexpr uint32_t PT_LOAD = 1;
    constexpr uint32_t PT_DYNAMIC = 2;
    constexpr uint32_t PT_INTERP = 3;
    constexpr uint32_t PT_NOTE = 4;
    constexpr uint32_t PT_SHLIB = 5;
    constexpr uint32_t PT_PHDR = 6;
    constexpr uint32_t PT_TLS = 7;
    constexpr uint32_t PT_LOOS = 0x60000000;
    constexpr uint32_t PT_HIOS = 0x6fffffff;
    constexpr uint32_t PT_LOPROC = 0x70000000;
    constexpr uint32_t PT_HIPROC = 0x7fffffff;
    constexpr uint32_t PT_GNU_EH_FRAME = (PT_LOOS + 0x474e550);
    constexpr uint32_t PT_GNU_STACK = (PT_LOOS + 0x474e551);
    constexpr uint32_t PT_GNU_RELRO = (PT_LOOS + 0x474e552);
    constexpr uint32_t PT_GNU_PROPERTY = (PT_LOOS + 0x474e553);

    template <typename T>
    struct elf_header
    {
        char magic[4];
        uint8_t wordsize;
        uint8_t endianness;
        uint8_t version;
        uint8_t abi;
        uint8_t abi_version;
        uint8_t pad[7]; // sakyua moment
        uint16_t machine;
        uint32_t version_ext;
        T entry;
        T phoff;
        T shoff;
        uint32_t flags;
        uint16_t ehsize;
        uint16_t e_phentsize;
        uint16_t e_phnum;
        uint16_t e_shentsize;
        uint16_t e_shnum;
        uint16_t e_shstrndx;
    };

    using elf64_header = elf_header<uint64_t>;
    using elf32_header = elf_header<uint32_t>;

    struct elf32_program_header
    {
        uint32_t type;
        uint32_t offset;
        uint32_t vaddr;
        uint32_t paddr;
        uint32_t filesz;
        uint32_t memsz;
        uint32_t flags;
        uint32_t align;
    };

    struct elf64_program_header
    {
        uint32_t type;
        uint32_t flags;
        uint64_t offset;
        uint64_t vaddr;
        uint64_t paddr;
        uint64_t filesz;
        uint64_t memsz;
        uint64_t align;
    };

    template <typename T>
    struct elf_section_header
    {
        uint32_t name;
        uint32_t type;
        T flags;
        T addr;
        T offset;
        T size;
        uint32_t link;
        uint32_t info;
        T addralign;
        T entsize;
    };

    using elf32_section_header = elf_section_header<uint32_t>;
    using elf64_section_header = elf_section_header<uint64_t>;

    struct elf32_dynamic
    {
        int32_t tag;
        uint32_t val;
    };

    struct elf64_dynamic
    {
        int64_t tag;
        uint64_t val;
    };

} // namespace elf

#endif

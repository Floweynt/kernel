#include <algorithm>
#include <cstdint>
#include <cxxabi.h>
#include <fcntl.h>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <span>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>
#include <vector>

namespace elf
{
    template <typename Iter, typename T>
    class map_ranges
    {
        using container_value_type = decltype(*std::declval<Iter>());

    public:
        using value_type = std::invoke_result_t<T, container_value_type>;
        using reference = std::add_lvalue_reference<value_type>;

        class iterator
        {
            friend class map_ranges;
            iterator(Iter rhs, T callback) : cb(callback), impl(rhs) {}

        public:
            using difference_type = decltype(std::declval<Iter>() - std::declval<Iter>());
            using iterator_category = std::random_access_iterator_tag;
            using value_type = map_ranges::value_type;
            using reference = map_ranges::reference;
            constexpr iterator() : impl(nullptr) {}
            constexpr iterator(const iterator& rhs) = default;
            auto operator+=(difference_type rhs) -> iterator&
            {
                impl += rhs;
                return *this;
            }
            auto operator-=(difference_type rhs) -> iterator&
            {
                impl -= rhs;
                return *this;
            }
            auto operator*() const -> value_type { return cb(*impl); }
            auto operator[](difference_type rhs) const -> value_type { return cb(impl[rhs]); }

            auto operator++() -> iterator&
            {
                ++impl;
                return *this;
            }
            auto operator--() -> iterator&
            {
                --impl;
                return *this;
            }
            iterator operator++(int) const
            {
                iterator tmp(*this);
                ++impl;
                return tmp;
            }
            iterator operator--(int) const
            {
                iterator tmp(*this);
                --impl;
                return tmp;
            }
            auto operator-(const iterator& rhs) const -> difference_type { return impl - rhs.impl; }
            auto operator+(difference_type rhs) const -> iterator { return iterator(impl + rhs, cb); }
            auto operator-(difference_type rhs) const -> iterator { return iterator(impl - rhs, cb); }
            friend auto operator+(difference_type lhs, const iterator& rhs) -> iterator { return Iterator(lhs + rhs.impl); }
            friend auto operator-(difference_type lhs, const iterator& rhs) -> iterator { return Iterator(lhs - rhs.impl); }

            auto operator==(const iterator& rhs) const -> bool { return impl == rhs.impl; }
            auto operator<=>(const iterator& rhs) const -> bool { return impl <=> rhs.impl; }

        private:
            T cb;
            Iter impl;
        };

    private:
        iterator _begin, _end;

    public:
        map_ranges() : _begin(), _end() {}
        map_ranges(Iter beg, Iter end, T mapper) : _begin(beg, mapper), _end(end, mapper) {}

        [[nodiscard]] auto begin() const -> iterator { return _begin; }
        [[nodiscard]] auto end() const -> iterator { return _end; }
        auto operator[](std::size_t index) const -> value_type { return _begin[index]; }
        [[nodiscard]] auto size() const -> std::size_t { return _end - _begin; }
    };

    struct dummy
    {
    };

    struct elf64_header
    {
        char sig[4];
        uint8_t clazz;
        uint8_t endian;
        uint8_t ei_version;
        uint8_t abi;
        uint8_t abi_version;
        uint8_t padding[7];
        uint16_t machine;
        uint32_t version;
        uint64_t entry;
        uint64_t ph_offset;
        uint64_t sh_offset;
        uint32_t flags;
        uint16_t eh_size;
        uint16_t ph_entry_size;
        uint16_t ph_count;
        uint16_t sh_entry_size;
        uint16_t sh_count;
        uint16_t sh_str_index;
    };

    struct elf64_section_header
    {
        uint32_t name_index;
        uint32_t type;
        uint64_t flags;
        uint64_t virtual_addr;
        uint64_t offset;
        uint64_t size;
        uint32_t link;
        uint32_t info;
        uint64_t addr_align;
        uint64_t entry_size;
    };

    struct elf64_sym
    {
        uint32_t name;
        unsigned char info;
        unsigned char other;
        uint16_t shndx;
        uint64_t value;
        uint64_t size;
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

    class castable_buffer
    {
        char* data;

    public:
        constexpr castable_buffer(void* buffer) : data((char*)buffer) {}
        constexpr castable_buffer(const castable_buffer&) = default;
        constexpr castable_buffer(castable_buffer&&) = default;

        template <typename T>
        constexpr auto cast_to(std::size_t index) -> T*
        {
            return (T*)(data + index);
        }

        template <typename T>
        [[nodiscard]] constexpr auto cast_to(std::size_t index) const -> const T*
        {
            return (T*)(data + index);
        }
    };

    template <typename Iter, typename Fn>
    auto find_if_optional(Iter first, Iter end, Fn pred) -> std::optional<typename Iter::value_type>
    {
        auto iter = std::find_if(first, end, pred);
        return iter == end ? std::optional<typename Iter::value_type>(std::nullopt) : *iter;
    }

    template <typename Iter, typename T>
    map_ranges(Iter, Iter, T) -> map_ranges<Iter, T>;

    class elf_file
    {
        template <typename T>
        class section
        {
            const elf64_section_header* header;
            std::string_view sv;
            const void* buf;
            friend class elf_file;

            constexpr section(const elf64_section_header* ptr, const char* str, const void* data) : header(ptr), sv(str), buf(data) {}

        public:
            [[nodiscard]] constexpr auto name() const -> const std::string_view& { return sv; }
            [[nodiscard]] constexpr auto type() const -> uint32_t { return header->type; }
            [[nodiscard]] constexpr auto flags() const -> uint64_t { return header->flags; }
            [[nodiscard]] constexpr auto virtual_addr() const -> uint64_t { return header->virtual_addr; }
            [[nodiscard]] constexpr auto data() const -> const T* { return (const T*)buf; }
            [[nodiscard]] constexpr auto size() const -> uint64_t { return header->size; }
            [[nodiscard]] constexpr auto link() const -> uint32_t { return header->link; }
            [[nodiscard]] constexpr auto info() const -> uint32_t { return header->info; }
            [[nodiscard]] constexpr auto addr_align() const -> uint64_t { return header->addr_align; }
            [[nodiscard]] constexpr auto entry_size() const -> uint64_t { return header->entry_size; }

            template <typename U>
            [[nodiscard]] auto cast() const -> section<U>
            {
                return section<U>(header, sv.data(), buf);
            }
        };

        class symbol
        {
            const elf64_sym* sym;
            std::string_view sv;

        public:
            constexpr symbol(const elf64_sym* ptr, const char* str) : sym(ptr), sv(str) {}

            [[nodiscard]] constexpr auto name() const -> const std::string_view& { return sv; }
            [[nodiscard]] constexpr auto info() const -> uint8_t { return sym->info; }
            [[nodiscard]] constexpr auto other() const -> uint8_t { return sym->other; }
            [[nodiscard]] constexpr auto shndx() const -> uint16_t { return sym->shndx; }
            [[nodiscard]] constexpr auto value() const -> uint64_t { return sym->value; }
            [[nodiscard]] constexpr auto size() const -> uint64_t { return sym->size; }
        };

        class segment
        {
            const elf64_program_header* header;
            const void* file_data;

        public:
            constexpr segment(const elf64_program_header* ptr, const void* file_data) : header(ptr), file_data(file_data) {}

            [[nodiscard]] constexpr auto type() const -> uint32_t { return header->type; };
            [[nodiscard]] constexpr auto flags() const -> uint32_t { return header->flags; };
            [[nodiscard]] constexpr auto data() const -> const void* { return file_data; };
            [[nodiscard]] constexpr auto vaddr() const -> uint64_t { return header->vaddr; };
            [[nodiscard]] constexpr auto paddr() const -> uint64_t { return header->paddr; };
            [[nodiscard]] constexpr auto filesz() const -> uint64_t { return header->filesz; };
            [[nodiscard]] constexpr auto memsz() const -> uint64_t { return header->memsz; };
            [[nodiscard]] constexpr auto align() const -> uint64_t { return header->align; };
        };

        struct section_transformer
        {
            elf_file* that;

            auto operator()(const elf64_section_header& hdr) const -> section<void>
            {
                return {&hdr, that->sh_str + hdr.name_index, that->elf.cast_to<char>(hdr.offset)};
            }
        };

        struct symbol_transformer
        {
            elf_file* that;

            auto operator()(const elf64_sym& hdr) const -> symbol { return {&hdr, that->strtab.value().data() + hdr.name}; }
        };

        struct segment_transformer
        {
            elf_file* that;

            auto operator()(const elf64_program_header hdr) const -> segment { return {&hdr, that->elf.cast_to<void>(hdr.offset)}; }
        };

        using sections_t = map_ranges<elf64_section_header*, section_transformer>;
        using symbols_t = map_ranges<const elf64_sym*, symbol_transformer>;
        using segments_t = map_ranges<const elf64_program_header*, segment_transformer>;

        castable_buffer elf;
        const elf64_header* header;
        elf64_section_header* elf_sections;
        const elf64_section_header* sh_str_tab;
        const char* sh_str;
        sections_t cache_sections;
        segments_t cache_segments;

        std::optional<section<char>> strtab;
        std::optional<symbols_t> symbols;
        auto check_elf_consistency(const elf64_header& header) -> std::optional<std::string>;

    public:
        elf_file(void* buffer);

        [[nodiscard]] constexpr auto sections() const -> const auto& { return cache_sections; }

        template <typename T = void>
        auto section_by_name(const std::string_view& name) -> std::optional<section<T>>
        {
            auto val = find_if_optional(cache_sections.begin(), cache_sections.end(), [&](const auto& ent) { return ent.name() == name; });
            if (val)
            {
                return val.value().template cast<T>();
            }

            return std::nullopt;
            //.transform([](const section<void>& e) { return e.cast<T>(); });
        }

        [[nodiscard]] constexpr auto section_sh_strtab() const -> section<char> { return {sh_str_tab, sh_str + sh_str_tab->name_index, sh_str}; }

        [[nodiscard]] constexpr auto section_strtab() const -> const auto& { return strtab; }
        [[nodiscard]] constexpr auto symbol_table() const -> const auto& { return symbols; }
        [[nodiscard]] constexpr auto segments() const -> const auto& { return cache_segments; }
    };
} // namespace elf

namespace sym
{
    struct entry
    {
        uint64_t address;
        uint64_t size;
        std::string_view buffer;
    };

    class writer
    {
        struct f_hdr
        {
            const std::size_t header = 0x4C6577644F776F21ul;
            const std::size_t n{};
        };

        struct f_entry
        {
            uint64_t address;
            uint64_t start;
            std::size_t index;
        };

        std::vector<f_entry> entries;
        std::vector<char> buffer;
        std::ofstream out;

    public:
        writer(const std::filesystem::path& path) { out.open(path, std::ios_base::trunc | std::ios_base::binary); }

        void write(uint64_t address, uint64_t size, std::string name)
        {
            entries.push_back({address, size, buffer.size()});

            buffer.insert(buffer.end(), name.begin(), name.end());
            buffer.push_back(0);
        }

        void write_to_file()
        {
            f_hdr hdr{.n = entries.size()};
            out.write((char*)&hdr, sizeof(hdr));

            for (const auto& entry : entries)
            {
                auto tmp = entry;
                tmp.index += sizeof(f_hdr) + entries.size() * sizeof(f_entry);
                out.write((char*)&tmp, sizeof(f_entry));
            }

            out.write(buffer.data(), buffer.size());
        }
    };
} // namespace sym

class memory_mapped_file
{
    std::size_t len{};
    void* buffer;

public:
    constexpr memory_mapped_file() : buffer(nullptr) {}
    memory_mapped_file(const std::string& str, std::size_t sz_offset = 0, int flags = PROT_READ) : buffer(nullptr)
    {
        int fd = open(str.c_str(), O_RDWR);
        if (fd == -1)
        {
            buffer = nullptr;
            return;
        }

        struct stat fd_stat
        {
        };
        if (fstat(fd, &fd_stat) == -1)
        {
            buffer = nullptr;
            return;
        }

        if (sz_offset)
        {
            fallocate(fd, 0, 0, fd_stat.st_size + sz_offset);
        }

        buffer = mmap(nullptr, len = (fd_stat.st_size + sz_offset), flags, MAP_SHARED, fd, 0);
        if (!buffer)
        {
            buffer = nullptr;
            return;
        }

        close(fd);
    }

    ~memory_mapped_file() { reset(); }

    void reset()
    {
        if (buffer != nullptr)
        {
            munmap(buffer, len);
        }
    }

    constexpr operator bool() { return buffer != nullptr; }
    constexpr auto is_open() -> bool { return buffer != nullptr; }

    constexpr auto to_span() -> std::span<char> { return {(char*)buffer, len}; }
    constexpr auto data() -> void* { return buffer; }
    [[nodiscard]] constexpr auto size() const -> std::size_t { return len; }
};

constexpr const char* SYM_VIS[]{"STV_DEFAULT", "STV_INTERNAL", "STV_HIDDEN", "STV_PROTECTED"};

constexpr auto SCREEN_LEN = 160;

auto demangle(const std::string_view& str) -> std::string
{
    int status = 0;
    char* name = abi::__cxa_demangle(str.data(), nullptr, nullptr, &status);
    std::string result = (std::string)str;
    switch (status)
    {
    case 0:
        result = name;
        free(name);
        break;
    case -1:
        std::cerr << "fatal: bad_alloc\n";
        exit(-1);
    case -2:
        break;
    case -3:
        std::cerr << "fatal: i am stupid\n";
        exit(-1);
        break;
    default:
        __builtin_unreachable();
    }

    return result;
}

auto demangle_and_restrict_len(const std::string_view& str) -> std::string
{
    std::string result = demangle(str.data());
    if (result.size() > SCREEN_LEN)
    {
        result.resize(SCREEN_LEN - 3);
        result += "...";
    }

    return result;
}

struct symtab_entry
{
    uint64_t start;
    uint64_t size;
    std::string name;
};

struct file_symtab_entry
{
    uint64_t start;
    uint64_t size;
    uint64_t str_index;
};

template <typename T>
void write_type(std::ostream& stream, const T* pointer)
{
    std::copy((char*)pointer, (char*)pointer + sizeof(T), std::ostreambuf_iterator<char>(stream));
}

auto prog_main(int argc, char** argv) -> int
{
    if (argc != 3)
    {
        std::cerr << "usage: " << argv[0] << " [filename] [output]\n";
        exit(-1);
    }

    memory_mapped_file file(argv[1]);

    if (!file)
    {
        perror("mmap_file()");
        exit(-1);
    }

    elf::elf_file elf(file.data());

    if (!elf.section_strtab())
    {
        std::cerr << ".strtab not found, did you strip the executable?\n";
        exit(-1);
    }

    if (!elf.symbol_table())
    {
        std::cerr << ".symtab not found, did you strip the executable?\n";
        exit(-1);
    }

    const auto& symbols = elf.symbol_table();
    std::vector<symtab_entry> entries;

    for (const auto& sym : symbols.value())
    {
        fmt::print("Sym: {:{}} size=0x{:016x} value=0x{:016x} flags={}\n", demangle_and_restrict_len(sym.name()), SCREEN_LEN, sym.size(), sym.value(),
                   SYM_VIS[sym.other() & 3]);

        entries.push_back({sym.value(), sym.size(), demangle(sym.name())});
    }

    std::sort(entries.begin(), entries.end(), [](const symtab_entry& a, symtab_entry& b) { return a.start < b.start; });

    sym::writer w(argv[2]);
    for (const auto& i : entries)
        w.write(i.start, i.size, i.name);
    w.write_to_file();

    memory_mapped_file out_file(argv[2]);
    elf::castable_buffer out_proxy(out_file.data());

    if (!out_file)
    {
        std::cerr << "integrity check failed\n";
        exit(-1);
    }

    std::vector<file_symtab_entry*> check_syms;
    if (*out_proxy.cast_to<std::size_t>(0) != 0x4C6577644F776F21UL)
    {
        std::cerr << "integrity check failed\n";
        exit(-1);
    }

    std::size_t count = *out_proxy.cast_to<std::size_t>(8);
    check_syms.reserve(count);
    for (std::size_t i = 0; i < count; i++)
    {
        check_syms.push_back(out_proxy.cast_to<file_symtab_entry>(sizeof(std::size_t) * 2 + sizeof(file_symtab_entry) * i));
    }

    if (count != entries.size())
    {
        std::cerr << "integrity check failed\n";
        exit(-1);
    }

    std::size_t index = 0;
    for (const auto& entry : entries)
    {
        if (check_syms[index]->size != entry.size || check_syms[index]->start != entry.start ||
            (std::string_view(out_proxy.cast_to<char>(check_syms[index]->str_index)) != entry.name))
        {
            std::cerr << "integrity check failed\n";
            exit(-1);
        }
        index++;
    }

    return 0;
}

auto main(int argc, char** argv) -> int
{
    try
    {
        return prog_main(argc, argv);
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << '\n';
        exit(-1);
    }
}

namespace elf
{
    auto elf_file::check_elf_consistency(const elf64_header& header) -> std::optional<std::string>
    {
        if (std::memcmp(header.sig,
                        "\x7f"
                        "ELF",
                        4) != 0)
        {
            return "bad elf signature";
        }
        if (header.clazz != 2)
        {
            return "elf not 64 bit";
        }
        if (header.version != 1 || header.ei_version != 1)
        {
            return "elf not version 1";
        }
        if (header.sh_entry_size != sizeof(elf64_section_header))
        {
            return "invalid section header size";
        }
        return std::nullopt;
    }

    elf_file::elf_file(void* buffer) : elf(buffer), header(elf.cast_to<elf64_header>(0))
    {

        auto result = check_elf_consistency(*header);
        if (result)
        {
            throw std::runtime_error(result.value());
        }

        elf_sections = (elf.cast_to<elf64_section_header>(header->sh_offset));

        sh_str_tab = &elf_sections[header->sh_str_index];
        sh_str = elf.cast_to<const char>(sh_str_tab->offset);

        cache_sections = map_ranges(elf_sections, elf_sections + header->sh_count, section_transformer{this});
        const auto* value = elf.cast_to<elf64_program_header>(header->ph_offset);
        cache_segments = map_ranges(value, value + header->ph_count, segment_transformer{this});

        strtab = section_by_name<char>(".strtab");

        if (!strtab)
        {
            return;
        }

        auto symtab = section_by_name(".symtab");

        if (!symtab)
        {
            symbols = std::nullopt;
            return;
        }

        auto sym_data = symtab.value().cast<elf64_sym>();
        symbols = symbols_t(sym_data.data(), sym_data.data() + sym_data.size() / sizeof(elf64_sym), symbol_transformer{this});

        //.transform([&](const section<void>& e) {
        //    auto s = e.cast<elf64_sym>();
        //    return symbols_t(s.data(), s.data() + s.size() / sizeof(elf64_sym), symbol_transformer{this});
        //});
    }
} // namespace elf

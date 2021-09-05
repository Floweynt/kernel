#ifndef __ARCH_X86_PAGING_ENTRIES_H__
#define __ARCH_X86_PAGING_ENTRIES_H__

#include "utils/utils.h"
#include <cstdint>

namespace paging
{
    template <typename subclass, uint8_t ptr_start, uint8_t ptr_end>
	class page_table_base
	{
    protected:
		uint64_t data;
	public:
        inline page_table_base() : data(0) { }
        // Bitflag format:
        // 0 - R/W bit
        // 1 - U/S bit
        // 2 - PWT
        // 3 - PCD
        // 4 - EXEC 
        // 5-7 - unused, use 0
		page_table_base(uint8_t flags, uint64_t ptr)
        {
            data = 0;
            set_bit<0>(data, true); // sets present flag
            set_bits<1, 4>(data, get_bits<0, 3>(flags) << 1); // sets the R/W, U/S, PWT, PCD
            set_bit<63>(data, get_bit<4>(flags)); // sets exec flag
            set_ptr(ptr);
        }

        // getters + setters for non-cpu output bits
        inline subclass& set_write(bool can_write) { set_bit<1>(data, can_write); return static_cast<subclass&>(*this); }
        inline subclass& set_supervisor(bool need_supervisor) { set_bit<2>(data, need_supervisor); return static_cast<subclass&>(*this); }
        inline subclass& set_pwt(bool pwt) { set_bit<3>(data, pwt); return static_cast<subclass&>(*this); }
        inline subclass& set_cache_disable(bool disable_cache) { set_bit<4>(data, disable_cache); return static_cast<subclass&>(*this); }
        inline subclass& set_exec(bool can_exec) { set_bit<63>(data, can_exec); return static_cast<subclass&>(*this); }

        inline bool get_write() { return get_bit<1>(data); }
        inline bool get_supervisor() { return get_bit<2>(data); }
        inline bool get_pwt() { return get_bit<3>(data); }
        inline bool get_cache_disable() { return get_bit<4>(data); }
        inline bool get_exec() { return get_bit<63>(data); }

        // get/set ptr
        inline subclass& set_ptr(uint64_t ptr) { set_bits<ptr_start, ptr_end>(data, ptr); return static_cast<subclass&>(*this); }
        inline uint64_t get_ptr() { return get_bits<ptr_start, ptr_end>(data); }

        // get cpu output bits
        inline bool get_accessed() { return get_bit<5>(data); }
	};

    class page_table_entry : public page_table_base<page_table_entry, 12, 51>
    {
        using base_class_t = page_table_base<page_table_entry, 12, 51>;
    public:
        inline page_table_entry() : page_table_base() { }
        inline page_table_entry(uint8_t flags, uint64_t ptr_to_next) : base_class_t(flags, ptr_to_next) { }
    };

    class page_small : public page_table_base<page_small, 12, 51>
    {
        using base_class_t = page_table_base<page_small, 12, 51>;
    public:
        // Bitflag format:
        // 0 - R/W bit
        // 1 - U/S bit
        // 2 - PWT
        // 3 - PCD
        // 4 - EXEC
        // 5 - G
        // 6 - PAT
        // 7 - unused, use 0
        inline page_small(uint8_t flags, uint8_t prot_key, uint64_t ptr_to_mem) : base_class_t(flags, ptr_to_mem)
        {
            set_bit<7>(data, get_bit<6>(flags)); // sets PAT flag
            set_bit<8>(data, get_bit<5>(flags)); // sets global flag
            set_key(prot_key);
        }

        // get/set prot key
        inline uint8_t get_key() { return get_bits<59, 62>(data) >> 59; }
        inline page_small& set_key(uint8_t key) { set_bits<59, 62>(data, (uint64_t)key << 59); return *this; }

        // getters + setters for non-cpu output bits
        inline page_small& set_global(bool global) { set_bit<8>(data, global); return *this; }
        inline page_small& set_pat(bool pat) { set_bit<7>(data, pat); return *this; }

        inline bool get_global() { return get_bit<8>(data); }
        inline bool get_pat() { return get_bit<7>(data); }

        // get cpu output bits
        inline bool get_dirty() { return get_bit<6>(data); }
    };

    class page_medium : public page_table_base<page_medium, 21, 51>
    {
        using base_class_t = page_table_base<page_medium, 21, 51>;
	public:
        // Bitflag format:
        // 0 - R/W bit
        // 1 - U/S bit
        // 2 - PWT
        // 3 - PCD
        // 4 - EXEC
        // 5 - G
        // 6 - PAT
        // 7 - unused, use 0
		inline page_medium(uint8_t flags, uint8_t prot_key, uint64_t ptr_to_mem) : base_class_t(flags, ptr_to_mem)
        {
            set_bit<7>(data, true); // sets page size flag
            set_bit<8>(data, get_bit<5>(flags)); // sets global flag
            set_bit<12>(data, get_bit<6>(flags));
            set_key(prot_key);
        }

        // get/set prot key
        inline uint8_t get_key() { return get_bits<59, 62>(data) >> 59; }
        inline page_medium& set_key(uint8_t key) { set_bits<59, 62>(data, (uint64_t)key << 59); return *this; }

        // getters + setters for non-cpu output bits
        inline page_medium& set_global(bool global) { set_bit<8>(data, global); return *this; }
        inline page_medium& set_pat(bool pat) { set_bit<12>(data, pat); return *this; }

        inline bool get_global() { return get_bit<8>(data); }
        inline bool get_pat() { return get_bit<12>(data); }

        // get cpu output bits
        inline bool get_dirty() { return get_bit<6>(data); }
    };

    class page_large : public page_table_base<page_large, 30, 51>
    {

    };

    static_assert(
        sizeof(page_table_entry) == sizeof(uint64_t) &&
        sizeof(page_table_entry) == sizeof(page_small) &&
        sizeof(page_table_entry) == sizeof(page_medium) &&
        sizeof(page_table_entry) == sizeof(page_large),
        "size of page table entries are wrong!"
    );
};

#endif

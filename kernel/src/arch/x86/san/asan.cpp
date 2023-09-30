// cSpell:ignore kasan
#include <cstddef>
#include <cstdint>
#include <klog/klog.h>
#include <misc/cast.h>
#include <misc/kassert.h>
#include <mm/paging/paging.h>
#include <tty/tty.h>

namespace san
{
    namespace
    {
        inline constexpr int KASAN_SHIFT = 3;
        inline constexpr std::uintptr_t KASAN_SHADOW_DELTA = 0xdfffe00000000000;
        inline constexpr bool KASAN_IS_DEBUG = false;

        inline constexpr std::size_t KASAN_SCALE = std::size_t{1} << KASAN_SHIFT;

        auto kasan_get_shadow(void* ptr) -> std::int8_t* { return as_ptr(KASAN_SHADOW_DELTA + (as_uptr(ptr) >> KASAN_SHIFT)); }
        auto kasan_get_pointer(std::int8_t* shadow) -> void* { return as_vptr(((as_uptr(shadow) - KASAN_SHADOW_DELTA) << KASAN_SHIFT)); }

        constexpr auto is_pointer_aligned(void* pointer) -> bool { return (as_uptr(pointer) & (KASAN_SCALE - 1)) == 0u; }

        inline constexpr std::int8_t KASAN_UNACCESSIBLE = static_cast<std::int8_t>(0xff);
        inline constexpr std::int8_t KASAN_FULLY_ADDRESSABLE = 0;
        constexpr auto get_partial_for(std::uintptr_t ptr) -> std::int8_t { return ptr & (KASAN_SCALE - 1); }
    } // namespace

    [[gnu::no_sanitize_address]] void kasan_unpoison(void* pointer, std::size_t size)
    {
        expect(is_pointer_aligned(pointer), "kasan: unaligned pointer");
        auto* shadow = kasan_get_shadow(pointer);

        if constexpr (KASAN_IS_DEBUG)
        {
            klog::log("kasan: un-poisoning memory at 0x%016llx, size: 0x%016llx", as_uptr(pointer), size);
        }

        for (std::size_t i = 0; i < (size >> KASAN_SHIFT); ++i)
        {
            expect(shadow[i] == KASAN_UNACCESSIBLE, "kasan: double un-poision");
            shadow[i] = KASAN_FULLY_ADDRESSABLE;
        }

        if (size & (KASAN_SCALE - 1))
        {
            expect(shadow[size >> KASAN_SHIFT] == KASAN_FULLY_ADDRESSABLE, "kasan: double un-poision");
            shadow[size >> KASAN_SHIFT] = get_partial_for(size);
        }
    }

    [[gnu::no_sanitize_address]] void kasan_poison(void* pointer, std::size_t size)
    {
        expect(is_pointer_aligned(pointer), "kasan: unaligned pointer");
        auto* shadow = kasan_get_shadow(pointer);
        if constexpr (KASAN_IS_DEBUG)
        {
            klog::log("kasan: poisoning memory at 0x%016llx, size: 0x%016llx", as_uptr(pointer), size);
        }

        for (std::size_t i = 0; i < (size >> KASAN_SHIFT); ++i)
        {
            expect(shadow[i] == KASAN_FULLY_ADDRESSABLE, "kasan: double-poision");
            shadow[i] = KASAN_UNACCESSIBLE;
        }
        if (size & (KASAN_SCALE - 1))
        {
            expect(shadow[size >> KASAN_SHIFT] == get_partial_for(size), "kasan: double-poision");
            shadow[size >> KASAN_SHIFT] = KASAN_UNACCESSIBLE;
        }
    }

    [[gnu::no_sanitize_address]] void kasan_clean(void* pointer, std::size_t size)
    {
        expect(is_pointer_aligned(pointer), "kasan: unaligned pointer");

        if constexpr (KASAN_IS_DEBUG)
        {
            klog::log("kasan: cleaning memory at 0x%016llx, size: 0x%016llx", as_uptr(pointer), size);
        }

        auto* shadow = kasan_get_shadow(pointer);

        for (std::size_t i = 0; i < (size >> KASAN_SHIFT); ++i)
        {
            shadow[i] = KASAN_FULLY_ADDRESSABLE;
        }
        if (size & (KASAN_SCALE - 1))
        {
            shadow[size >> KASAN_SHIFT] = get_partial_for(size);
        }
    }

    [[gnu::no_sanitize_address]] void kasan_validate_clean(void* pointer, std::size_t size)
    {
        expect(is_pointer_aligned(pointer), "kasan: unaligned pointer");

        auto* shadow = kasan_get_shadow(pointer);
        for (std::size_t i = 0; i < (size >> KASAN_SHIFT); ++i)
        {
            expect(shadow[i] == KASAN_FULLY_ADDRESSABLE, "bad kasan clean");
        }
    }

    /*
        void scrubStackFrom(std::uintptr_t top, Continuation cont)
        {
            auto bottom = reinterpret_cast<std::uintptr_t>(cont.sp);
            assert(top >= bottom);
            cleanKasanShadow(cont.sp, top - bottom);
            // Perform some sanity checking.
            validateKasanClean(reinterpret_cast<void*>(bottom & ~(kPageSize - 1)), bottom & (kPageSize - 1));
        }*/
} // namespace san

/*
extern "C" void __asan_alloca_poison(std::uintptr_t address, std::size_t size)
{
    (void)address;
    (void)size;
    // TODO: Implement alloca poisoning.
}

extern "C" void __asan_allocas_unpoison(void* stackTop, void* stackBottom)
{
    (void)stackTop;
    (void)stackBottom;
    // TODO: Implement alloca poisoning.
}*/

namespace
{
    void asan_hook_start()
    {
        static lock::spinlock lock;
        lock.lock();
        klog::log("====================== " RED("asan") " ======================");
        klog::log("a runtime kernel error has occurred that cannot be recovered from due to a memory access error");
        klog::log("now why did I mess up?");
        // dont release since ASAN is always fatal and should hang the system
    }

    [[gnu::no_sanitize_address]] [[gnu::always_inline]] void kasan_report(bool write, std::uintptr_t address, std::size_t size, void* ip)
    {
        asan_hook_start();
        klog::log("kasan failure: %llu-byte %s 0x%016llx", size, write ? "write to" : "read from", address);
        auto* shadow = san::kasan_get_shadow(as_vptr(address));
        auto shadow_low = as_uptr(shadow) & 15;
        auto valid_behind = (as_uptr(shadow) - shadow_low) & (paging::PAGE_SIZE - 1);
        auto valid_after = paging::PAGE_SMALL_SIZE - valid_behind;
        auto shown_behind = std::min(valid_behind, std::size_t{2 * 16});
        auto shown_after = std::min(valid_after, std::size_t{2 * 16});

        std::ptrdiff_t behind = -static_cast<std::ptrdiff_t>(shown_behind);

        while (behind < static_cast<std::ptrdiff_t>(16 + shown_after))
        {
            klog::log_many([&]() {
                klog::print("kasan: shadow[0x%016llx]:", san::kasan_get_pointer(&shadow[-shadow_low + behind]));
                for (std::size_t j = 0; j < 16; ++j)
                {
                    auto tag = static_cast<std::uint8_t>(shadow[-shadow_low + behind]);
                    const auto* surround = behind == shadow_low ? "[" : " ";
                    klog::print("%s%s%02x%s", surround, tag <= 8 ? "0" : "", tag, surround);
                    ++behind;
                }
            });
        }

        klog::panic("asan failure");
    
} // namespace

extern "C" auto __asan_address_is_poisoned(void* addr) -> int
{
    auto uptr = as_uptr(addr);
    auto* shadow = san::kasan_get_shadow(addr);

    if (config::get_val<"mmap.start.heap"> <= uptr && uptr < (config::get_val<"mmap.start.heap"> + config::get_val<"mmap.size.heap">))
    {
        return *shadow == san::KASAN_FULLY_ADDRESSABLE ? 0 : 1;
    }

    return 0;
}

extern "C" [[gnu::no_sanitize_address]] void __asan_set_shadow_00(void* pointer, std::size_t size)
{
    auto* ptr = as_ptr<std::int8_t>(pointer);
    for (std::size_t i = 0; i < size; ++i)
    {
        ptr[i] = 0;
    }
}

extern "C" void __asan_report_load1_noabort(std::uintptr_t address) { kasan_report(false, address, 1, __builtin_return_address(0)); }
extern "C" void __asan_report_load2_noabort(std::uintptr_t address) { kasan_report(false, address, 2, __builtin_return_address(0)); }
extern "C" void __asan_report_load4_noabort(std::uintptr_t address) { kasan_report(false, address, 4, __builtin_return_address(0)); }
extern "C" void __asan_report_load8_noabort(std::uintptr_t address) { kasan_report(false, address, 8, __builtin_return_address(0)); }
extern "C" void __asan_report_load16_noabort(std::uintptr_t address) { kasan_report(false, address, 16, __builtin_return_address(0)); }
extern "C" void __asan_report_load_n_noabort(std::uintptr_t address, std::size_t size)
{
    kasan_report(false, address, size, __builtin_return_address(0));
}

extern "C" void __asan_report_store1_noabort(std::uintptr_t address) { kasan_report(true, address, 1, __builtin_return_address(0)); }
extern "C" void __asan_report_store2_noabort(std::uintptr_t address) { kasan_report(true, address, 2, __builtin_return_address(0)); }
extern "C" void __asan_report_store4_noabort(std::uintptr_t address) { kasan_report(true, address, 4, __builtin_return_address(0)); }
extern "C" void __asan_report_store8_noabort(std::uintptr_t address) { kasan_report(true, address, 8, __builtin_return_address(0)); }
extern "C" void __asan_report_store16_noabort(std::uintptr_t address) { kasan_report(true, address, 16, __builtin_return_address(0)); }
extern "C" void __asan_report_store_n_noabort(std::uintptr_t address, std::size_t size)
{
    kasan_report(false, address, size, __builtin_return_address(0));
}

extern "C" void __asan_handle_no_return()
{
    // Do nothing (Linux leaves this empty as well).
}


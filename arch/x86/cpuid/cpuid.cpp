// cpuid support
#include "cpuid.h"
#include <array>
#include <asm/asm_cpp.h>
#include <config.h>
#include <cstddef>

namespace cpuid_info
{
    namespace
    {
        std::uint32_t cpuid_max;
        std::array<std::uint32_t, config::get_val<"cpuid-feature-size">> features;
        std::array<std::uint32_t, 6> vendor_buf;
        std::array<std::uint32_t, 13> brand_buf;
    } // namespace

    void initialize_cpuglobal()
    {
        cpuid(0, &cpuid_max, vendor_buf.data(), vendor_buf.data() + 2, vendor_buf.data() + 1);
        cpuid(1, nullptr, nullptr, features.data(), features.data() + 1);
        for (int i = 0; i < 3; i++)
        {
            cpuid(0x80000002 + i, brand_buf.data() + static_cast<std::ptrdiff_t>(i * 4), brand_buf.data() + static_cast<std::ptrdiff_t>(i * 4 + 1),
                  brand_buf.data() + static_cast<std::ptrdiff_t>(i * 4 + 2), brand_buf.data() + static_cast<std::ptrdiff_t>(i * 4 + 3));
        }
    };

    auto cpu_vendor_string() -> const char*
    {
        return (const char*)vendor_buf.data();
    }

    auto cpu_brand_string() -> const char*
    {
        return (const char*)brand_buf.data();
    }

    auto test_feature(std::size_t feature) -> bool { return (features.at(feature / 32) & (1 << (feature % 32))) != 0U; }
} // namespace cpuid_info

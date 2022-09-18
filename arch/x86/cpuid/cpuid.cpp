// cpuid support
#include "cpuid.h"
#include <asm/asm_cpp.h>
#include <config.h>

namespace cpuid_info
{
    static std::uint32_t cpuid_max;
    static std::uint32_t features[config::get_val<"cpuid-feature-size">];
    static std::uint32_t vendor_buf[3];
    static std::uint32_t brand_buf[12];

    void initialize_cpuglobal()
    {
        cpuid(0, &cpuid_max, vendor_buf, vendor_buf + 2, vendor_buf + 1);
        cpuid(1, nullptr, nullptr, features, features + 1);
        std::size_t n = cpuid_ext(0, features + 2, features + 3, features + 4);
        // for(size_t = 1; i < n && i < ; i++)
        for (int i = 0; i < 3; i++)
            cpuid(0x80000002 + i, brand_buf + i * 4, brand_buf + i * 4 + 1, brand_buf + i * 4 + 2, brand_buf + i * 4 + 3);
    };

    const char* cpu_vendor_string()
    {
        static bool is_init = false;
        static std::uint32_t buf[3];

        if (!is_init)
        {
            cpuid(0, nullptr, buf, buf + 2, buf + 1);
            is_init = true;
        }
        return (const char*)buf;
    }

    const char* cpu_brand_string()
    {
        static bool is_init = false;
        static std::uint32_t buf[12];

        if (!is_init)
        {
            for (int i = 0; i < 3; i++)
                cpuid(0x80000002 + i, buf + i * 4, buf + i * 4 + 1, buf + i * 4 + 2, buf + i * 4 + 3);
            is_init = true;
        }
        return (const char*)buf;
    }

    bool test_feature(std::size_t feature) { return features[feature / 32] & (1 << (feature % 32)); }
} // namespace cpuid_info

#ifndef __X86_GDT_H__
#define __X86_GDT_H__
#include <cstdint>
namespace gdt
{
    /// \brief generates and installs a GDT
    ///
    void install_gdt();
} // namespace gdt

#endif

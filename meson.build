project('kernel', 'cpp')

kernel_gen = find_program('gen_binary.sh')
kernel_gen = find_program('gen_binary.sh')

add_project_arguments(
    '-fno-rtti',
    '-fno-exceptions',
    '-Os',
    '-ffreestanding',
    '-fno-PIC',
    '-no-pie',
    '-fno-exceptions',
    '-Wno-ignored-attributes',
    '-Wall',
    '-fcf-protection=none',
    '-I' + (meson.source_root() / 'kernel/'),
    '-I' + (meson.source_root() / 'kernel/arch/x86/'),
    '-I' + (meson.source_root() / 'kernel/arch/'),
    '-fno-threadsafe-statics',
    '-mno-sse',
    '-mno-sse2',
    '-mno-mmx',
    '-mno-80387',
    language: 'cpp'
)

add_project_arguments(
    '-Os',
    '-ffreestanding',
    '-fno-PIC',
    language: 'c'
)

kernel_source = [
    'kernel/arch/x86/idt/idt.cpp',
    'kernel/arch/x86/apic/apic.cpp',
    'kernel/arch/x86/kinit/kinit.cpp',
    'kernel/arch/x86/gdt/gdt.cpp',
    'kernel/arch/x86/paging/paging.cpp',
    'kernel/arch/x86/driver/kinit/serial.cpp',
    'kernel/kernel.cpp',
    'lib/cxxabi.cpp',
    'kernel/arch/x86/idt/idt.S',
    'kernel/arch/x86/kinit/kinit.S'
]

kernel = executable('kernel.bin',
    kernel_source,
    link_args: [
        '-T' + (meson.source_root() / 'kernel/kernel.lds'),
        '-static',
        '-nostdlib',
        '-lgcc',
        '-m64',
    ],
    cpp_args: ['-m64', '-march=x86-64', '-mcmodel=kernel'],
    c_args: ['-m64', '-march=x86-64', '-mcmodel=kernel']
)

kernel_debug = executable('kernel.o',
    kernel_source,
    link_args: [
        '-T' + (meson.source_root() / 'debug.lds'),
        '-static',
        '-nostdlib',
        '-lgcc',
        '-m64',
    ],
    cpp_args: ['-m64', '-march=x86-64', '-mcmodel=kernel', '-g'],
    c_args: ['-m64', '-march=x86-64', '-mcmodel=kernel', '-g']
)



project('kernel', 'cpp')

add_project_arguments(
    '-fno-rtti',
    '-fno-exceptions',
    '-Os',
    '-ffreestanding',
    '-fno-PIC',
    '-no-pie',
    '-fno-exceptions',
    '-Wno-ignored-attributes',
    '-std=c++17',
    '-Wall',
    '-fcf-protection=none',
    '-I' + (meson.source_root() + '/kernel/'),
    '-I' + (meson.source_root() + '/kernel/arch/x86/'),
    '-I' + (meson.source_root() + '/kernel/arch/'),
    '-I' + (meson.source_root() + '/lib/'),
    '-I' + (meson.source_root() + '/lib/include/'),
    '-fno-threadsafe-statics',
    '-mno-sse',
    '-mno-sse2',
    '-nostdinc',
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
    'lib/src/cstring.cpp',
    'lib/src/new.cpp',
    'kernel/arch/x86/idt/idt.cpp',
    'kernel/arch/x86/apic/apic.cpp',
    'kernel/arch/x86/kinit/kinit.cpp',
    'kernel/arch/x86/gdt/gdt.cpp',
    'kernel/arch/x86/paging/paging.cpp',
    'kernel/arch/interface/driver/tty.cpp',
    'kernel/kernel.cpp',
    'kernel/cxxabi.cpp',
    'kernel/arch/x86/idt/idt.S'
]

# config
conf = configuration_data()
conf.set('VERSION', '"0.0.1"')
conf.set('ARCH', '"x86_64"')
conf.set('ARCH_PATH', '"x86"')
conf.set('MALLOC_IMPL_PATH', '"memory-allocator/linked-list-memory-allocator/alloc.h"')
configure_file(input : 'kernel/config.h.in',
               output : 'config.h',
               configuration : conf)
configuration_inc = include_directories('.')


kernel = executable('kernel.elf',
    kernel_source,
    link_args: [
        '-T' + (meson.source_root() + '/kernel/kernel.lds'),
        '-static',
        '-nostdlib',
        '-lgcc',
        '-m64',
    ],
    cpp_args: ['-m64', '-march=x86-64', '-mcmodel=kernel'],
    c_args: ['-m64', '-march=x86-64', '-mcmodel=kernel'],
    include_directories : configuration_inc
)

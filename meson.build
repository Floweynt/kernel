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
    '-I' + (meson.source_root() + '/'),
    '-I' + (meson.source_root() + '/arch/x86/'),
    '-I' + (meson.source_root() + '/arch/'),
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
    './arch/x86/idt/idt.cpp',
    './arch/x86/apic/apic.cpp',
    './arch/x86/kinit/kinit.cpp',
    './arch/x86/gdt/gdt.cpp',
    './arch/x86/paging/paging.cpp',
    './arch/x86/pmm/pmm.cpp',
    './arch/common/driver/tty.cpp',
    './lib/src/cstring.cpp',
    './lib/src/new.cpp',
    './lib/src/backtrace.cpp',
    './lib/cxxabi.cpp',
    './libimpl/memory-allocator/linked-list-memory-allocator/alloc.cpp',
    './libimpl/cxxabi.cpp',
    './libimpl/implement.cpp',
    './lib/src/panic.cpp',
    'arch/x86/idt/idt.S',
]

# config
conf = configuration_data()
conf.set('VERSION', '"0.0.1"')
conf.set('ARCH', '"x86_64"')
conf.set('ARCH_PATH', '"x86"')
conf.set('MALLOC_IMPL_PATH', '"memory-allocator/linked-list-memory-allocator/alloc.h"')
configure_file(input : 'config.h.in',
               output : 'config.h',
               configuration : conf)
configuration_inc = include_directories('.')

kernel = executable('kernel.elf',
    kernel_source,
    link_args: [
        '-T' + (meson.source_root() + '/kernel.lds'),
        '-static',
        '-nostdlib',
        '-lgcc',
        '-m64',
    ],
    cpp_args: ['-m64', '-march=x86-64', '-mcmodel=kernel'],
    c_args: ['-m64', '-march=x86-64', '-mcmodel=kernel'],
    include_directories : configuration_inc
)

# required program for generating disk image
qemu = find_program('qemu-system-x86_64')
generate_image = find_program('./build-resources/generate_image.sh')

kernel_img = custom_target('kernel_img',
    output: 'kernel.img',
    input: kernel,
    depends: [kernel],
    command: [
        generate_image, 
        meson.build_root() + 'kernel.img', 
        meson.source_root() + './build-resources/kernel-template.img',
        meson.build_root() + 'kernel.elf'
    ]
)

run = run_target('run',
    depends: [kernel_img],
    command: [qemu, '-bios', '/usr/share/ovmf/x64/OVMF.fd', '-hda', meson.build_root() + 'kernel.img']
)

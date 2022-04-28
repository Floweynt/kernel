project('kernel', 'c', 'cpp', default_options: ['c_std=c11', 'cpp_std=c++17', 'warning_level=3'])

add_project_arguments(
    '-fno-exceptions',
    '-fno-PIC',
    '-fcf-protection=none',
    '-fno-rtti',
    '-fno-stack-protector',
    '-fno-threadsafe-statics',
    '-ffreestanding',
    '-no-pie',
    '-nostdinc',
    '-Wno-ignored-attributes',
    '-I' + (meson.source_root() + '/'),
    '-I' + (meson.source_root() + '/arch/x86/'),
    '-I' + (meson.source_root() + '/arch/'),
    '-I' + (meson.source_root() + '/lib/'),
    '-I' + (meson.source_root() + '/lib/include/'),
    '-mno-sse',
    '-mno-sse2',
    '-mno-mmx',
    '-mno-80387',
    language: 'cpp'
)

add_project_arguments(
    '-ffreestanding',
    '-fno-PIC',
    language: 'c'
)

kernel_source = [
    'arch/common/driver/tty.cpp',
    'arch/x86/idt/idt.cpp',
    'arch/x86/idt/handlers/div_by_zero.cpp',
    'arch/x86/idt/handlers/handle_gp.cpp',
    'arch/x86/idt/handlers/handle_timer.cpp',
    'arch/x86/apic/apic.cpp',
    'arch/x86/kinit/kinit.cpp',
    'arch/x86/process/process.cpp',
    'arch/x86/process/scheduler/scheduler.cpp',
    'arch/x86/mm/pmm.cpp',
    'arch/x86/mm/mm.cpp',
    'arch/x86/gdt/gdt.cpp',
    'arch/x86/paging/paging.cpp',
    'arch/x86/paging/pgfault.cpp',
    'arch/x86/smp/smp.cpp',
    'arch/x86/driver/terminal.cpp',
    'lib/src/cstring.cpp',
    'lib/src/panic.cpp',
    'lib/src/new.cpp',
    'lib/src/backtrace.cpp',
    'libimpl/memory-allocator/linked-list-memory-allocator/extend_alloc.cpp',
    'libimpl/cxxabi.cpp',
    'libimpl/implement.cpp',
    'arch/x86/idt/idt.S'
]

# config
conf = configuration_data()
conf.set('VERSION', '"0.0.1"')
conf.set('ARCH', '"x86_64"')
conf.set('ARCH_PATH', '"x86"')
conf.set('MALLOC_IMPL_PATH', '<libimpl/memory-allocator/linked-list-memory-allocator/alloc.h>')
conf.set('PMM_COUNT', '0x20')
conf.set('PRE_ALLOCATE_PAGES', '100')


conf.set('HHDM_START', '0xffff800000000000')
conf.set('SCROLLBACK_START', '0xffff900000000000')
conf.set('HEAP_START', '0xffff900080000000')

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
gdb = find_program('gdb')
objcopy = find_program('objcopy')
qemu = find_program('qemu-system-x86_64')
generate_image = find_program('./build-resources/generate_image.sh')

kernel_stripped = custom_target('kernel_stripped',
    output: 'kernel_stripped.elf',
    input: kernel,
    depends:  [kernel],
    command: [
        objcopy,
        '--strip-debug', 
        meson.build_root() + '/kernel.elf',
        meson.build_root() + '/kernel_stripped.elf'
    ]
)

kernel_dbg = custom_target('kernel_dbg',
    output: 'kernel.dbg',
    input: kernel,
    depends:  [kernel],
    command: [
        objcopy,
        '--only-keep-debug',
        meson.build_root() + '/kernel.elf',
        meson.build_root() + '/kernel.dbg'
    ]
)


kernel_img = custom_target('kernel_img',
    output: 'kernel.img',
    input: kernel_stripped,
    depends: [kernel_stripped],
    command: [
        generate_image, 
        meson.build_root() + '/kernel.img', 
        meson.source_root() + '/build-resources/kernel-template.img',
        meson.build_root() + '/kernel_stripped.elf'
    ]
)

run = run_target('run',
    depends: [kernel_img],
    command: [
        qemu, 
        '-bios', 
        '/usr/share/ovmf/x64/OVMF.fd',
        '-hda', 
        meson.build_root() + '/kernel.img',
        '-no-reboot',
        '-monitor',
        'stdio',
        '-d',
        'int',
        '-D',
        'qemu.log' ,
        '-no-shutdown',
        '-s',
        '-S',
        '-M',
        'smm=off',
        '-smp', '2'
    ]
)

attach_debugger = run_target('attach',
    depends: [kernel_dbg],
    command: [
        gdb,
        meson.build_root() + '/kernel.dbg',
        '-ex', 'target remote localhost:1234',
        '-ex', 'b _start', 
        '-ex', 'c',
        '-ex', 'tui enable'
    ]
)

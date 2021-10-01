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
    '-I' + (meson.source_root() / 'kernel/arch/interface/'),
    language: 'cpp'
)

add_project_arguments(
    '-Os',
    '-ffreestanding',
    '-fno-PIC',
    language: 'c'
)

c = run_command('glob_match_source.sh')
sources = c.stdout().strip().split('\n')

kernel = executable('kernel.bin',
    sources,
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
    sources,
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

boot = executable('boot.bin',
    'boot/boot.S',
    link_args: [
        '-T' + (meson.source_root() / 'boot/boot.lds'),
        '-static',
        '-nostdlib',
        '-m16'
    ],
    c_args: ['-m16', '-march=i386']
)

stage2 = executable('stage2.bin',
    'stage2/main.cpp',
    link_args: [
        '-T' + (meson.source_root() / 'stage2/link.lds'),
        '-static',
        '-nostdlib',
        '-m16'
    ],
    c_args: ['-m16', '-march=i386'],
    cpp_args: ['-m16', '-march=i386']
)

kernelimg = custom_target(
    'kernel',
    output : 'kernel.img',
    input : [boot, stage2, kernel],
    command : [kernel_gen, '@BUILD_ROOT@', '@INPUT0@', '@INPUT1@', '@INPUT2@', '@OUTPUT@']
)

bochs = find_program('bochs')
run = run_target(
    'run',
    depends : [kernelimg],
    command : [bochs]
)

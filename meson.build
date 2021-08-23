project('kernel', 'cpp')

kernel_gen = find_program('gen_binary.sh')

add_project_arguments(
    '-fno-rtti',
    '-fno-exceptions',
    '-lc',
    '-march=i386',
    '-Os',
    '-ffreestanding',
    '-fno-PIC',
    '-fno-exceptions',
    '-fcf-protection=none',
    '-I' + (meson.source_root() / 'kernel/'),
    '-I' + (meson.source_root() / 'kernel/arch/x86/'),
    language: 'cpp'
)

add_project_arguments(
    '-march=i386',
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
        '-lgcc'
	  ],
    cpp_args: '-m32',
    c_args: '-m32'
)

boot = executable('boot.bin',
    'boot/boot.S',
    link_args: [
		    '-T' + (meson.source_root() / 'boot/boot.lds'),
		    '-static',
        '-nostdlib',
	  ],
    c_args: '-m16'
)

stage2 = executable('stage2.bin',
    'stage2/main.cpp',
    link_args: [
		    '-T' + (meson.source_root() / 'stage2/link.lds'),
		    '-static',
        '-nostdlib',
	  ],
    c_args: '-m16',
    cpp_args: '-m16'
)

kernelimg = custom_target(
    'kernel',
    output : 'kernel.img',
    input : [boot, stage2, kernel],
    command : [kernel_gen, '@BUILD_ROOT@', '@INPUT0@', '@INPUT1@', '@INPUT2@', '@OUTPUT@']
)

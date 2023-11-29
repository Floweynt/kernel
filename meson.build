project('kernel', 'c', 'cpp', version: '0.0.1', default_options: [
    'c_std=c11', 
    'cpp_std=c++20', 
    'warning_level=2',
    'cpp_rtti=false',
    'cpp_eh=none',
])

build_resources = meson.source_root()/'resources'

# required program for generating the kernel
gdb = find_program('gdb')
objcopy = find_program('objcopy')
qemu = find_program('qemu-system-x86_64')

gen_dbg = executable('gen-dbg',
    build_resources/'gen_dbg.cpp',
    cpp_args: [
    ],
    dependencies: dependency('fmt'),
    override_options: [
        'cpp_eh=default'
    ]
)

strip_debug =       find_program(build_resources/'strip_debug.sh')
generate_image =    find_program(build_resources/'generate_image.sh')
git_version_info =  find_program(build_resources/'git_version_info.sh')
mcopy =             find_program('mcopy')
parted =            find_program('parted')

kernel_src = [

    'kernel/src/arch/x86/idt/idt.cpp',
    'kernel/src/arch/x86/idt/handlers/handle_ud.cpp',
    'kernel/src/arch/x86/idt/handlers/handle_np.cpp',
    'kernel/src/arch/x86/idt/handlers/handle_page_fault.cpp',
    'kernel/src/arch/x86/idt/handlers/handle_gp.cpp',
    'kernel/src/arch/x86/idt/handlers/handle_overflow.cpp',
    'kernel/src/arch/x86/idt/handlers/handle_db.cpp',
    'kernel/src/arch/x86/idt/handlers/handle_div_by_zero.cpp',
    'kernel/src/arch/x86/idt/handlers/handle_stack.cpp',
    'kernel/src/arch/x86/idt/handlers/handle_double_fault.cpp',
    'kernel/src/arch/x86/idt/handlers/handle_bad_tss.cpp',
    'kernel/src/arch/x86/idt/handlers/handle_bounds.cpp',
    'kernel/src/arch/x86/idt/handlers/handle_breakpoint.cpp',
    'kernel/src/arch/x86/idt/handlers/handle_timer.cpp',
    'kernel/src/arch/x86/idt/handlers/handle_nm.cpp',
    'kernel/src/arch/x86/idt/idt.S',
    'kernel/src/arch/x86/debug/kinit_dump.cpp',
    'kernel/src/arch/x86/debug/panic.cpp',
    'kernel/src/arch/x86/debug/symbols.cpp',
    'kernel/src/arch/x86/asm/return_to_context.cpp',
    'kernel/src/arch/x86/asm/return_to_context.S',
    'kernel/src/arch/x86/pci/pci_scan.cpp',
    'kernel/src/arch/x86/sync/spinlock.cpp',
    'kernel/src/arch/x86/apic/apic.cpp',
    'kernel/src/arch/x86/kinit/kinit.cpp',
    'kernel/src/arch/x86/process/process.cpp',
    'kernel/src/arch/x86/process/save_ctx_for_reschedule.S',
    'kernel/src/arch/x86/process/scheduler/scheduler.cpp',
    'kernel/src/arch/x86/klog/klog.cpp',
    'kernel/src/arch/x86/tty/tty.cpp',
    'kernel/src/arch/x86/mm/pmm.cpp',
    'kernel/src/arch/x86/mm/malloc.cpp',
    'kernel/src/arch/x86/mm/init.cpp',
    #    'kernel/src/arch/x86/mm/slab.cpp',
    'kernel/src/arch/x86/mm/paging/paging.cpp',
    'kernel/src/arch/x86/mm/vmm.cpp',
    'kernel/src/arch/x86/cpuid/cpuid.cpp',
    'kernel/src/arch/x86/gdt/gdt.cpp',
    #  'kernel/src/arch/x86/acpi/lai.cpp',
    'kernel/src/arch/x86/user/fd/console.cpp',
    'kernel/src/arch/x86/user/syscall/syscall_entry.S',
    'kernel/src/arch/x86/user/elf_load.cpp',
    'kernel/src/arch/x86/drivers/keyboard/ps.cpp',    
    'kernel/src/arch/x86/smp/smp.cpp',
    'kernel/src/arch/x86/smp/tls.cpp',
    'kernel/external/terminal/limine_term_ccompat.cpp',
    'kernel/src/arch/x86/fs/vfs.cpp',
    'kernel/src/arch/x86/fs/impl/fpk.cpp'
]

lib_src = [
    'lib/src/cstring.cpp',
    'lib/src/new.cpp',
    'lib/src/backtrace.cpp',
    'lib/src/printf.cpp',
    'libimpl/cxxabi.cpp',
    'libimpl/implement.cpp',
]

common_src = [
    'kernel/external/terminal/backends/framebuffer.c',
    'kernel/external/terminal/term.c',
    'kernel/external/terminal/limine_term_ccompat.cpp',
]

opt_to_config = {
    'PRE_ALLOCATE_PAGES': 'preallocate_pages',
    'KLOG_PAGES': 'klog_size_pages',
    'CPUID_FEATURE_SIZE': 'cpuid_feature_buffer_size',
    'DEBUG_LOG_MMAP': 'debug_log_mmap',
    'DEBUG_LOG_CPUID': 'debug_log_cpuid',
    'DEBUG_LOG_ACPI': 'debug_log_acpi',
    'DEBUG_LOG_PCI': 'debug_log_pci',
    'DEBUG_SPINLOCK_DEP': 'debug_spinlock_dep',
    'SLAB_MIN_ORDER': 'slab_min_order',
    'SLAB_MAX_ORDER': 'slab_max_order',
    'SLAB_SIZE_ORDER': 'slab_size_order',
    'VMM_SIZE': 'vmm_size',
    'HAS_ASAN': 'kernel_asan',
    'HAS_UBSAN': 'kernel_ubsan'
}

qemu_base_flags = [
    '-bios', build_resources/'OVMF.fd',         # Make sure this path points to **your** ovmf
    '-hda', meson.build_root()/'kernel.img',    # Kernel image to run
    '-no-reboot',                               # Prevent rebooting on exception
    '-monitor', 'stdio',                        # Monitor output to stdio
    '-d', 'int,cpu_reset',                      # Generate interrupt logging
    '-D', 'qemu.log',                           # QEMU logging output
    '-no-shutdown',                             
    '-s',                                       # Enable debugging port (same as -gdb tcp::1234)
    '-S',                                       # Freeze CPU on start
    '-M', 'smm=off'                             # No SMM interrupts
]

execution_targets = {
    'emu': {
        'exec': {
            'smp':      ['-smp', '2', '-m', '8G'],
            'very-smp': ['-smp', '8', '-m', '8G'],
            'nosmp':    ['-smp', '1', '-m', '8G'],
        },
        'debugger': {
            'gef': [
                'gef-remote --qemu-user --qemu-binary kernel_stripped.elf localhost 1234',
                'b _start',
                'c',
            ],
            '@default': [
                'target remote localhost:1234',
                'b _start',
                'c',
            ]
        }
    },
    'kvm': {
        'exec': {
            'smp':      ['-enable-kvm', '-cpu', 'host', '-smp', '2', '-m', '8G'],
            'very-smp': ['-enable-kvm', '-cpu', 'host', '-smp', '8', '-m', '8G'],
            'nosmp':    ['-enable-kvm', '-cpu', 'host', '-smp', '1', '-m', '8G']
        },
        'debugger': {
            'gef': [
                'gef-remote --qemu-user --qemu-binary kernel_stripped.elf localhost 1234',
                'hbreak _start',
                'c',
            ],
            '@default': [
                'target remote localhost:1234',
                'hbreak _start',
                'c',
            ]
        }
    },
}

kernel_derivatives = {
    'kernel_stripped': {
        'name': 'kernel_stripped.elf',
        'args': [ strip_debug ]
    },
    'kernel_dbg': {
        'name': 'kernel_dbg.elf',
        'args': [ objcopy, '--only-keep-debug' ]
    },
    'kernel_syms': {
        'name': 'kernel.syms',
        'args': [ gen_dbg ]
    }
}

kernel_build_args = [
    '-ffreestanding',
    '-fno-PIC',
    '-fno-exceptions',
    '-fno-omit-frame-pointer',
    '-fno-strict-aliasing',
    '-fcf-protection=none',
    '-femulated-tls',
    '-fno-stack-protector',
    '-fno-threadsafe-statics',
    '-mno-red-zone',
    '-nostdinc',
    '-Wno-ignored-attributes',
    '-mno-sse',
    '-mno-sse2',
    '-mno-mmx',
    '-mno-80387',
    '-Wno-unused-parameter',
]

ubsan = get_option('kernel_ubsan')
asan = get_option('kernel_asan')

ubsan_extra = ['-fno-sanitize=function', '-fsanitize-undefined-strip-path-components=1']
asan_extra = []

ubsan_src = 'kernel/src/arch/x86/san/ubsan.cpp'
asan_src = 'kernel/src/arch/x86/san/asan.cpp'

if ubsan and asan 
    kernel_build_args += ['-fsanitize=kernel-address,undefined'] + ubsan_extra + asan_extra
    kernel_src += [ubsan_src, asan_src] 
elif ubsan 
    kernel_build_args += ['-fsanitize=undefined'] + ubsan_extra
    kernel_src += [ubsan_src]
elif asan 
    kernel_build_args += ['-fsanitize=kernel-address'] + asan_extra
    kernel_src += [asan_src]
endif

# options configuration
if get_option('slab_min_order') >= get_option('slab_max_order')
    error('config: slab_min_order cannot >= slab_max_order')
endif

if get_option('slab_max_order') >= get_option('slab_size_order')
    error('config: slab_max_order cannot >= slab_size_order')
endif

if get_option('preallocate_pages') < 0x500
    warning('config: pre-allocating more than 0x500 pages is recommended')
endif

# config
conf = configuration_data()
conf.set('VERSION', meson.project_version())
conf.set('ARCH_NAME', 'x86_64')
conf.set('ARCH_PATH', 'x86')
conf.set('ARCH_PAGE_SIZE', '4096')
conf.set('COMPILER_C_ID', meson.get_compiler('c').get_id())
conf.set('COMPILER_C_VER', meson.get_compiler('c').version())
conf.set('COMPILER_CXX_ID', meson.get_compiler('cpp').get_id())
conf.set('COMPILER_CXX_VER', meson.get_compiler('cpp').version())

foreach key, opt : opt_to_config
    conf.set(key, get_option(opt).to_string())
endforeach

conf.set('HHDM_START',       '0xffff800000000000')
conf.set('PFN_START',        '0xffff900000000000')
conf.set('SCROLLBACK_START', '0xffff910000000000')
conf.set('HEAP_START',       '0xffff920000000000')
conf.set('SLAB_START',       '0xffff930000000000')
conf.set('KLOG_START',       '0xffff940000000000')
conf.set('VMM_START',        '0xffffa00000000000')
conf.set('HEAP_SIZE',        '0x10000000000')
conf.set('KLOG_SHIFT',       '17')

comp_id = meson.get_compiler('cpp').get_id()

if comp_id == 'gcc'
    conf.set('COMPILER_GCC', true)
    conf.set('COMPILER_CLANG', false)
elif comp_id == 'clang'
    conf.set('COMPILER_GCC', false)
    conf.set('COMPILER_CLANG', true)
else 
    error('unknown cc', comp_id)
endif

configure_file(input : 'config.h.in',
               output : 'config.h.in',
               configuration : conf)

configuration_inc = include_directories(
    '.',
    'kernel/include/arch/x86',
    'kernel/include',
    'kernel/external',
    'kernel/external/lai',
    'lib',
    'lib/include',
    is_system: true
)

config_output = custom_target('config_output',
    input: 'config.h.in',
    output: 'config.h',
    command: [
        git_version_info, 
        meson.build_root()/'config.h.in', 
        meson.build_root()/'config.h'
    ],
)

lds = meson.source_root()/'kernel/src/arch/x86/kernel.lds'
kernel = executable('kernel.elf',
    config_output,
    [kernel_src, lib_src, common_src],
    link_depends: lds,
    link_args: [
        '-T' + lds,
        '-static',
        '-nostdlib',
        '-m64',
        '-femulated-tls'
    ],
    cpp_args: [
        '-m64', 
        '-march=x86-64', 
        '-mcmodel=kernel',
        kernel_build_args
    ],
    c_args: [
        '-m64', 
        '-march=x86-64', 
        '-mcmodel=kernel',
        kernel_build_args
    ],
    include_directories : configuration_inc
)

foreach name, args : kernel_derivatives 
    set_variable(name.replace('-', '_'), custom_target(name,
        output: args.get('name'),
        input: kernel,
        command: [
            args.get('args'),
            '@INPUT@',
            '@OUTPUT@'
        ]
    ))
endforeach

kernel_img = custom_target('kernel-img',
    output: 'kernel.img',
    input: [ kernel_stripped, kernel_syms ],
    command: [
        generate_image, 
        '@OUTPUT@', 
        build_resources/'kernel-template.img',
        '@INPUT0@',
        '@INPUT1@'
    ]
)

qemu_targets = {}

foreach type, value : execution_targets
    # extract execution modes
    message('registering targets for: ', type)

    foreach name, flags : value.get('exec')
        real_name = 'run-' + type + '-' + name
        message('registering target: ', real_name)
        message('flags: ', qemu_base_flags, flags)

        qemu_targets += {
            real_name: run_target(real_name, 
                depends: [kernel_img],
                command: [
                    qemu,
                    qemu_base_flags,
                    flags
                ]
            )
        }
    endforeach

    message('registering debugging for: ', type)
    foreach debug_type, flags : value.get('debugger')
        debugger_name = ''
        if debug_type != '@default'
            message('registering debugging type ', debug_type,' for: ', type)
            debugger_name = 'attach-' + type + '-' + debug_type
        else 
            message('registering default dbeugging type for: ', type)
            debugger_name = 'attach-' + type 
        endif

        gdb_flags = [ ]
        foreach flag : flags
            gdb_flags += [ '-ex', flag ]
        endforeach

        message('flags: ', gdb_flags)

        qemu_targets += {
            debugger_name: run_target(debugger_name,
                depends: [kernel_dbg],
                command: [
                    gdb,
                    kernel_dbg,
                    gdb_flags
                ]
            )
        }
    endforeach
endforeach 

project('kernel', 'c', 'cpp', version: '0.0.1', default_options: [
    'c_std=c11', 
    'cpp_std=c++20', 
    'warning_level=2',
    'cpp_rtti=false',
    'cpp_eh=none',
    'error_level=3'
])

build_resources = meson.source_root()/'build-resources'

# required program for generating the kernel
gdb = find_program('gdb')
objcopy = find_program('objcopy')
qemu = find_program('qemu-system-x86_64')

gen_dbg =           find_program(build_resources/'gen-dbg')
strip_debug =       find_program(build_resources/'strip_debug.sh')
generate_image =    find_program(build_resources/'generate_image.sh')
git_version_info =  find_program(build_resources/'git_version_info.sh')

kernel_source = [
    'arch/x86/idt/idt.cpp',
    'arch/x86/idt/handlers/div_by_zero.cpp',
    'arch/x86/idt/handlers/handle_gp.cpp',
    'arch/x86/idt/handlers/handle_timer.cpp',
    'arch/x86/idt/idt.S',
    'arch/x86/debug/kinit_dump.cpp',
    'arch/x86/debug/panic.cpp',
    'arch/x86/syscall/syscall_entry.S',
    'arch/x86/pci/pci_scan.cpp',
    'arch/x86/sync/spinlock.cpp',
    'arch/x86/apic/apic.cpp',
    'arch/x86/kinit/kinit.cpp',
    'arch/x86/process/process.cpp',
    'arch/x86/process/save_ctx_for_reschedule.S',
    'arch/x86/process/scheduler/scheduler.cpp',
    'arch/x86/klog/klog.cpp',
    'arch/x86/tty/terminal.cpp',
    'arch/x86/mm/pmm.cpp',
    'arch/x86/mm/malloc.cpp',
    'arch/x86/mm/mm.cpp',
    'arch/x86/cpuid/cpuid.cpp',
    'arch/x86/gdt/gdt.cpp',
    'arch/x86/paging/paging.cpp',
    'arch/x86/paging/pgfault.cpp',
    'arch/x86/smp/smp.cpp',
    'arch/x86/debug/symbols.cpp',
    'lib/src/cstring.cpp',
    'lib/src/new.cpp',
    'lib/src/backtrace.cpp',
    'libimpl/cxxabi.cpp',
    'libimpl/implement.cpp',
    'arch/x86/san/ubsan.cpp'
]

opt_to_config = {
    'PMM_COUNT': 'max_physical_memory_allocators',
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
    'SLAB_SIZE_ORDER': 'slab_size_order'
}

qemu_base_flags = [
    '-bios', build_resources/'OVMF.fd',         # Make sure this path points to **your** ovmf
    '-hda', meson.build_root()/'kernel.img',    # Kernel image to run
    '-no-reboot',                               # Prevent rebooting on exception
    '-monitor', 'stdio',                        # Monitor output to stdio
    '-d', 'int',                                # Generate interrupt logging
    '-D', 'qemu.log',                           # QEMU logging output
    '-no-shutdown',                             
    '-s',                                       # Enable debugging port (same as -gdb tcp::1234)
    '-S',                                       # Freeze CPU on start
    '-M', 'smm=off'                             # No SMM interrupts
]

execution_targets = {
    'normal': {
        'exec': {
            'smp': ['-smp', '2'],
            'nosmp': ['-smp', '1'],
        },
        'debugger': [
            'b _start',
            'c',
            'tui enable',
        ]
    },
    'kvm': {
        'exec': {
            'smp': ['-enable-kvm', '-cpu', 'host', '-smp', '2'],
            'nosmp': ['-enable-kvm', '-cpu', 'host', '-smp', '1']
        },
        'debugger': [
            'hbreak _start',
            'c',
            'tui enable'
        ]
    }
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

# Bunch of ugly implementation details...

add_project_arguments(
    '-ffreestanding',
    '-fno-PIC',
    '-fno-exceptions',
    '-fno-omit-frame-pointer',
    '-fcf-protection=none',
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
    '-Wno-c99-extensions',
    language: 'cpp'
)

add_project_arguments(
    '-ffreestanding',
    '-fno-PIC',
    language: 'c'
)

# options configuration
if get_option('slab_min_order') >= get_option('slab_max_order')
    error('config: slab_min_order cannot >= slab_max_order')
endif

if get_option('slab_max_order') >= get_option('slab_size_order')
    error('config: slab_max_order cannot >= slab_size_order')
endif

# config
conf = configuration_data()
conf.set('VERSION', meson.project_version())
conf.set('ARCH', 'x86_64')
conf.set('ARCH_PATH', 'x86')

foreach key, opt : opt_to_config
    conf.set(key, get_option(opt).to_string())
endforeach

conf.set('HHDM_START', '0xffff800000000000')
conf.set('SCROLLBACK_START', '0xffff900000000000')
conf.set('HEAP_START', '0xffff900080000000')
conf.set('SLAB_START', '0xffff980000000000')
conf.set('KLOG_BUFFER', '0xffffa00000000000')

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
    './',
    './arch/x86/',
    './arch/',
    './lib/',
    './lib/include/',
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

kernel = executable('kernel.elf',
    config_output,
    kernel_source,
    link_depends: meson.source_root()/'kernel.lds',
    link_args: [
        '-T' + (meson.source_root()/'kernel.lds'),
        '-static',
        '-nostdlib',
        '-lgcc',
        '-m64',
    ],
    cpp_args: [
        '-m64', 
        '-march=x86-64', 
        '-mcmodel=kernel',
        '-fsanitize=undefined',
        '-fno-sanitize=function'
    ],
    c_args: ['-m64', '-march=x86-64', '-mcmodel=kernel'],
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
        real_name = type + '-' + name
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
    gdb_flags = [ '-ex', 'target remote localhost:1234' ]

    foreach flag : value.get('debugger')
        gdb_flags += [ '-ex', flag ]
    endforeach

    debugger_name = 'attach-' + type


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

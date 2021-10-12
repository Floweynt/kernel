OUTPUT_FORMAT("binary")
ENTRY(main);
SECTIONS
{
    . = 0xFFFFFFFF80000000;

    kstart = .;

    .text :
	{
		code = .;
        *(.text.preinit)
        *(.text.init)
        *(.place.in.front)
		*(.text*)
	}

	.rodata BLOCK(1) :
	{
		*(.rodata)
	}

	.data BLOCK(1) :
	{
		data = .;
		*(.data)
	}

	.bss BLOCK(1) :
	{
		PROVIDE(_bss_start = .);
		bss = .;
		*(COMMON)
		*(.bss)
		*(.stack)
		PROVIDE(_bss_end = .);
	}

	kend = .;
    PROVIDE(KERNEL_LENGTH = kend - kstart);

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
    }
}

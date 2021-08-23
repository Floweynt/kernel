OUTPUT_FORMAT("binary")
phys = 0x8000;

SECTIONS
{
	. = 0x8000;

	.text :
	{
		code = .;
		*(.text)
	}

	.rodata BLOCK(1) : ALIGN(1)
	{
		*(.rodata)
	}

	.data BLOCK(1) : ALIGN(1)
	{
		data = .;
		*(.data)
	}

	.bss BLOCK(1) : ALIGN(1)
	{
		bss = .;
		*(COMMON)
		*(.bss)
		*(.stack)
	}

    .sig : AT(0x8FFF)
    {
        BYTE(00);
    }

	end = .;

	/DISCARD/ :
	{
		*(.comment)
		*(.eh_frame)
		*(.note.gnu.build-id)
	}
}

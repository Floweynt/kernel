OUTPUT_FORMAT("binary")
ENTRY(main);
SECTIONS
{   
    . = 0x100000;
        
    kstart = .;
    
    .text : 
	{
		code = .;
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
    
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
    }
}

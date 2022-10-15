/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(sys_entry_point)
/*OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)*/
 
/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Begin putting sections at 1 MiB, a conventional place for kernels to be
	   loaded at by the bootloader. */
	. = 1M;

	.multiboot :
	{
		sys_multiboot_header.o(.rodata)
	}
 
	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	/*.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}*/
	.text :
	{
		*(.text)
	}
 
	/* Read-write data (initialized) */
	/*.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}*/
	.data :
	{
		*(.data)
	}

	/* Read-only data. */
	/* CHECK IF WE CAN PUT HERE ONLY RODATA OF SYS_MULTIBOOT_HEADER.O */
	/*.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}*/
	.rodata :
	{
		*(.rodata)
	}
 
	/* Read-write data (uninitialized) and stack */
	/*.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}*/
	.bss :
	{
		*(COMMON)
		*(.bss)
	}
 
	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}


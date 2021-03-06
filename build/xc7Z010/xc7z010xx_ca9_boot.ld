/* $Id$ */
/* XILINX XC7Z010/XC7Z015/XC7Z020 memory layout */
/* Linker script to configure memory regions. */

MEMORY
{
  FLASH (rx) 	: ORIGIN = 0xFC000000, LENGTH = 16M		/* W25Q32FVZPIG */
  OCM (rwx)		: ORIGIN = 0x00000000, LENGTH = 192k	/* OCM - on-chip memory 192k */
  OCM3 (rwx)	: ORIGIN = 0x000F0000, LENGTH = 48k		/* OCM3 - on-chip memory 64k - 16k for TTB */
  TTB (rwx)  	: ORIGIN = 0x000FC000, LENGTH = 16k		/* last 16 kB OCM3 for TTB */
  DDR3 (rwx) 	: ORIGIN = 0x00100000, LENGTH = 191M 	/* lower 64MB reserved for image, DDR3 single chip NT5CC128M16IP-DI */
}

/* Library configurations */
/* GROUP(libgcc.a libc.a libm.a libnosys.a) */

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 */
	 ENTRY(Reset_Handler7)

SECTIONS
{
	.text :
	{
		. = ALIGN(32);
		KEEP(*(.vectors))
		__Vectors_End = .;
		__Vectors_Size = __Vectors_End - __Vectors;
		
		. = ALIGN(32);
		KEEP(*(.init))
		KEEP(*(.initfunc*))
		KEEP(*(.initdata*))
		KEEP(*(.fini))

		KEEP(*(.text*))
		KEEP(*(.itcm*))
	} > OCM

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > OCM

	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > OCM
	__exidx_end = .;

	/* To copy multiple ROM to RAM sections,
	 * uncomment .copy.table section and,
	 * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S */
	.copy.table :
	{
		. = ALIGN(4);
		__copy_table_start__ = .;
	/*
		LONG (__etext)
		LONG (__data_start__)
		LONG (__data_end__ - __data_start__)
		LONG (__etext2)
		LONG (__data2_start__)
		LONG (__data2_end__ - __data2_start__)
	*/
		__copy_table_end__ = .;
	} > OCM

	/* To clear multiple BSS sections,
	 * uncomment .zero.table section and,
	 * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */
	.zero.table :
	{
		. = ALIGN(4);
		__zero_table_start__ = .;
		LONG (__bss_start__)
		LONG (__bss_end__ - __bss_start__)
	/*
		LONG (__bss2_start__)
		LONG (__bss2_end__ - __bss2_start__)
	*/
		__zero_table_end__ = .;
	} > OCM

	__etext = .;

	.data : /*AT (__etext)*/
	{
		__data_start__ = .;
		. = ALIGN(32);
		KEEP(*(.dtcm*))

		. = ALIGN(32);
		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)

		/* .dtors */
 		*crtbegin.o(.dtors)
 		*crtbegin?.o(.dtors)
 		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
 		*(SORT(.dtors.*))
 		*(.dtors)

		*(.rodata*)

		KEEP(*(.eh_frame*))

		KEEP(*(.data*))
	   . = ALIGN(4);	/* one line added by ua1arn */

		. = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(4);
		/* init data */
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);


		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);

		KEEP(*(.jcr*))
		. = ALIGN(32);
		/* All data end */
		__data_end__ = .;

	} > OCM

	.bss :
	{
		__bss_start__ = .;
		. = ALIGN(32);
		KEEP(*(.bss*))
		KEEP(*(COMMON))
		. = ALIGN(32);
		__bss_end__ = .;
	} > OCM

	.noinit :
	{
		. = ALIGN(32);
		KEEP(*(.noinit*))
		. = ALIGN(32);
	} > OCM

	.vram :
	{
		. = ALIGN(64);
		KEEP(*(.framebuff*))
		. = ALIGN(64);
	} > OCM

	.heap (COPY):
	{
		__HeapBase = .;
		__end__ = .;
		end = __end__;
		KEEP(*(.heap*))
		__HeapLimit = .;
	} > OCM

	/* .stack_dummy section doesn't contains any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
	.stack_dummy (COPY):
	{
		KEEP(*(.stack*))
	} > OCM

	/* Set stack top to end of RAM, and stack limit move down by
	 * size of stack_dummy section */
	__StackTop = ORIGIN(OCM) + LENGTH(OCM);
	__StackLimit = __StackTop - SIZEOF(.stack_dummy);
	PROVIDE(__stack = __StackTop);

	PROVIDE(__TTB_BASE = ORIGIN(TTB));
	/* PROVIDE(__ttb_end__ = ORIGIN(TTB) + LENGTH(TTB)); */

	/* Check if data + heap + stack exceeds RAM limit */
	/*ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")*/
}

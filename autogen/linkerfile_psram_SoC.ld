/***************************************************************************//**
 * GCC/CMake Linker script for Silicon Labs devices
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/


 MEMORY
 {
   rom   (rx)  : ORIGIN = 0x8202000, LENGTH = 0x1fe000
   DUMMY (!ax) : ORIGIN = 0xffe02000, LENGTH = 0x1fe000

   ram   (rwx) : ORIGIN = 0xc, LENGTH = 0x2fbf4
   psram (rwx) : ORIGIN = 0xa001000, LENGTH = 0x7ff000
 }
 MEMORY
 {
   udma0   (rwx)  : ORIGIN = 0x2fc00, LENGTH = 0x400
   udma1   (rwx)  : ORIGIN = 0x24061c00, LENGTH = 0x400
 }
/*******************************************************************************
 * Program headers: explicit PT_LOAD with correct p_flags so the linker does not
 * apply wrong access flags when a single flash region holds both executable and
 * read-only data. ELF p_flags: 4=read, 5=read+execute, 6=read+write.
 *
 * When ram_execution is set, output section text_ram (VMA in RAM, LMA in flash)
 * follows the usual ramfunc pattern: startup copies (or uses tables) before .data init.
 ******************************************************************************/
PHDRS
{
  vectors PT_LOAD FLAGS(4);      /* Flash: vector table (separate segment so ram_code LMA doesn't overlap) */
  readonly PT_LOAD FLAGS(4);     /* Flash: .rodata */
  text PT_LOAD FLAGS(5);         /* Flash: executable code, unwind tables */
  tables PT_LOAD FLAGS(4);       /* Flash: .copy.table / .zero.table */
  empty PT_LOAD FLAGS(4);        /* Flash: terminates LMA chain before NOLOAD */
  ram_pre_code PT_LOAD FLAGS(6); /* RAM: BSS, stack before heap */
  ram_code PT_LOAD FLAGS(5);     /* text_ram: VMA in RAM, LMA in PSRAM (ram_execution) */
  ram_data PT_LOAD FLAGS(6);     /* RAM: .data (init values only) AT>rom */
  ram_post_data PT_LOAD FLAGS(6); /* RAM: heap */
  udma0_init PT_LOAD FLAGS(6);   /* .udma_addr0: VMA udma0, LMA rom */
  udma1_init PT_LOAD FLAGS(6);   /* .udma_addr1: VMA udma1, LMA rom */
}

ENTRY(Reset_Handler)
 
SECTIONS
{
	.vectors :
	{
		KEEP(*(.isr_vector))
		. = ALIGN(32);
	} > rom :vectors
	text_ram :
	{
		. = ALIGN(4);
		__ramfuncs_start__ = .;
		*(text_application_ram)
		__text_core_ram_start__ = .;
		*(text_core_*timecritical*)
		. = ALIGN(4);
		__text_core_ram_end__ = .;
		__text_sleeptimer_ram_start__ = .;
		*(text_sleeptimer_*timecritical*)
		. = ALIGN(4);
		__text_sleeptimer_ram_end__ = .;
		__text_cmsis_udma_ram_start__ = .;
		*(text_cmsis_udma_*timecritical*)
		. = ALIGN(4);
		__text_cmsis_udma_ram_end__ = .;
		__text_psram_core_ram_start__ = .;
		*(text_psram_core_*timecritical*)
		. = ALIGN(4);
		__text_psram_core_ram_end__ = .;
		__text_rsilib_chip_ram_start__ = .;
		*(text_rsilib_chip_*timecritical*)
		. = ALIGN(4);
		__text_rsilib_chip_ram_end__ = .;
		__text_rsilib_dcache_ram_start__ = .;
		*(text_rsilib_dcache_*timecritical*)
		. = ALIGN(4);
		__text_rsilib_dcache_ram_end__ = .;
		__text_rsilib_egpio_ram_start__ = .;
		*(text_rsilib_egpio_*timecritical*)
		. = ALIGN(4);
		__text_rsilib_egpio_ram_end__ = .;
		__text_rsilib_qspi_ram_start__ = .;
		*(text_rsilib_qspi_*timecritical*)
		. = ALIGN(4);
		__text_rsilib_qspi_ram_end__ = .;
		__text_rsilib_sysrtc_ram_start__ = .;
		*(text_rsilib_sysrtc_*timecritical*)
		. = ALIGN(4);
		__text_rsilib_sysrtc_ram_end__ = .;
		__text_sl_clock_manager_ram_start__ = .;
		*(text_sl_clock_manager_*timecritical*)
		. = ALIGN(4);
		__text_sl_clock_manager_ram_end__ = .;
		__text_sleeptimer_si91x_ram_start__ = .;
		*(text_sleeptimer_si91x_*timecritical*)
		. = ALIGN(4);
		__text_sleeptimer_si91x_ram_end__ = .;
		__text_systemlevel_pll_ram_start__ = .;
		*(text_systemlevel_pll_*timecritical*)
		. = ALIGN(4);
		__text_systemlevel_pll_ram_end__ = .;
		*(text_freertos_kernel_*timecritical*)
		. = ALIGN(4);
		__ramfuncs_end__ = .;
	} > ram AT> rom :ram_code

	__lma_ramfuncs_start__ = LOADADDR(text_ram);
	__lma_ramfuncs_end__ = LOADADDR(text_ram) + SIZEOF(text_ram);

	.rodata : ALIGN(32)
	{
		*(.rodata*)
		*(.rodata.*)
	} > rom :readonly

	.text :
	{
		KEEP(*(.reset_handler))
		*(SORT_BY_ALIGNMENT(.text*))
			*(SORT_BY_ALIGNMENT(text_*))
		KEEP(*(.init))
		KEEP(*(.fini))	

		/* .ctors */
		*/crtbegin.*(.ctors)
		*/crtbegin?.*(.ctors)
		*(EXCLUDE_FILE(*/crtend?.* */crtend.*) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)
		
		/* .dtors */
		*/crtbegin.*(.dtors)
		*/crtbegin?.*(.dtors)
		*(EXCLUDE_FILE(*/crtend?.* */crtend.*) .dtors)
		*(SORT(.dtors.*))
		*(.dtors)
		
		KEEP(*(.eh_frame*))			
	} > rom :text	
	
	.common_tcm_code :
	{
		*(.common_tcm_code*)
	} > rom :text

	.ARM.extab : 
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)			
	} > rom :text
	
	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)			
	} > rom :text 
	__exidx_end = .;
	.copy.table :
	{
		. = ALIGN(4);
		__copy_table_start__ = .;
		LONG (__etext)
		LONG (__data_start__)
		LONG ((__data_end__ - __data_start__) / 4)
		__copy_table_end__ = .;
	} > rom :tables

	.zero.table :
	{
		. = ALIGN(4);
		__zero_table_start__ = .;
		LONG (__bss_start__)
		LONG ((__bss_end__ - __bss_start__) / 4)
		__zero_table_end__ = .;
	} > rom :tables

	__etext = .;

	

	/* _sidata is used in code startup code */
	_sidata = __etext;

	.data : ALIGN(8)
	
	{
		__data_start__ = .;
		
		/* _sdata is used in startup code */
		_sdata = __data_start__;
		*(.data*)
		/* ipmu calibration data */
		*(.common_ipmu_ram*)
		  
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
		. = ALIGN(4);
		/* All data end */
		__data_end__ = .;
		
		/* _edata is used in startup code */
		_edata = __data_end__; 
	} > ram AT> rom :ram_data
 __etext = LOADADDR(.data);
 _sidata = __etext;
 __rom_start  = ORIGIN(rom);
 __rom_length = LENGTH(rom);

 ASSERT((__etext % 4) == 0, "Data LMA must be word-aligned (Clang copy table)")
 ASSERT((__data_start__ % 4) == 0, "Data VMA must be word-aligned (Clang copy table)")
 ASSERT((__data_end__ % 4) == 0, "Data section must be word-multiple (Clang copy table)")
	.empty :
	{
	} > rom :empty

	.bss (NOLOAD) :
	{
		. = ALIGN(8);
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
	} > ram :ram_pre_code
 ASSERT(((__bss_end__ - __bss_start__) % 4) == 0, "BSS must be word-multiple (Clang zero table)")
		
	.stack (NOLOAD):
	{
		__StackLimit = .;
		KEEP(*(.stack*))
		. = ALIGN(4);
		__StackTop = .;
		PROVIDE(__stack = __StackTop);
	} > ram :ram_pre_code
	/* RAM overflow detection (only valid when stack is in RAM) */
	__current_ram_used = . - ORIGIN(ram);
	__ram_total = LENGTH(ram);
	__ram_remaining = __ram_total - __current_ram_used;
	
	ASSERT((. <= (ORIGIN(ram) + LENGTH(ram))), "RAM OVERFLOW: RAM usage exceeds available memory region")
	ASSERT((__ram_remaining >= 1024), "WARNING: Less than 1KB RAM left for heap - application may run out of dynamic memory")
	ASSERT((__StackLimit >= __bss_end__), "CRITICAL: Stack overlaps with BSS section - increase RAM or reduce static allocation")

	__ram_start__ = ORIGIN(ram);
	__ram_end__   = ORIGIN(ram) + LENGTH(ram);

  	.heap (NOLOAD):
  	{
		. = ALIGN(8);
		__HeapBase = .;
		__end__ = .;
		end = __end__;
		_end = __end__;
		KEEP(*(.heap*))	
		. = ORIGIN(ram) + LENGTH(ram);
  		
		__HeapLimit = .;
  	} > ram :ram_post_data
	__heap_size = __HeapLimit - __HeapBase;

	/* Final heap validation */
	ASSERT((__heap_size >= 0), "CRITICAL: Heap region is invalid (negative size) - stack and heap are overlapping")
	.udma_addr0 :
	{
		*(.udma_addr0*)
	} > udma0 AT> rom :udma0_init

	.udma_addr1 :
	{
		*(.udma_addr1*)		
	} > udma1 AT> rom :udma1_init	
  /* nvm3 enable*/  /* Flash Present*/  /* littlefs_enable */  /* Flash Present */

  /* Stabs debugging sections. */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }

  /* DWARF debug sections.
   * Each DWARF section must be kept in its own uniquely-named output section
   * so that debuggers (GDB, LLDB) and tools can locate them by their
   * canonical names. Merging them into a single catch-all section makes the
   * DWARF info unreadable. */
  .debug              0 : { *(.debug) }
  .line               0 : { *(.line) }
  .debug_srcinfo      0 : { *(.debug_srcinfo) }
  .debug_sfnames      0 : { *(.debug_sfnames) }
  .debug_aranges      0 : { *(.debug_aranges) }
  .debug_pubnames     0 : { *(.debug_pubnames) }
  .debug_info         0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev       0 : { *(.debug_abbrev) }
  .debug_line         0 : { *(.debug_line .debug_line.* .debug_line_end) }
  .debug_frame        0 : { *(.debug_frame) }
  .debug_str          0 : { *(.debug_str) }
  .debug_loc          0 : { *(.debug_loc) }
  .debug_macinfo      0 : { *(.debug_macinfo) }
  .debug_weaknames    0 : { *(.debug_weaknames) }
  .debug_funcnames    0 : { *(.debug_funcnames) }
  .debug_typenames    0 : { *(.debug_typenames) }
  .debug_varnames     0 : { *(.debug_varnames) }
  .debug_pubtypes     0 : { *(.debug_pubtypes) }
  .debug_ranges       0 : { *(.debug_ranges) }
  .debug_macro        0 : { *(.debug_macro) }
  .debug_addr         0 : { *(.debug_addr) }
  .debug_line_str     0 : { *(.debug_line_str) }
  .debug_loclists     0 : { *(.debug_loclists) }
  .debug_rnglists     0 : { *(.debug_rnglists) }
  .debug_str_offsets  0 : { *(.debug_str_offsets) }
  .debug_names        0 : { *(.debug_names) }
  .zdebug_info        0 : { *(.zdebug_info) }
  .zdebug_abbrev      0 : { *(.zdebug_abbrev) }
  .zdebug_line        0 : { *(.zdebug_line) }
  .zdebug_frame       0 : { *(.zdebug_frame) }
  .zdebug_str         0 : { *(.zdebug_str) }
  .zdebug_loc         0 : { *(.zdebug_loc) }
  .zdebug_macinfo     0 : { *(.zdebug_macinfo) }
  .zdebug_macro       0 : { *(.zdebug_macro) }
  .zdebug_pubnames    0 : { *(.zdebug_pubnames) }
  .zdebug_pubtypes    0 : { *(.zdebug_pubtypes) }
  .zdebug_aranges     0 : { *(.zdebug_aranges) }
  .zdebug_ranges      0 : { *(.zdebug_ranges) }
  .zdebug_addr        0 : { *(.zdebug_addr) }
  .zdebug_line_str    0 : { *(.zdebug_line_str) }
  .zdebug_loclists    0 : { *(.zdebug_loclists) }
  .zdebug_rnglists    0 : { *(.zdebug_rnglists) }
  .zdebug_str_offsets 0 : { *(.zdebug_str_offsets) }
  .zdebug_names       0 : { *(.zdebug_names) }

/*
 * Flash overflow guards.
 *
 * Verify that the firmware image fits in the available flash region:
 *   - `rom`   : code + initialized `.data` LMA, when text is in internal flash.
 *   - `psram` : sum of all PSRAM-resident sections (the QSPI XIP-mapped flash),
 *               when text/data are placed in PSRAM.
 * Without these checks, an oversized image can link successfully and only fail
 * at programming time.
 */
ASSERT(((__etext + SIZEOF(.data)) <= (ORIGIN(rom) + LENGTH(rom))),
       "FLASH memory overflowed: firmware exceeds available flash (rom) region. Reduce code/data or move sections to PSRAM.")
}

/*
Linker script for STM32F10x
Copyright RAISONANCE 2007 (modified by Lanchon 1-Feb-2008)
You can use, copy and distribute this file freely, but without any waranty.
Configure memory sizes, end of stack and boot mode for your project here.
*/


/* include the common STM32F10x sub-script */
INCLUDE "lib/ld/common.ld"

/* STM32F102x4 */

/* Flash page size */
_pagesize = 1K;

/* Memory Spaces Definitions */
MEMORY
{
  RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 4K
  FLASH (rx) : ORIGIN = 0x8000000, LENGTH = 16K
  FLASHB1 (rx) : ORIGIN = 0x00000000, LENGTH = 0
  EXTMEMB0 (rx) : ORIGIN = 0x00000000, LENGTH = 0
  EXTMEMB1 (rx) : ORIGIN = 0x00000000, LENGTH = 0
  EXTMEMB2 (rx) : ORIGIN = 0x00000000, LENGTH = 0
  EXTMEMB3 (rx) : ORIGIN = 0x00000000, LENGTH = 0
}

/* stack and heap config */
_Minimum_Stack_Size = 1K;
_Minimum_Heap_Size = 0;

/* highest address of the user mode stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* include the section management sub-script */
INCLUDE "lib/ld/flash.ld"

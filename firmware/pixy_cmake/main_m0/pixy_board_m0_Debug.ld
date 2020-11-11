/*
 * GENERATED FILE - DO NOT EDIT
 * Copyright (c) 2008-2013 Code Red Technologies Ltd,
 * Copyright 2015, 2018-2019 NXP
 * (c) NXP Semiconductors 2013-2020
 * Generated linker script file for LPC4330-M0
 * Created from linkscript.ldt by FMCreateLinkLibraries
 * Using Freemarker v2.3.23
 * MCUXpresso IDE v11.1.1 [Build 3241] [2020-03-02] on 2020-07-16 22:55:44
 */


MEMORY
{
  /* Define each memory region */
  RamLoc72 (rwx) : ORIGIN = 0x10080000, LENGTH = 0x12000 /* 72K bytes (alias RAM) */
  RamAHB32 (rwx) : ORIGIN = 0x20000000, LENGTH = 0x8000 /* 32K bytes (alias RAM2) */
  RamAHB16 (rwx) : ORIGIN = 0x20008000, LENGTH = 0x4000 /* 16K bytes (alias RAM3) */
  RamAHB_ETB16 (rwx) : ORIGIN = 0x2000c000, LENGTH = 0x4000 /* 16K bytes (alias RAM4) */
}

  /* Define a symbol for the top of each memory region */
  __base_RamLoc72 = 0x10080000  ; /* RamLoc72 */
  __base_RAM = 0x10080000 ; /* RAM */
  __top_RamLoc72 = 0x10080000 + 0x12000 ; /* 72K bytes */
  __top_RAM = 0x10080000 + 0x12000 ; /* 72K bytes */
  __base_RamAHB32 = 0x20000000  ; /* RamAHB32 */
  __base_RAM2 = 0x20000000 ; /* RAM2 */
  __top_RamAHB32 = 0x20000000 + 0x8000 ; /* 32K bytes */
  __top_RAM2 = 0x20000000 + 0x8000 ; /* 32K bytes */
  __base_RamAHB16 = 0x20008000  ; /* RamAHB16 */
  __base_RAM3 = 0x20008000 ; /* RAM3 */
  __top_RamAHB16 = 0x20008000 + 0x4000 ; /* 16K bytes */
  __top_RAM3 = 0x20008000 + 0x4000 ; /* 16K bytes */
  __base_RamAHB_ETB16 = 0x2000c000  ; /* RamAHB_ETB16 */
  __base_RAM4 = 0x2000c000 ; /* RAM4 */
  __top_RamAHB_ETB16 = 0x2000c000 + 0x4000 ; /* 16K bytes */
  __top_RAM4 = 0x2000c000 + 0x4000 ; /* 16K bytes */

ENTRY(ResetISR)

SECTIONS
{
     /* MAIN TEXT SECTION */
    .text : ALIGN(4)
    {
        FILL(0xff)
        __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))
        /* Global Section Table */
        . = ALIGN(4) ;
        __section_table_start = .;
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        LONG(LOADADDR(.data_RAM2));
        LONG(    ADDR(.data_RAM2));
        LONG(  SIZEOF(.data_RAM2));
        LONG(LOADADDR(.data_RAM3));
        LONG(    ADDR(.data_RAM3));
        LONG(  SIZEOF(.data_RAM3));
        LONG(LOADADDR(.data_RAM4));
        LONG(    ADDR(.data_RAM4));
        LONG(  SIZEOF(.data_RAM4));
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        LONG(    ADDR(.bss_RAM2));
        LONG(  SIZEOF(.bss_RAM2));
        LONG(    ADDR(.bss_RAM3));
        LONG(  SIZEOF(.bss_RAM3));
        LONG(    ADDR(.bss_RAM4));
        LONG(  SIZEOF(.bss_RAM4));
        __bss_section_table_end = .;
        __section_table_end = . ;
        /* End of Global Section Table */

        *(.after_vectors*)

    } > RamLoc72

    .text : ALIGN(4)
    {
       *(.text*)
       *(.rodata .rodata.* .constdata .constdata.*)
       . = ALIGN(4);
            /* C++ constructors etc */
            . = ALIGN(4);
            KEEP(*(.init))
            
            . = ALIGN(4);
            __preinit_array_start = .;
            KEEP (*(.preinit_array))
            __preinit_array_end = .;
            
            . = ALIGN(4);
            __init_array_start = .;
            KEEP (*(SORT(.init_array.*)))
            KEEP (*(.init_array))
            __init_array_end = .;
            
            KEEP(*(.fini));
            
            . = ALIGN(4);
            KEEP (*crtbegin.o(.ctors))
            KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
            KEEP (*(SORT(.ctors.*)))
            KEEP (*crtend.o(.ctors))
            
            . = ALIGN(4);
            KEEP (*crtbegin.o(.dtors))
            KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
            KEEP (*(SORT(.dtors.*)))
            KEEP (*crtend.o(.dtors))
            . = ALIGN(4);
            /* End C++ */
    } > RamLoc72
    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this.
     */
    .ARM.extab : ALIGN(4)
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > RamLoc72

    .ARM.exidx : ALIGN(4)
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > RamLoc72
 
    _etext = .;
        
    /* DATA section for RamAHB32 */

    .data_RAM2 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM2 = .) ;
        PROVIDE(__start_data_RamAHB32 = .) ;
        *(.ramfunc.$RAM2)
        *(.ramfunc.$RamAHB32)
        *(.data.$RAM2)
        *(.data.$RamAHB32)
        *(.data.$RAM2.*)
        *(.data.$RamAHB32.*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM2 = .) ;
        PROVIDE(__end_data_RamAHB32 = .) ;
     } > RamAHB32 AT>RamLoc72

    /* DATA section for RamAHB16 */

    .data_RAM3 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM3 = .) ;
        PROVIDE(__start_data_RamAHB16 = .) ;
        *(.ramfunc.$RAM3)
        *(.ramfunc.$RamAHB16)
        *(.data.$RAM3)
        *(.data.$RamAHB16)
        *(.data.$RAM3.*)
        *(.data.$RamAHB16.*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM3 = .) ;
        PROVIDE(__end_data_RamAHB16 = .) ;
     } > RamAHB16 AT>RamLoc72

    /* DATA section for RamAHB_ETB16 */

    .data_RAM4 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM4 = .) ;
        PROVIDE(__start_data_RamAHB_ETB16 = .) ;
        *(.ramfunc.$RAM4)
        *(.ramfunc.$RamAHB_ETB16)
        *(.data.$RAM4)
        *(.data.$RamAHB_ETB16)
        *(.data.$RAM4.*)
        *(.data.$RamAHB_ETB16.*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM4 = .) ;
        PROVIDE(__end_data_RamAHB_ETB16 = .) ;
     } > RamAHB_ETB16 AT>RamLoc72

    /* MAIN DATA SECTION */
    .uninit_RESERVED (NOLOAD) : ALIGN(4)
    {
        _start_uninit_RESERVED = .;
        KEEP(*(.bss.$RESERVED*))
       . = ALIGN(4) ;
        _end_uninit_RESERVED = .;
    } > RamLoc72 AT> RamLoc72

    /* Main DATA section (RamLoc72) */
    .data : ALIGN(4)
    {
       FILL(0xff)
       _data = . ;
       PROVIDE(__start_data_RAM = .) ;
       PROVIDE(__start_data_RamLoc72 = .) ;
       *(vtable)
       *(.ramfunc*)
       KEEP(*(CodeQuickAccess))
       KEEP(*(DataQuickAccess))
       *(RamFunction)
       *(.data*)
       . = ALIGN(4) ;
       _edata = . ;
       PROVIDE(__end_data_RAM = .) ;
       PROVIDE(__end_data_RamLoc72 = .) ;
    } > RamLoc72 AT>RamLoc72

    /* BSS section for RamAHB32 */
    .bss_RAM2 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM2 = .) ;
       PROVIDE(__start_bss_RamAHB32 = .) ;
       *(.bss.$RAM2)
       *(.bss.$RamAHB32)
       *(.bss.$RAM2.*)
       *(.bss.$RamAHB32.*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM2 = .) ;
       PROVIDE(__end_bss_RamAHB32 = .) ;
    } > RamAHB32 AT> RamAHB32

    /* BSS section for RamAHB16 */
    .bss_RAM3 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM3 = .) ;
       PROVIDE(__start_bss_RamAHB16 = .) ;
       *(.bss.$RAM3)
       *(.bss.$RamAHB16)
       *(.bss.$RAM3.*)
       *(.bss.$RamAHB16.*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM3 = .) ;
       PROVIDE(__end_bss_RamAHB16 = .) ;
    } > RamAHB16 AT> RamAHB16

    /* BSS section for RamAHB_ETB16 */
    .bss_RAM4 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM4 = .) ;
       PROVIDE(__start_bss_RamAHB_ETB16 = .) ;
       *(.bss.$RAM4)
       *(.bss.$RamAHB_ETB16)
       *(.bss.$RAM4.*)
       *(.bss.$RamAHB_ETB16.*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM4 = .) ;
       PROVIDE(__end_bss_RamAHB_ETB16 = .) ;
    } > RamAHB_ETB16 AT> RamAHB_ETB16

    /* MAIN BSS SECTION */
    .bss : ALIGN(4)
    {
        _bss = .;
        PROVIDE(__start_bss_RAM = .) ;
        PROVIDE(__start_bss_RamLoc72 = .) ;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(__end_bss_RAM = .) ;
        PROVIDE(__end_bss_RamLoc72 = .) ;
        PROVIDE(end = .);
    } > RamLoc72 AT> RamLoc72

    /* NOINIT section for RamAHB32 */
    .noinit_RAM2 (NOLOAD) : ALIGN(4)
    {
       PROVIDE(__start_noinit_RAM2 = .) ;
       PROVIDE(__start_noinit_RamAHB32 = .) ;
       *(.noinit.$RAM2)
       *(.noinit.$RamAHB32)
       *(.noinit.$RAM2.*)
       *(.noinit.$RamAHB32.*)
       . = ALIGN(4) ;
       PROVIDE(__end_noinit_RAM2 = .) ;
       PROVIDE(__end_noinit_RamAHB32 = .) ;
    } > RamAHB32 AT> RamAHB32

    /* NOINIT section for RamAHB16 */
    .noinit_RAM3 (NOLOAD) : ALIGN(4)
    {
       PROVIDE(__start_noinit_RAM3 = .) ;
       PROVIDE(__start_noinit_RamAHB16 = .) ;
       *(.noinit.$RAM3)
       *(.noinit.$RamAHB16)
       *(.noinit.$RAM3.*)
       *(.noinit.$RamAHB16.*)
       . = ALIGN(4) ;
       PROVIDE(__end_noinit_RAM3 = .) ;
       PROVIDE(__end_noinit_RamAHB16 = .) ;
    } > RamAHB16 AT> RamAHB16

    /* NOINIT section for RamAHB_ETB16 */
    .noinit_RAM4 (NOLOAD) : ALIGN(4)
    {
       PROVIDE(__start_noinit_RAM4 = .) ;
       PROVIDE(__start_noinit_RamAHB_ETB16 = .) ;
       *(.noinit.$RAM4)
       *(.noinit.$RamAHB_ETB16)
       *(.noinit.$RAM4.*)
       *(.noinit.$RamAHB_ETB16.*)
       . = ALIGN(4) ;
       PROVIDE(__end_noinit_RAM4 = .) ;
       PROVIDE(__end_noinit_RamAHB_ETB16 = .) ;
    } > RamAHB_ETB16 AT> RamAHB_ETB16

    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD): ALIGN(4)
    {
        _noinit = .;
        PROVIDE(__start_noinit_RAM = .) ;
        PROVIDE(__start_noinit_RamLoc72 = .) ;
        *(.noinit*)
         . = ALIGN(4) ;
        _end_noinit = .;
       PROVIDE(__end_noinit_RAM = .) ;
       PROVIDE(__end_noinit_RamLoc72 = .) ;        
    } > RamLoc72 AT> RamLoc72
    PROVIDE(_pvHeapStart = DEFINED(__user_heap_base) ? __user_heap_base : .);
    PROVIDE(_pvHeapEnd = __top_RamLoc72 - 0x1000);
    PROVIDE(_vStackTop = DEFINED(__user_stack_top) ? __user_stack_top : __top_RamLoc72 - 0);

    /* Provide basic symbols giving location and size of main text
     * block, including initial values of RW data sections. Note that
     * these will need extending to give a complete picture with
     * complex images (e.g multiple Flash banks).
     */
    _image_start = LOADADDR(.text);
    _image_end = LOADADDR(.data) + SIZEOF(.data);
    _image_size = _image_end - _image_start;
}
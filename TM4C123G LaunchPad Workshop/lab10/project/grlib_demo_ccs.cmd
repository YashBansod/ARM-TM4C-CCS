/******************************************************************************
 *
 * grlib_demo_ccs.cmd - CCS linker configuration file for grlib_demo.
 *
 * Copyright (c) 2012 Texas Instruments Incorporated.  All rights reserved.
 * TI Information - Selective Disclosure
 *
 *****************************************************************************/

--retain=g_pfnVectors

/* The following command line options are set as part of the CCS project.    */
/* If you are building using the command line, or for some reason want to    */
/* define them here, you can uncomment and modify these lines as needed.     */
/* If you are using CCS for building, it is probably better to make any such */
/* modifications in your CCS project and leave this file alone.              */
/*                                                                           */
/* --heap_size=0                                                             */
/* --stack_size=256                                                          */
/* --library=rtsv7M3_T_le_eabi.lib                                           */

/* The starting address of the application.  Normally the interrupt vectors  */
/* must be located at the beginning of the application.                      */
#define APP_BASE 0x00000000
#define RAM_BASE 0x20000000

/* System memory map */

MEMORY
{
    /* Application stored in and executes from internal flash */
    FLASH (RX) : origin = APP_BASE, length = 0x00040000
    /* Application uses internal RAM for data */
    SRAM (RWX) : origin = 0x20000000, length = 0x00008000
}

/* Section allocation in memory */

SECTIONS
{
    .intvecs:   > APP_BASE
    .text   :   > FLASH
    .const  :   > FLASH
    .cinit  :   > FLASH
    .pinit  :   > FLASH
    .init_array : > FLASH

    .vtable :   > RAM_BASE
    .data   :   > SRAM
    .bss    :   > SRAM
    .sysmem :   > SRAM
    .stack  :   > SRAM
}

__STACK_TOP = __stack + 1024;

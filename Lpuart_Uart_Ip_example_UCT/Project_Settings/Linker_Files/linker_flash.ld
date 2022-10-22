/*==================================================================================================
*   Project              : RTD AUTOSAR 4.4
*   Platform             : CORTEXM
*   Peripheral           : 
*   Dependencies         : none
*
*   Autosar Version      : 4.4.0
*   Autosar Revision     : ASR_REL_4_4_REV_0000
*   Autosar Conf.Variant :
*   SW Version           : 0.8.1
*   Build Version        : S32K3_RTD_0_8_1_D2011_ASR_REL_4_4_REV_0000_20201127
*
*   (c) Copyright 2020 NXP Semiconductors
*   All Rights Reserved.
*
*   NXP Confidential. This software is owned or controlled by NXP and may only be
*   used strictly in accordance with the applicable license terms. By expressly
*   accepting such terms or by downloading, installing, activating and/or otherwise
*   using the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms. If you do not agree to be
*   bound by the applicable license terms, then you may not retain, install,
*   activate or otherwise use the software.
==================================================================================================*/
/*
* GCC Linker Command File:
* 0x00400000    0x007FFFFF  4194304 Program Flash (last 64K sBAF)
* 0x10000000    0x1003FFFF  262144  Data Flash (last 32K HSE_NVM)
* 0x20400000    0x20408000  32768   Standby RAM_0 (32K)
* 0x20400000    0x20427FFF  163840  SRAM_0
* 0x20428000    0x2044FFFF  163840  SRAM_1
* Last 48 KB of SRAM_1 reserved by HSE Firmware
* Last 128 KB of CODE_FLASH_3 reserved by HSE Firmware
* Last 128 KB of DATA_FLASH reserved by HSE Firmware (not supported in this linker file)
*/

ENTRY(Reset_Handler)

MEMORY 
{         
    int_flash               : ORIGIN = 0x00400000, LENGTH = 0x003E0000    /* 4096K - 128K (sBAF + HSE)*/
    int_itcm                : ORIGIN = 0x00000000, LENGTH = 0x00008000    /* 32K */
    int_dtcm                : ORIGIN = 0x20000000, LENGTH = 0x00010000    /* 64K */
    int_sram                : ORIGIN = 0x20400000, LENGTH = 0x0002E000    /* 184K */
    int_sram_stack_c0       : ORIGIN = 0x2042E000, LENGTH = 0x00001000    /* 4KB  */
    int_sram_stack_c1       : ORIGIN = 0x2042F000, LENGTH = 0x00001000    /* 4KB  */
    int_sram_no_cacheable   : ORIGIN = 0x20430000, LENGTH = 0x00010000    /* 64KB, needs to include int_results  */
    int_sram_shareable      : ORIGIN = 0x20440000, LENGTH = 0x00004000    /* 16KB */
    ram_rsvd2               : ORIGIN = 0x20444000, LENGTH = 0             /* End of SRAM */
}


SECTIONS
{
    
    .flash :
    {
    	KEEP(*(.boot_header))
        . = ALIGN(4096);
        __text_start = .;
        __interrupts_rom_start = .;
        KEEP(*(.intc_vector))    
        . = ALIGN(4);
        __interrupts_rom_end = .;
        KEEP(*(.core_loop)) 
        . = ALIGN(4);
        *(.startup) 
        . = ALIGN(4);
        *(.systeminit) 
        . = ALIGN(4);
        *(.text.startup) 
        . = ALIGN(4);
        *(.text)
        *(.text*) 
        . = ALIGN(4);
        *(.mcal_text) 
        . = ALIGN(4);
        KEEP(*(.init))
        . = ALIGN(4);
        KEEP(*(.fini)) 
         
        . = ALIGN(4);
        *(.rodata)  
        *(.rodata*)  
        . = ALIGN(4);
        *(.mcal_const_cfg)  
        . = ALIGN(4);
        *(.mcal_const)  
        . = ALIGN(4);
        __init_table = .;
        KEEP(*(.init_table))  
        . = ALIGN(4);
        __zero_table = .;
        KEEP(*(.zero_table))
    } > int_flash

    . = ALIGN(4);
    __text_end = .;
    __sram_data_rom = __text_end;
    __sram_data_rom_acfls = __sram_data_rom;
    
    .sram_data : AT(__sram_data_rom)
    {
        . = ALIGN(4);
        __sram_data_begin__ = .;
        __sram_data_acfls_begin__ = .;
        *(.acfls_code_rom) 
        acfls_code_ram_stop   = .;
        . = ALIGN(4);
        *(.aceep_code_rom) 
        . = ALIGN(4);
        *(.acmcu_code_rom) 
        . = ALIGN(4);
        *(.ramcode)    
        . = ALIGN(4);
        *(.data)  
        *(.data*)
        . = ALIGN(4);
        *(.mcal_data)
        . = ALIGN(4);
        __sram_data_end__ = .;
    } > int_sram

    __sram_data_rom_end = __sram_data_rom + (__sram_data_end__ - __sram_data_begin__);
    __sram_data_rom_acfls_end = __sram_data_rom_acfls + (acfls_code_ram_stop - __sram_data_acfls_begin__);
    .sram_bss (NOLOAD) :
    {
        . = ALIGN(16);
        __sram_bss_start = .;
        *(.bss)
        *(.bss*)
        . = ALIGN(16);
        *(.mcal_bss)
        . = ALIGN(4);
        __sram_bss_end = .;
    } > int_sram

    __non_cacheable_data_rom = __sram_data_rom_end;

    .non_cacheable_data : AT(__non_cacheable_data_rom)
    {
        . = ALIGN(4);
        __non_cacheable_data_start__ = .;
        KEEP(*(.int_results))  
        . += 0x100;
        . = ALIGN(4096);
        __interrupts_ram_start = .;
        . += __interrupts_rom_end - __interrupts_rom_start;    
        . = ALIGN(4);
        __interrupts_ram_end = .;
        *(.mcal_data_no_cacheable)        
        . = ALIGN(4);
        *(.mcal_const_no_cacheable)      
        . = ALIGN(4);
        HSE_LOOP_ADDR = .;
        LONG(0x0);
        __non_cacheable_data_end__ = .;  
    } > int_sram_no_cacheable

    __non_cacheable_data_rom_end = __non_cacheable_data_rom + (__non_cacheable_data_end__ - __non_cacheable_data_start__);

    .non_cacheable_bss (NOLOAD) :
    {  
        . = ALIGN(16);
        __non_cacheable_bss_start = .;
        *(.mcal_bss_no_cacheable)      
        . = ALIGN(4);
        __non_cacheable_bss_end = .;       
    } > int_sram_no_cacheable

    __shareable_data_rom = __non_cacheable_data_rom_end;

    .shareable_data : AT(__shareable_data_rom)
    {
        . = ALIGN(4);
        __shareable_data_start__ = .;
        KEEP(*(.mcal_shared_data)) 
        . = ALIGN(4);
        __shareable_data_end__ = .;  
    } > int_sram_shareable

    __shareable_data_rom_end = __shareable_data_rom + (__shareable_data_end__ - __shareable_data_start__);

    .shareable_bss (NOLOAD) :
    {  
        . = ALIGN(16);
        __shareable_bss_start = .;
        *(.mcal_shared_bss)      
        . = ALIGN(4);
        __shareable_bss_end = .;       
    } > int_sram_shareable

    __Stack_end_c0           = ORIGIN(int_sram_stack_c0);
    __Stack_start_c0         = ORIGIN(int_sram_stack_c0) + LENGTH(int_sram_stack_c0);
    __Stack_end_c1           = ORIGIN(int_sram_stack_c1);
    __Stack_start_c1         = ORIGIN(int_sram_stack_c1) + LENGTH(int_sram_stack_c1);

    __INT_SRAM_START         = ORIGIN(int_sram);
    __INT_SRAM_END           = ORIGIN(ram_rsvd2);
    
    __INT_ITCM_START         = ORIGIN(int_itcm);
    __INT_ITCM_END           = ORIGIN(int_itcm) + LENGTH(int_itcm);
    
    __INT_DTCM_START         = ORIGIN(int_dtcm);
    __INT_DTCM_END           = ORIGIN(int_dtcm) + LENGTH(int_dtcm);
    
    __RAM_SHAREABLE_START    = ORIGIN(int_sram_shareable);
    __RAM_SHAREABLE_END      = ORIGIN(ram_rsvd2)-1;
    __ROM_SHAREABLE_START    = __shareable_data_rom;
    __ROM_SHAREABLE_END      = __shareable_data_rom_end;
    __RAM_NO_CACHEABLE_START = ORIGIN(int_sram_no_cacheable);
    __RAM_NO_CACHEABLE_END   = ORIGIN(int_sram_shareable)-1;
    __ROM_NO_CACHEABLE_START = __non_cacheable_data_rom;
    __ROM_NO_CACHEABLE_END   = __non_cacheable_data_rom_end;
    __RAM_CACHEABLE_START    = ORIGIN(int_sram);
    __RAM_CACHEABLE_END      = ORIGIN(int_sram_no_cacheable)-1;
    __ROM_CACHEABLE_START    = __sram_data_rom;
    __ROM_CACHEABLE_END      = __sram_data_rom_end;
    
    
    __BSS_SRAM_START         = __sram_bss_start;
    __BSS_SRAM_END           = __sram_bss_end;
    __BSS_SRAM_SIZE          = __sram_bss_end - __sram_bss_start;
    
    __BSS_SRAM_NC_START      = __non_cacheable_bss_start;
    __BSS_SRAM_NC_SIZE       = __non_cacheable_bss_end - __non_cacheable_bss_start;
    __BSS_SRAM_NC_END        = __non_cacheable_bss_end;

    __BSS_SRAM_SH_START      = __shareable_bss_start;
    __BSS_SRAM_SH_SIZE       = __shareable_bss_end - __shareable_bss_start;
    __BSS_SRAM_SH_END        = __shareable_bss_end;

    __RAM_INTERRUPT_START    = __interrupts_ram_start;
    __ROM_INTERRUPT_START    = __interrupts_rom_start;
    __ROM_INTERRUPT_END      = __interrupts_rom_end;

    __INIT_TABLE             = __init_table;
    __ZERO_TABLE             = __zero_table;
    
    __RAM_INIT               = 1;
    __ITCM_INIT              = 1;
    __DTCM_INIT              = 1;
    
    Fls_ACEraseRomStart         = __sram_data_rom_acfls;
    Fls_ACEraseRomEnd           = __sram_data_rom_acfls_end;
    Fls_ACEraseSize             = __sram_data_rom_acfls_end - __sram_data_rom_acfls; /* Copy 4 bytes at a time*/

    Fls_ACWriteRomStart         = __sram_data_rom_acfls;
    Fls_ACWriteRomEnd           = __sram_data_rom_acfls_end;
    Fls_ACWriteSize             = __sram_data_rom_acfls_end - __sram_data_rom_acfls; /* Copy 4 bytes at a time*/
    
    _ERASE_FUNC_ADDRESS_        = __sram_data_acfls_begin__;
    _WRITE_FUNC_ADDRESS_        = __sram_data_acfls_begin__;

}

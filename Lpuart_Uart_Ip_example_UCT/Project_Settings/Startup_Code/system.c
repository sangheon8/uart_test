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

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/
#include "Platform_Types.h"
#include "Mcal.h"
#include "system.h"

#include "S32K344_SCB.h"
#include "S32K344_MPU.h"
#include "S32K344_MSCM.h"

/*==================================================================================================
*                                      FILE VERSION CHECKS
==================================================================================================*/

/*==================================================================================================
*                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                       LOCAL MACROS
==================================================================================================*/
#define CM7_0  (0UL)
#define CM7_1  (1UL)

#define SVC_GoToSupervisor()      ASM_KEYWORD("svc 0x0")
#define SVC_GoToUser()            ASM_KEYWORD("svc 0x1")

#define S32_SCB_CPACR_CPx_MASK(CpNum)             (0x3U << S32_SCB_CPACR_CPx_SHIFT(CpNum))
#define S32_SCB_CPACR_CPx_SHIFT(CpNum)            (2U*((uint32)CpNum))
#define S32_SCB_CPACR_CPx(CpNum, x)               (((uint32)(((uint32)(x))<<S32_SCB_CPACR_CPx_SHIFT((CpNum))))&S32_SCB_CPACR_CPx_MASK((CpNum)))

/*==================================================================================================
*                                       LOCAL VARIABLES
==================================================================================================*/
#define CPU_DEFAULT_MEMORY_CNT (14U)

/*
  Region  Description       Start       End           Size[KB]  Type              Inner Cache Policy    Outer Cache Policy    Shareable    Executable    Privileged Access    Unprivileged Access
--------  -------------     ----------  ----------  ----------  ----------------  --------------------  --------------------  -----------  ------------  -------------------  ---------------------
       0  Whole memory map  0x0         0xFFFFFFFF     4194304  Strongly Ordered  None                  None                  Yes          No            No Access            No Access
       1  ITCM              0x0         0xFFFF              64  Strongly Ordered  None                  None                  Yes          Yes           Read/Write           No Access
       2  Program Flash     0x400000    0x7FFFFF          4096  Normal            Write-Back/Allocate   Write-Back/Allocate   No           Yes           Read-Only            Read-Only
       3  Data Flash        0x10000000  0x1003FFFF         256  Normal            Write-Back/Allocate   Write-Back/Allocate   No           No            Read-Only            Read-Only
       4  DTCM              0x20000000  0x2001FFFF         128  Strongly Ordered  None                  None                  Yes          Yes           Read/Write           No Access
       5  SRAM + STACK      0x20400000  0x2043FFFF         256  Normal            Write-Back/Allocate   Write-Back/Allocate   No           Yes           Read/Write           Read/Write
       6  SRAM NC           0x20430000  0x2043FFFF          64  Normal            None                  None                  Yes          No            Read/Write           Read/Write
       7  SRAM SHARED       0x20440000  0x20443FFF          16  Normal            None                  None                  Yes          No            Read/Write           Read/Write
       8  AIPS_0            0x40000000  0x401FFFFF        2048  Device            None                  None                  Yes          No            Read/Write           Read/Write
       9  AIPS_1            0x40200000  0x403FFFFF        2048  Device            None                  None                  Yes          No            Read/Write           Read/Write
      10  AIPS_2            0x40400000  0x405FFFFF        2048  Device            None                  None                  Yes          No            Read/Write           Read/Write
      11  QSPI Rx           0x67000000  0x670003FF           1  Device            None                  None                  Yes          No            Read/Write           Read/Write
      12  QSPI AHB          0x68000000  0x6FFFFFFF      131072  Normal            Write-Back/Allocate   Write-Back/Allocate   No           Yes           Read/Write           Read/Write
      13  PPB               0xE0000000  0xE00FFFFF        1024  Strongly Ordered  None                  None                  Yes          No            Read/Write           Read/Write
*/

static const uint32 rbar[CPU_DEFAULT_MEMORY_CNT] = {0x00000000UL, 0x00000000UL, 0x00400000UL, 0x10000000UL, 0x20000000UL, 0x20400000UL, 0x20430000UL, 0x20440000UL, 0x40000000UL, 0x40200000UL, 0x40400000UL, 0x67000000UL, 0x68000000UL, 0xE0000000UL};
static const uint32 rasr[CPU_DEFAULT_MEMORY_CNT] = {0x1004003FUL, 0x0104001FUL, 0x060B002BUL, 0x160B0023UL, 0x01040021UL, 0x030B0023UL, 0x130C001FUL, 0x130C001BUL, 0x13050029UL, 0x13050029UL, 0x13050029UL, 0x13050013UL, 0x030B0035UL, 0x13040027UL};

/*==================================================================================================-
*                                       GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                       GLOBAL VARIABLES
==================================================================================================*/
/* Allocate a global variable which will be overwritten by the debugger if attached(in CMM), to catch the core after reset. */
uint32 RESET_CATCH_CORE=0x00U;

/*==================================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void sys_m7_cache_init(void);
/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/
#ifdef MCAL_ENABLE_USER_MODE_SUPPORT  
    extern uint32 startup_getControlRegisterValue(void);
    extern uint32 startup_getAipsRegisterValue(void);
#endif /*MCAL_ENABLE_USER_MODE_SUPPORT*/


/*================================================================================================*/
/**
* @brief    startup_go_to_user_mode
* @details  Function called from startup.s to switch to user mode if MCAL_ENABLE_USER_MODE_SUPPORT
*           is defined
*/
/*================================================================================================*/
void startup_go_to_user_mode(void);
void startup_go_to_user_mode(void)
{
#ifdef MCAL_ENABLE_USER_MODE_SUPPORT
    ASM_KEYWORD("svc 0x1");
#endif
}

/*================================================================================================*/
/**
* @brief   Default IRQ handler
* @details Infinite Loop
*/
/*================================================================================================*/
void default_interrupt_routine(void)
{
    while(1);
}

/*================================================================================================*/
/**
* @brief Sys_GoToSupervisor
* @details function used to enter to supervisor mode.
*           check if it's needed to switch to supervisor mode and make the switch.
*           Return 1 if switch was done
*/
/*================================================================================================*/

#ifdef MCAL_ENABLE_USER_MODE_SUPPORT
uint32 Sys_GoToSupervisor(void)
{
    uint32 u32ControlRegValue;
    uint32 u32AipsRegValue;
    uint32 u32SwitchToSupervisor;

    /* if it's 0 then Thread mode is already in supervisor mode */
    u32ControlRegValue = startup_getControlRegisterValue();
    /* if it's 0 the core is in Thread mode, otherwise in Handler mode */
    u32AipsRegValue = startup_getAipsRegisterValue();

    /* if core is already in supervisor mode for Thread mode, or running form Handler mode, there is no need to make the switch */
    if((0UL == (u32ControlRegValue & 1)) || (0 < (u32AipsRegValue & 0xFF)))
    {
        u32SwitchToSupervisor = 0U;
    }
    else
    {
        u32SwitchToSupervisor = 1U;
        SVC_GoToSupervisor();
    }

    return u32SwitchToSupervisor;
}

/*================================================================================================*/
/**
* @brief Sys_GoToUser_Return
* @details function used to switch back to user mode for Thread mode, return a uint32 value passed as parameter
*/
/*================================================================================================*/
uint32 Sys_GoToUser_Return(uint32 u32SwitchToSupervisor, uint32 u32returnValue)
{
    if (1UL == u32SwitchToSupervisor)
    {
        SVC_GoToUser();
    }

    return u32returnValue;
}

uint32 Sys_GoToUser(void)
{
    SVC_GoToUser();
    return 0UL;
}
#endif


/*================================================================================================*/
/**
* @brief Sys_GetCoreID
* @details Function used to get the ID of the currently executing thread
*/
/*================================================================================================*/
#if !defined(USING_OS_AUTOSAROS)
uint8 Sys_GetCoreID(void)
{
    return (MSCM->CPXNUM & MSCM_CPXNUM_CPN_MASK);
}
#endif

/*================================================================================================*/
/*
 * system initialization : system clock, interrupt router ...
 */
#ifdef __ICCARM__ 
    #pragma default_function_attributes = @ ".systeminit"
#else
    __attribute__ ((section (".systeminit")))
#endif 

void SystemInit(void)
{
    uint32 i;
    uint32 coreMask;
	uint8 regionNum = 0U;

    uint8 coreId = OsIf_GetCoreID();

    switch(coreId)
    {
        case CM7_0:
            coreMask = (1UL << MSCM_IRSPRC_M7_0_SHIFT);
            break;
        case CM7_1:
            coreMask = (1UL << MSCM_IRSPRC_M7_1_SHIFT);
            break;
        default:
            coreMask = 0UL;
            break;
    }

    /* Configure MSCM to enable/disable interrupts routing to Core processor */
    for (i = 0; i < MSCM_IRSPRC_COUNT; i++) 
    {
        MSCM->IRSPRC[i] |= coreMask;
    }
/**************************************************************************/
                      /* FPU ENABLE*/
/**************************************************************************/
#ifdef ENABLE_FPU
    /* Enable CP10 and CP11 coprocessors */
    S32_SCB->CPACR |= (S32_SCB_CPACR_CPx(10U, 3U) | S32_SCB_CPACR_CPx(11U, 3U)); 

    ASM_KEYWORD("dsb");
    ASM_KEYWORD("isb");
#endif /* ENABLE_FPU */

/**************************************************************************/
                      /* DEFAULT MEMORY ENABLE*/
/**************************************************************************/
    ASM_KEYWORD("dsb");
    ASM_KEYWORD("isb");

    /* Set default memory regions */
    for (regionNum = 0U; regionNum < CPU_DEFAULT_MEMORY_CNT; regionNum++)
    {
        S32_MPU->RNR  = regionNum;
        S32_MPU->RBAR = rbar[regionNum];
        S32_MPU->RASR = rasr[regionNum];
    }

    /* Enable MPU */
    S32_MPU->CTRL |= S32_MPU_CTRL_ENABLE_MASK;

    ASM_KEYWORD("dsb");
    ASM_KEYWORD("isb");
    
/**************************************************************************/
            /* ENABLE CACHE */
/**************************************************************************/
    sys_m7_cache_init();
}

__attribute__ ((section (".systeminit")))

static void sys_m7_cache_init(void)
{        
    uint32 ccsidr = 0U;
    uint32 sets = 0U;
    uint32 ways = 0U;

#ifdef D_CACHE_ENABLE    
    /*init Data caches*/
    S32_SCB->CSSELR = 0U;                       /* select Level 1 data cache */
    ASM_KEYWORD("dsb");
    ccsidr = S32_SCB->CCSIDR;
    sets = (uint32)(CCSIDR_SETS(ccsidr));
    do {
      ways = (uint32)(CCSIDR_WAYS(ccsidr));
      do {
        S32_SCB->DCISW = (((sets << SCB_DCISW_SET_Pos) & SCB_DCISW_SET_Msk) |
                      ((ways << SCB_DCISW_WAY_Pos) & SCB_DCISW_WAY_Msk)  );  
        ASM_KEYWORD("dsb");
      } while (ways-- != 0U);
    } while(sets-- != 0U);
    ASM_KEYWORD("dsb");
    S32_SCB->CCR |=  (uint32)SCB_CCR_DC_Msk;  /* enable D-Cache */
    ASM_KEYWORD("dsb");
    ASM_KEYWORD("isb");
#endif

#ifdef I_CACHE_ENABLE  
    /*init Code caches*/
    ASM_KEYWORD("dsb");
    ASM_KEYWORD("isb");
    S32_SCB->ICIALLU = 0UL;                     /* invalidate I-Cache */
    ASM_KEYWORD("dsb");
    ASM_KEYWORD("isb");
    S32_SCB->CCR |=  (uint32)SCB_CCR_IC_Msk;  /* enable I-Cache */
    ASM_KEYWORD("dsb");
    ASM_KEYWORD("isb");
#endif
}

#ifdef __ICCARM__ 
    #pragma default_function_attributes =
#endif

#ifdef __cplusplus
}
#endif

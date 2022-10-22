/*==================================================================================================
*   Project              : RTD AUTOSAR 4.4
*   Platform             : CORTEXM
*   Peripheral           : S32K3XX
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

/**
*   @file main.c
*
*   @addtogroup main_module main module documentation
*   @{
*/

/* Including necessary configuration files. */
#include "Mcal.h"


volatile int exit_code = 0;
/* User includes */

#include "Lpuart_Uart_Ip.h"
#include "Clock_Ip.h"
#include "IntCtrl_Ip.h"
#include "Siul2_Port_Ip.h"
#include "string.h"
/* Welcome message displayed at the console */
#define WELCOME_MSG "This example is an simple echo using LPUART\r\n\
The board will greet you if you send 'Hello Board'\r\
\nNow you can begin typing:\r\n"

/* Error message displayed at the console, in case data is received erroneously */
#define ERROR_MSG "An error occurred! The application will stop!\r\n"

#define EXPECT_RX_MSG "Hello Board\n"

/* Length of the message to be received from the console */
#define MSG_LEN  50U


/*!
  \brief The main function for the project.
  \details The startup initialization sequence is the following:
 * - startup asm routine
 * - main()
*/
int main(void)
{
    /* Write your code here */
	volatile Lpuart_Uart_Ip_StatusType driverStatus;
	uint32 varRemainingBytes;
	uint8 Rx_Buffer[MSG_LEN];
	uint8 Tx_Buffer[MSG_LEN];

    /* Init clock  */
    Clock_Ip_Init(&Mcu_aClockConfigPB[0]);

    /* Initialize all pins */
    Siul2_Port_Ip_Init(NUM_OF_CONFIGURED_PINS0, g_pin_mux_InitConfigArr0);

	/* Initialize IRQs */
	IntCtrl_Ip_Init(&IntCtrlConfig_0);
	IntCtrl_Ip_EnableIrq(LPUART6_IRQn);
	IntCtrl_Ip_InstallHandler(LPUART6_IRQn, LPUART_UART_IP_6_IRQHandler, NULL_PTR);

	/* Initializes an UART driver*/
	Lpuart_Uart_Ip_Init(LPUART_IP_6, &Lpuart_Uart_Ip_pHwConfigPB_0_BOARD_INITPERIPHERALS);
	/* Uart_AsyncSend transmit data */
	(void)Lpuart_Uart_Ip_AsyncSend(LPUART_IP_6, (const uint8 *)WELCOME_MSG, strlen(WELCOME_MSG));
	/* Wait for Uart successfully send data */
	while(Lpuart_Uart_Ip_GetTransmitStatus(LPUART_IP_6, &varRemainingBytes) == LPUART_UART_IP_STATUS_BUSY);

	/* Infinite loop:
	*     - Receive data from user
	*     - Echo the received data back
	*/
	while (1)
	{
		memset(Rx_Buffer, 0 , MSG_LEN);
		memset(Tx_Buffer, 0 , MSG_LEN);
		/* Receive and store data byte by byte until new line character is received,
		* or the buffer becomes full
		*/
		(void)Lpuart_Uart_Ip_AsyncReceive(LPUART_IP_6, Rx_Buffer, strlen(EXPECT_RX_MSG));
		/* Wait for transfer to be completed */
		while(Lpuart_Uart_Ip_GetReceiveStatus(LPUART_IP_6, &varRemainingBytes) == LPUART_UART_IP_STATUS_BUSY);

		/* Check the status */
		driverStatus = Lpuart_Uart_Ip_GetReceiveStatus(LPUART_IP_6, &varRemainingBytes);

		if (driverStatus != LPUART_UART_IP_STATUS_SUCCESS)
		{
			/* If an error occurred, send the error message and exit the loop */
			(void)Lpuart_Uart_Ip_AsyncSend(LPUART_IP_6, (const uint8 *)ERROR_MSG, strlen(ERROR_MSG));
			while(Lpuart_Uart_Ip_GetTransmitStatus(LPUART_IP_6, &varRemainingBytes) == LPUART_UART_IP_STATUS_BUSY);
			break;
		}

		/* If the received string is "Hello Board", send back "Hello World" */
		if(strcmp((const char *)EXPECT_RX_MSG, (const char *)Rx_Buffer) == 0)
		{
			strcpy((char *)Tx_Buffer, "Hello World\n");
			/* Send the received data back */
			Lpuart_Uart_Ip_AsyncSend(LPUART_IP_6, Tx_Buffer, strlen((char *)Tx_Buffer));
			while(Lpuart_Uart_Ip_GetTransmitStatus(LPUART_IP_6, &varRemainingBytes) == LPUART_UART_IP_STATUS_BUSY);
			break;
		}
	}
    for(;;)
    {
        if(exit_code != 0)
        {
            break;
        }
    }
    return exit_code;
}

/** @} */

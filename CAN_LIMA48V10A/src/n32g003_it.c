/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
*\*\file n32g003_it.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
**/
#include "n32g003_it.h"
#include "..\..\includeAll.h"
 //uint8_t TxBuffer1[];
 //uint8_t TxBuffer2[];
 //uint8_t RxBuffer1[];
 //uint8_t RxBuffer2[];
 __IO uint8_t TxCounter1;
 __IO uint8_t TxCounter2;
 __IO uint8_t RxCounter1;
 __IO uint8_t RxCounter2;
 uint8_t NbrOfDataToTransfer1;
 uint8_t NbrOfDataToTransfer2;
 uint8_t NbrOfDataToRead1;
 uint8_t NbrOfDataToRead2;

extern volatile uint32_t systick ;
extern volatile uint32_t systick_100us;
/** Cortex-M0 Processor Exceptions Handlers **/


/**
*\*\name    NMI_Handler.
*\*\fun     This function handles NMI exception.
*\*\param   none
*\*\return  none 
**/
void NMI_Handler(void)
{
}

/**
*\*\name    HardFault_Handler.
*\*\fun     This function handles Hard Fault exception.
*\*\param   none
*\*\return  none 
**/
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
*\*\name    SVC_Handler.
*\*\fun     This function handles SVC exception.
*\*\param   none
*\*\return  none 
**/
void SVC_Handler(void)
{
}

/**
*\*\name    PendSV_Handler.
*\*\fun     This function handles PendSV exception.
*\*\param   none
*\*\return  none 
**/
void PendSV_Handler(void)
{
}

/**
*\*\name    SysTick_Handler.
*\*\fun     This function handles SysTick Handler.
*\*\param   none
*\*\return  none 
**/
void SysTick_Handler(void)
{
}

/* N32G003 Peripherals Interrupt Handlers, interrupt handler's name please refer to the startup file (startup_n32g003.s). */

void TIM3_IRQHandler(void)
{static uint32_t frequencyDivision = 0;
   if (TIM_Interrupt_Status_Get(TIM3, TIM_INT_UPDATE) != RESET)
    {
        TIM_Interrupt_Status_Clear(TIM3, TIM_INT_UPDATE);
        frequencyDivision++;systick_100us++;
        if(frequencyDivision >= 100) {
        	frequencyDivision = 0; //10ms
        	systick++;
        }
        adcRunning();
    }
}

void UART1_IRQHandler(void)
{
    if (UART_Interrupt_Status_Get(UART1, UART_INT_RXDNE) != RESET)
    {
        /* Read one byte from the receive data register */
        uart_seriousInto((uint8_t)UART_Data_Receive(UART1));
    }

    if (UART_Interrupt_Status_Get(UART1, UART_INT_TXDE) != RESET)
    {
            UART_Interrput_Disable(UART1, UART_INT_TXDE);

    }
}

void UART2_IRQHandler(void){
	  //static unsigned char test_flag=1;
    if (UART_Interrupt_Status_Get(UART2, UART_INT_RXDNE) != RESET)
    {uint8_t temp;
        /* Read one byte from the receive data register */
			temp = (uint8_t)UART_Data_Receive(UART2);
      nns_rx_data_put(temp);
    }

    if (UART_Interrupt_Status_Get(UART2, UART_INT_TXDE) != RESET)
    {
            UART_Interrput_Disable(UART2, UART_INT_TXDE);

    }
		if(UART_Interrupt_Status_Get(UART2, UART_INT_OREF)!= RESET)
    {
			(void)UART2->STS;
			(void)UART2->DAT;
		}
		
}

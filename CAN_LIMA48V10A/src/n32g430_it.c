/**
*     Copyright (c) 2022, Nations Technologies Inc.
* 
*     All rights reserved.
*
*     This software is the exclusive property of Nations Technologies Inc. (Hereinafter 
* referred to as NATIONS). This software, and the product of NATIONS described herein 
* (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
* of the People’s Republic of China and other applicable jurisdictions worldwide.
*
*     NATIONS does not grant any license under its patents, copyrights, trademarks, or other 
* intellectual property rights. Names and brands of third party may be mentioned or referred 
* thereto (if any) for identification purposes only.
*
*     NATIONS reserves the right to make changes, corrections, enhancements, modifications, and 
* improvements to this software at any time without notice. Please contact NATIONS and obtain 
* the latest version of this software before placing orders.

*     Although NATIONS has attempted to provide accurate and reliable information, NATIONS assumes 
* no responsibility for the accuracy and reliability of this software.
* 
*     It is the responsibility of the user of this software to properly design, program, and test 
* the functionality and safety of any application made of this information and any resulting product. 
* In no event shall NATIONS be liable for any direct, indirect, incidental, special,exemplary, or 
* consequential damages arising in any way out of the use of this software or the Product.
*
*     NATIONS Products are neither intended nor warranted for usage in systems or equipment, any
* malfunction or failure of which may cause loss of human life, bodily injury or severe property 
* damage. Such applications are deemed, “Insecure Usage”.
*
*     All Insecure Usage shall be made at user’s risk. User shall indemnify NATIONS and hold NATIONS 
* harmless from and against all claims, costs, damages, and other liabilities, arising from or related 
* to any customer’s Insecure Usage.

*     Any express or implied warranty with regard to this software or the Product, including,but not 
* limited to, the warranties of merchantability, fitness for a particular purpose and non-infringement
* are disclaimed to the fullest extent permitted by law.

*     Unless otherwise explicitly permitted by NATIONS, anyone may not duplicate, modify, transcribe
* or otherwise distribute this software for any purposes, in whole or in part.
*
*     NATIONS products and technologies shall not be used for or incorporated into any products or systems
* whose manufacture, use, or sale is prohibited under any applicable domestic or foreign laws or regulations. 
* User shall comply with any applicable export control laws and regulations promulgated and administered by 
* the governments of any countries asserting jurisdiction over the parties or transactions.
**/

/**
*\*\file n32g430_it.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
**/

#include "includeAll.h"
#include "n32g430_it.h"
#include "main.h"


/** Cortex-M4 Processor Exceptions Handlers **/

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
*\*\name    MemManage_Handler.
*\*\fun     This function handles Memory Manage exception.
*\*\param   none
*\*\return  none 
**/
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
*\*\name    BusFault_Handler.
*\*\fun     This function handles Bus Fault exception.
*\*\param   none
*\*\return  none 
**/
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
*\*\name    UsageFault_Handler.
*\*\fun     This function handles Usage Fault exception.
*\*\param   none
*\*\return  none 
**/
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
*\*\name    SVC_Handler.
*\*\fun     This function handles SVCall exception.
*\*\param   none
*\*\return  none 
**/
void SVC_Handler(void)
{
}

/**
*\*\name    DebugMon_Handler.
*\*\fun     This function handles Debug Monitor exception.
*\*\param   none
*\*\return  none 
**/
void DebugMon_Handler(void)
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


/* N32G430 Peripherals Interrupt Handlers, interrupt handler's name please refer to the startup file (startup_n32g430.s). */

/**
*\*\name   CAN_RX0_IRQHandler
*\*\fun    CAN FIFO0 receive interrupt.
*\*\param  none
*\*\return none
**/
volatile unsigned char can_rx_flag=0;
void CAN_RX0_IRQHandler(void)
{
    /* Receives a message */
    CAN_Message_Receive(CAN,CAN_FIFO0, &CAN_RxMessage);
	  can_rx_flag=1;
}

extern volatile uint32_t systick ;
extern volatile uint32_t systick_100us;



void TIM3_IRQHandler(void)
{static uint32_t frequencyDivision = 0;
   if (TIM_Interrupt_Status_Get(TIM3, TIM_INT_UPDATE) != RESET)
    {
        TIM_Interrupt_Status_Clear(TIM3, TIM_INT_UPDATE);
        frequencyDivision++;systick_100us++;
        if(frequencyDivision >= 100) {
        	frequencyDivision = 0; //10ms
        	systick++;
				//	can_test();
//					GPIO_Pins_Set(GPIOA , GPIO_PIN_8);
        } 
//				else if(frequencyDivision==50){	 GPIO_Pins_Reset(GPIOA , GPIO_PIN_8);	 }
				
        adcRunning();
    }
}



void USART2_IRQHandler(void)
{
    if (USART_Interrupt_Status_Get(USART2, USART_INT_RXDNE) != RESET)
    {
       
        uart_seriousInto((uint8_t)USART_Data_Receive(USART2));
    }

    if (USART_Interrupt_Status_Get(USART2, USART_INT_TXDE) != RESET)
    {
            USART_Interrput_Disable(USART2, USART_INT_TXDE);

    }
}


void USART1_IRQHandler(void)
{
    if (USART_Interrupt_Status_Get(USART1, USART_INT_RXDNE) != RESET)
    {
      uint8_t temp;
			temp = (uint8_t)USART_Data_Receive(USART1);
      nns_rx_data_put(temp);
    }

    if (USART_Interrupt_Status_Get(USART1, USART_INT_TXDE) != RESET)
    {
            USART_Interrput_Disable(USART1, USART_INT_TXDE);

    }
}


/*
void UART1_IRQHandler(void)
{
    if (UART_Interrupt_Status_Get(UART1, UART_INT_RXDNE) != RESET)
    {
        
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

void USART2_IRQHandler(void)
{
    if (USART_Interrupt_Status_Get(USART2, USART_INT_RXDNE) != RESET)
    {
        
        RxBuffer1[RxCounter1++] = USART_Data_Receive(USART2);

        if (RxCounter1 == NbrOfDataToRead1)
        {
            
            USART_Interrput_Disable(USARTy, USART_INT_RXDNE);
        }
    }

    if (USART_Interrupt_Status_Get(USARTy, USART_INT_TXDE) != RESET)
    {
        
        USART_Data_Send(USARTy, TxBuffer1[TxCounter1++]);

        if (TxCounter1 == NbrOfDataToTransfer1)
        {
            
            USART_Interrput_Disable(USARTy, USART_INT_TXDE);
        }
    }
}
*/

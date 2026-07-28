/**
*     Copyright (c) 2022, Nations Technologies Inc.
* 
*     All rights reserved.
*
*     This software is the exclusive property of Nations Technologies Inc. (Hereinafter 
* referred to as NATIONS). This software, and the product of NATIONS described herein 
* (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
* of the People's Republic of China and other applicable jurisdictions worldwide.
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
* damage. Such applications are deemed, "Insecure Usage".
*
*     All Insecure Usage shall be made at user's risk. User shall indemnify NATIONS and hold NATIONS 
* harmless from and against all claims, costs, damages, and other liabilities, arising from or related 
* to any customer's Insecure Usage.

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
*\*\file can_config.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
**/ 

#include "can_config.h"

#define CAN_TXDLC_8    ((uint8_t)8)
#define CAN_FILTERNUM0 ((uint8_t)0)

CanTxMessage CAN_TxMessage; // 发送消息结构体
CanRxMessage CAN_RxMessage; // 接收消息结构体


CAN_InitType        CAN_InitStructure;
CAN_FilterInitType  CAN_FilterInitStructure;


/**
*\*\name   CAN_CONFIG
*\*\fun    CAN configure.
*\*\param  none
*\*\return none
**/


void GPIO_ADC_Configuration(void)
{
    GPIO_InitType GPIO_InitStructure;

    GPIO_Structure_Initialize(&GPIO_InitStructure);
    /* Configure adc input as analog input -------------------------*/
    GPIO_InitStructure.Pin       = GPIO_PIN_7 |GPIO_PIN_6 |GPIO_PIN_5 |GPIO_PIN_4 |GPIO_PIN_3 |GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_Peripheral_Initialize(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin       = GPIO_PIN_14 |GPIO_PIN_13 |GPIO_PIN_12 |GPIO_PIN_11 |GPIO_PIN_10 |GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_Peripheral_Initialize(GPIOB, &GPIO_InitStructure);
}




void CAN_CONFIG(void)
{
    RCC_APB1_Peripheral_Clock_Enable(RCC_APB1_PERIPH_CAN);
	
		RCC_AHB_Peripheral_Clock_Enable(RCC_AHB_PERIPH_GPIOB);
	  RCC_AHB_Peripheral_Clock_Enable(RCC_AHB_PERIPH_GPIOA);
	  RCC_APB2_Peripheral_Clock_Enable(RCC_APB2_PERIPH_AFIO);
    
    /* Initializes CAN*/
    CAN_NVIC_Configuration();
    CAN_GPIO_Configuration();
	  //GPIO_Pins_Set(GPIOA , GPIO_PIN_8); // disable tx.
    
    CAN_Reset(CAN);
    CAN_Structure_Initializes(&CAN_InitStructure);
    /* CAN cell init */
    CAN_InitStructure.TTCM          = DISABLE;
    CAN_InitStructure.ABOM          = DISABLE;
    CAN_InitStructure.AWKUM         = DISABLE;
    CAN_InitStructure.NART          = DISABLE;
    CAN_InitStructure.RFLM          = DISABLE;
    CAN_InitStructure.TXFP          = ENABLE;
    CAN_InitStructure.OperatingMode = CAN_NORMAL_MODE;
    /* Baud rate calculation = CAN clock/(BaudRatePrescaler*(TBS1+TBS2+1)) =48/4/3/16=0.25M */
    CAN_InitStructure.RSJW          = CAN_RSJW_1TQ;
    CAN_InitStructure.TBS1          = CAN_TBS1_8TQ;
    CAN_InitStructure.TBS2          = CAN_TBS2_7TQ; 
    CAN_InitStructure.BaudRatePrescaler = 6;
    
    /* Initializes the CAN */
    CAN_Initializes(CAN, &CAN_InitStructure); 
    
    
    /* CAN filter init */
	
    CAN_FilterInitStructure.Filter_Num            = 0;
    CAN_FilterInitStructure.Filter_Mode           = CAN_FILTER_IDMASKMODE;
    CAN_FilterInitStructure.Filter_Scale          = CAN_FILTER_32BITSCALE;
    CAN_FilterInitStructure.Filter_HighId         = CAN_STD_ID_LIST_32BIT_H(0x00000400);
    CAN_FilterInitStructure.Filter_LowId          = CAN_STD_ID_LIST_32BIT_L(0x00000400);
    CAN_FilterInitStructure.FilterMask_HighId     = CAN_STD_ID_LIST_32BIT_H(0x00000000);
    CAN_FilterInitStructure.FilterMask_LowId      = CAN_STD_ID_LIST_32BIT_L(0x00000000);
    CAN_FilterInitStructure.Filter_FIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.Filter_Act            = ENABLE;
    CAN_Filter_Initializes(&CAN_FilterInitStructure);
	
    /* IT Configuration for CAN */  
    CAN_Config_Interrupt_Enable(CAN, CAN_INT_FMP0);
}


/**
*\*\name   CAN_NVIC_Configuration
*\*\fun    CAN NVIC configure.
*\*\param  none
*\*\return none
**/
void CAN_NVIC_Configuration(void)
{
    NVIC_InitType NVIC_InitStructure;
    
    /* NVIC priority group set */
    NVIC_Priority_Group_Set(NVIC_PER2_SUB2_PRIORITYGROUP);
    
    /* NVIC configure */
    NVIC_InitStructure.NVIC_IRQChannel                   = CAN_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Initializes(&NVIC_InitStructure);
}


/**
*\*\name   CAN_GPIO_Configuration
*\*\fun    CAN GPIO configure.
*\*\param  none
*\*\return none
**/
void CAN_GPIO_Configuration(void)
{
    GPIO_InitType GPIO_InitStructure;

    /* Initializes the GPIO */
    GPIO_Structure_Initialize(&GPIO_InitStructure);
    
    /* configure CAN pin pa11 rx pa12 tx*/
		RCC_AHB_Peripheral_Clock_Enable(RCC_AHB_PERIPH_GPIOB);
		RCC_AHB_Peripheral_Clock_Enable(RCC_AHB_PERIPH_GPIOA);
		RCC_APB2_Peripheral_Clock_Enable(RCC_APB2_PERIPH_AFIO);

    GPIO_InitStructure.Pin            = GPIO_PIN_11;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF2_CAN;
    GPIO_Peripheral_Initialize(GPIOA, &GPIO_InitStructure);


    GPIO_InitStructure.Pin             = GPIO_PIN_12;
    GPIO_InitStructure.GPIO_Mode       = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate  = GPIO_AF2_CAN;
    GPIO_Peripheral_Initialize(GPIOA, &GPIO_InitStructure);
}


/**
*\*\name    Check_CanRecData
*\*\fun     Check Can Receive Date
*\*\param   receive_message :
*\*\            - StdId
*\*\                - It ranges from 0 to 0x7FF
*\*\            - ExtId
*\*\                - It ranges from 0 to 0x1FFFFFFF
*\*\            - IDE
*\*\                - CAN_STANDARD_ID
*\*\                - CAN_EXTENDED_ID
*\*\            - RTR
*\*\                - CAN_RTRQ_DATA
*\*\                - CAN_RTRQ_REMOTE
*\*\            - DLC
*\*\                - It ranges from 0 to 8
*\*\            - Data[0-7]
*\*\                - It ranges from 0 to 0xFF
*\*\            - FMI
*\*\                - It ranges from 0 to 0xFF             
*\*\return  Pass / Fail
**/
uint8_t Check_CanRecData(CanRxMessage* RxMessage, uint32_t StdId, uint32_t ExtId, uint8_t IDE, uint8_t RTR, uint8_t DLC,
                         uint8_t Data0, uint8_t Data1, uint8_t Data2, uint8_t Data3,
                         uint8_t Data4, uint8_t Data5, uint8_t Data6, uint8_t Data7, uint8_t FMI)
{
    /* Receive */

    /* ID */
    if(IDE == CAN_EXTENDED_ID)
    {
        if(RxMessage->ExtId   != ExtId)          
        {
            return Fail;
        }
    }
    else if(IDE == CAN_STANDARD_ID)
    {
        if(RxMessage->StdId   != StdId)  
        {
            return Fail;
        }
    }
    /* **** */

    /* IDE/RTR/DLC */
    if( (RxMessage->IDE     != IDE)   ||         /* CAN_ID_STD / CAN_ID_EXT */
        (RxMessage->RTR     != RTR)   ||         /* CAN_RTR_DATA / CAN_RTR_REMOTE */
        (RxMessage->DLC     != DLC)              /* 0 to 8 */
    )
    {
        return Fail;
    }
    /* **** */

    /* DATA */
    if(RTR == CAN_RTRQ_DATA)
    {
        if(DLC >= 1)
        {
            if(RxMessage->Data[0] != Data0)
            {
                return Fail;
            }
        }
        if(DLC >= 2)
        {
            if(RxMessage->Data[1] != Data1)
            {
                return Fail;
            }
        }
        if(DLC >= 3)
        {
            if(RxMessage->Data[2] != Data2)
            {
                return Fail;
            }
        }
        if(DLC >= 4)
        {
            if(RxMessage->Data[3] != Data3)
            {
                return Fail;
            }
        }
        if(DLC >= 5)
        {
            if(RxMessage->Data[4] != Data4)
            {
                return Fail;
            }
        }
        if(DLC >= 6)
        {
            if(RxMessage->Data[5] != Data5)
            {
                return Fail;
            }
        }
        if(DLC >= 7)
        {
            if(RxMessage->Data[6] != Data6)
            {
                return Fail;
            }
        }
        if(DLC == 8)
        {
            if(RxMessage->Data[7] != Data7)
            {
                return Fail;
            }
        }
        if(DLC > 8)
        {
            return Fail;
        }
    }
    else if(RTR == CAN_RTRQ_REMOTE)
    {
        
    }

    /* FMI */
    if(RxMessage->FMI != FMI)           
    {
        return Fail;
    }

    return Pass;
}


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
*\*\file n32g430_wwdg.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
**/

#include "n32g430_wwdg.h"
#include "n32g430_rcc.h"


/**
*\*\name    WWDG_Reset.
*\*\fun     Resets the WWDG peripheral registers to their default reset values.
*\*\return  none
**/
void WWDG_Reset(void)
{
    RCC_APB1_Peripheral_Reset(RCC_APB1_PERIPH_WWDG);
}


/**
*\*\name    WWDG_Prescaler_Division_Set.
*\*\fun     Set the WWDOG Prescaler Division Value.
*\*\param   prescaler_division : 
*\*\          - WWDG_PRESCALER_DIV1   WWDG Counter Clock (PCLK1 / 4096) / 1
*\*\          - WWDG_PRESCALER_DIV2   WWDG Counter Clock (PCLK1 / 4096) / 2
*\*\          - WWDG_PRESCALER_DIV4   WWDG Counter Clock (PCLK1 / 4096) / 4
*\*\          - WWDG_PRESCALER_DIV8   WWDG Counter Clock (PCLK1 / 4096) / 8
*\*\return  none
**/
void WWDG_Prescaler_Division_Set(uint32_t prescaler_division)
{
    uint32_t temp_value = 0;

    /* Clear TIMERB[1:0] bits */
    temp_value = WWDG->CFG & CFG_TIMERB_MASK;

    /* Set TIMERB[1:0] bits according to prescaler_division value */
    temp_value |= prescaler_division;

    /* Store the new value */
    WWDG->CFG = temp_value;
}


/**
*\*\name    WWDG_Window_Value_Set.
*\*\fun     Set the WWDOG Window Value.
*\*\param   window_value : WWDOG Window Value
*\*\          The value range of this parameter :
*\*\          - 0x0040 ~ 0x3FFF
*\*\return  none
**/
void WWDG_Window_Value_Set(uint16_t window_value)
{
    __IO uint32_t temp_value = 0;

    /* Clear W[13:0] bits */
    temp_value = WWDG->CFG & CFG_W_MASK;

    /* Set W[13:0] bits according to window_value value */
    temp_value |= window_value & (uint32_t)BIT_MASK;

    /* Store the new value */
    WWDG->CFG = temp_value;
}


/**
*\*\name    WWDG_Interrupt_Enable.
*\*\fun     Enable WWDG Early Wakeup interrupt(EWINT).
*\*\return  none
**/
void WWDG_Interrupt_Enable(void)
{
    *(__IO uint32_t*)CFG_EWINT_BB = (uint32_t)ENABLE;
}


/**
*\*\name    WWDG_Counter_Value_Set.
*\*\fun     Set the WWDOG Counter Value.
*\*\param   counter_value : WWDOG Counter value
*\*\          The value range of this parameter :
*\*\          - 0x40 ~ 0x3FFF
*\*\return  none 
**/
void WWDG_Counter_Value_Set(uint16_t counter_value)
{
    uint32_t temp_value = 0x00U;
    temp_value = (counter_value & BIT_MASK);
    /* Write the T[13:0] bits to configure the counter value, which can be written directly 
       without read-modify-write; only write 1 to the ACTB bit to activate the window watchdog */
    WWDG->CTRL = temp_value;
}


/**
*\*\name    WWDG_Enable.
*\*\fun     Set the WWDOG Counter Value and Enable WWDOG .
*\*\param   counter_value : WWDOG Counter value
*\*\          The value range of this parameter :
*\*\          - 0x40 ~ 0x3FFF
*\*\return  none
**/
void WWDG_Enable(uint16_t counter_value)
{
    WWDG->CTRL = CTRL_ACTB_SET | counter_value;
}



/**
*\*\name    WWDG_EWINTF_Flag_Get.
*\*\fun     Get WWDOG Early Wake-up Interrupt Flag.
*\*\return  SET or RESET
**/
FlagStatus WWDG_EWINTF_Flag_Get(void)
{
    return (FlagStatus)(WWDG->STS);
}

/**
*\*\name    WWDG_EWINTF_Flag_Clear.
*\*\fun     Clear WWDOG Early Wake-up Interrupt Flag.
*\*\return  none
**/
void WWDG_EWINTF_Flag_Clear(void)
{
    WWDG->STS = (uint32_t)RESET;
}

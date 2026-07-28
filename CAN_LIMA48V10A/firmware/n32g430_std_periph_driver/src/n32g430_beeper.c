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
*\*\file n32g430_beeper.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
**/

#include "n32g430_beeper.h"
#include "n32g430_rcc.h"

/**
*\*\name    BEEPER_Reset.
*\*\fun     Reset the BEEPER register to it's default reset values.
*\*\param   none.
*\*\return  none. 
**/
void BEEPER_Reset(void)
{
    RCC_APB2_Peripheral_Reset(RCC_APB2_PERIPH_BEEPER);   
}

/**
*\*\name    BEEPER_Clock_Select.
*\*\fun     Beeper clock selection.
*\*\param   clock_selection:  
*\*\          - BEEPER_CLK_SOURCE_APB 
*\*\          - BEEPER_CLK_SOURCE_LSI 
*\*\          - BEEPER_CLK_SOURCE_LSE 
*\*\return  none. 
**/
void BEEPER_Clock_Select(uint32_t clock_selection)
{
    BEEPER->CTRL &= BEEPER_CLK_MASK;
    BEEPER->CTRL |= clock_selection;    
}

/**
*\*\name    BEEPER_APB_Clock_Prescale_Set.
*\*\fun     Set BEEPER APB clock prescale factor.
*\*\param   prescale_factor:  if LSI or LSE clock is selected, this parameter is ignored
*\*\          - 0 ~ 63  if prescale_factor > 0 and prescale_factor < 64, the APB clock frequency is divided by (prescale_factor + 1)
*\*\return  none. 
**/
void BEEPER_APB_Clock_Prescale_Set(uint32_t prescale_factor)
{
    BEEPER->CTRL &= BEEPER_FREQUENCY_MASK;
    BEEPER->CTRL |= (prescale_factor << BEEPER_PSC_BIT_MASK);
}

/**
*\*\name    BEEPER_Div_Factor_Select.
*\*\fun     Beeper division factor selection.
*\*\param   div_factor:  
*\*\          - 0 ~ 511  if div_factor is x, output frequency value divided by (x + 1) * 2
*\*\return  none. 
**/
void BEEPER_Div_Factor_Select(uint32_t div_factor)
{
    BEEPER->CTRL &= BEEPER_DIV_MASK;
    BEEPER->CTRL |= (div_factor << BEEPER_BEEPDIV_BIT_MASK);
}

/**
*\*\name    BEEPER_Initialize.
*\*\fun     Init Beeper.
*\*\param   clock_selection:  
*\*\          - BEEPER_CLK_SOURCE_APB 
*\*\          - BEEPER_CLK_SOURCE_LSI 
*\*\          - BEEPER_CLK_SOURCE_LSE
*\*\param   prescale_factor:  if LSI or LSE clock is selected, this parameter is ignored
*\*\          - 0 ~ 63  if prescale_factor > 0 and prescale_factor < 64, the APB clock frequency is divided by (prescale_factor + 1)
*\*\param   div_factor:  
*\*\          - 0 ~ 511  if div_factor is x, output frequency value divided by (x + 1) * 2
*\*\return  none. 
**/
void BEEPER_Initialize(uint32_t clock_selection, uint32_t prescale_factor, uint32_t div_factor)
{
    BEEPER_Clock_Select(clock_selection);
    BEEPER_APB_Clock_Prescale_Set(prescale_factor);
    BEEPER_Div_Factor_Select(div_factor);
}


/**
*\*\name    BEEPER_Bypass_Clock_Signal.
*\*\fun     Bypass the selected clock signal to the output ports.
*\*\param   none. 
*\*\return  none. 
**/
void BEEPER_Bypass_Clock_Signal(void)
{
    BEEPER->CTRL |= BEEPER_BYPASS;
}


/**
*\*\name    BEEPER_Inverted_Enable.
*\*\fun     Beeper complementary output enable.
*\*\param   none. 
*\*\return  none. 
**/
void BEEPER_Inverted_Enable(void)
{
    BEEPER->CTRL |= BEEPER_INV_ENABLE;
}

/**
*\*\name    BEEPER_Inverted_Disable.
*\*\fun     Beeper complementary output disable.
*\*\param   none. 
*\*\return  none. 
**/
void BEEPER_Inverted_Disable(void)
{
    BEEPER->CTRL &= BEEPER_INV_DISABLE;
}


/**
*\*\name    BEEPER_Enable.
*\*\fun     Enable beeper.
*\*\param   none. 
*\*\return  none. 
**/
void BEEPER_Enable(void)
{
    BEEPER->CTRL |= BEEPER_ENABLE;
}

/**
*\*\name    BEEPER_Disable.
*\*\fun     Disable beeper.
*\*\param   none. 
*\*\return  none. 
**/
void BEEPER_Disable(void)
{
    BEEPER->CTRL &= BEEPER_DISABLE;
}



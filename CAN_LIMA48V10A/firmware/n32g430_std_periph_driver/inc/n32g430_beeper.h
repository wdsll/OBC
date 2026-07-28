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
*\*\file n32g430_beeper.h
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
**/

#ifndef __N32G430_BEEPER_H__
#define __N32G430_BEEPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32g430.h"

/* Beeper clock source */
#define BEEPER_CLK_SOURCE_APB        (BEEPER_CTRL_CLKSEL_0)  /* APB clock */
#define BEEPER_CLK_SOURCE_LSI        (BEEPER_CTRL_CLKSEL_1)  /* LSI clock */
#define BEEPER_CLK_SOURCE_LSE        (BEEPER_CTRL_CLKSEL)    /* LSE clock */


#define BEEPER_ENABLE                (BEERPER_CTRL_BEEPEN)   /* Enable Beeper */
#define BEEPER_DISABLE               (~BEERPER_CTRL_BEEPEN)  /* Disable Beeper */

/* Beeper reverse enable */
#define BEEPER_INV_ENABLE            (BEERPER_CTRL_INVEN)    /* Only one output, the other output is off */
#define BEEPER_INV_DISABLE           (~BEERPER_CTRL_INVEN)   /* Both outputs are on, and the outputs are complementary */

/* Bypass the selected clock signal to the output ports define */
#define BEEPER_BYPASS                (BEEPER_CTRL_BYPASSEN)

/* Mask Define */
#define BEEPER_DIV_MASK              (~BEEPER_CTRL_BEEPDIV)
#define BEEPER_CLK_MASK              (~BEEPER_CTRL_CLKSEL)
#define BEEPER_FREQUENCY_MASK        (~BEEPER_CTRL_PSC)

#define BEEPER_PSC_BIT_MASK          (16U)
#define BEEPER_BEEPDIV_BIT_MASK      (4U)

void BEEPER_Reset(void);
void BEEPER_Clock_Select(uint32_t clock_selection);
void BEEPER_APB_Clock_Prescale_Set(uint32_t prescale_factor);
void BEEPER_Div_Factor_Select(uint32_t div_factor);
void BEEPER_Initialize(uint32_t clock_selection, uint32_t prescale_factor, uint32_t div_factor);
void BEEPER_Bypass_Clock_Signal(void);
void BEEPER_Inverted_Enable(void);
void BEEPER_Inverted_Disable(void);
void BEEPER_Enable(void);
void BEEPER_Disable(void);

#ifdef __cplusplus
}
#endif

#endif /* __N32G430_BEEPER_H__ */


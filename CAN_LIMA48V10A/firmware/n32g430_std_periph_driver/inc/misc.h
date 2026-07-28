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
*\*\file misc.h
*\*\author Nations
*\*\version v1.0.2
*\*\copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
**/

#ifndef __MISC_H__
#define __MISC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32g430.h"

/** MISC Driving Functions Declaration **/

/** NVIC Init Structure definition **/
typedef struct
{
    uint8_t NVIC_IRQChannel; /* Specifies the IRQ channel to be enabled or disabled. */

    uint8_t NVIC_IRQChannelPreemptionPriority; /* Specifies the pre-emption priority for the IRQ channel
                                                  specified in NVIC_IRQChannel. */

    uint8_t NVIC_IRQChannelSubPriority; /* Specifies the subpriority level for the IRQ channel specified
                                           in NVIC_IRQChannel. */

    FunctionalState NVIC_IRQChannelCmd; /* Specifies whether the IRQ channel defined in NVIC_IRQChannel
                                           will be enabled or disabled. */
} NVIC_InitType;

/** MISC driver modules **/
#define AIRCR_VECTKEY_MASK       ((uint32_t)0x05FA0000) /* access key */

/** Vector_Table_Base **/
#define NVIC_VECTTAB_RAM         ((uint32_t)0x20000000) /* RAM start address*/
#define NVIC_VECTTAB_FLASH       ((uint32_t)0x08000000) /* FLASH start address*/
#define NVIC_VECTTAB_MASK        (SCB_VTOR_TBLOFF)

/** System_Low_Power **/
#define NVIC_LP_SEVONPEND        (SCB_SCR_SEVONPEND)
#define NVIC_LP_SLEEPDEEP        (SCB_SCR_SLEEPDEEP)
#define NVIC_LP_SLEEPONEXIT      (SCB_SCR_SLEEPONEXIT)

/** Preemption_Sub_Priority_Group **/
#define NVIC_PER0_SUB4_PRIORITYGROUP (SCB_AIRCR_PRIGROUP7) /* 0 bits for pre-emption priority 4 bits for subpriority */
#define NVIC_PER1_SUB3_PRIORITYGROUP (SCB_AIRCR_PRIGROUP6) /* 1 bits for pre-emption priority 3 bits for subpriority */
#define NVIC_PER2_SUB2_PRIORITYGROUP (SCB_AIRCR_PRIGROUP5) /* 2 bits for pre-emption priority 2 bits for subpriority */
#define NVIC_PER3_SUB1_PRIORITYGROUP (SCB_AIRCR_PRIGROUP4) /* 3 bits for pre-emption priority 1 bits for subpriority */
#define NVIC_PER4_SUB0_PRIORITYGROUP (SCB_AIRCR_PRIGROUP3) /* 4 bits for pre-emption priority 0 bits for subpriority */
#define NVIC_PRIORITYGROUP_MASK      (SCB_AIRCR_PRIGROUP)  /* PRIGROUP[2:0] bits (Priority group) */
#define NVIC_IRQCHANNEL_MASK         ((uint8_t)0x1F) 
#define NVIC_ISER_BIT_LENGTH         ((uint8_t)0x05)
#define NVIC_PRE_SUB_SUM             ((uint8_t)0x04)
#define SCB_AIRCR_OFFSET             ((uint8_t)REG_BIT8_OFFSET) 
#define NVIC_IP_OFFSET               ((uint8_t)REG_BIT4_OFFSET) 

/** Preemption_Priority_Group **/
#define NVIC_PER_PRIORITY_0           ((uint8_t)0x00)
#define NVIC_PER_PRIORITY_1           ((uint8_t)0x01)
#define NVIC_PER_PRIORITY_2           ((uint8_t)0x02)
#define NVIC_PER_PRIORITY_3           ((uint8_t)0x03)
#define NVIC_PER_PRIORITY_4           ((uint8_t)0x04)
#define NVIC_PER_PRIORITY_5           ((uint8_t)0x05)
#define NVIC_PER_PRIORITY_6           ((uint8_t)0x06)
#define NVIC_PER_PRIORITY_7           ((uint8_t)0x07)
#define NVIC_PER_PRIORITY_8           ((uint8_t)0x08)
#define NVIC_PER_PRIORITY_9           ((uint8_t)0x09)
#define NVIC_PER_PRIORITY_10          ((uint8_t)0x0A)
#define NVIC_PER_PRIORITY_11          ((uint8_t)0x0B)
#define NVIC_PER_PRIORITY_12          ((uint8_t)0x0C)
#define NVIC_PER_PRIORITY_13          ((uint8_t)0x0D)
#define NVIC_PER_PRIORITY_14          ((uint8_t)0x0E)
#define NVIC_PER_PRIORITY_15          ((uint8_t)0x0F)

/** Sub_Priority_Group **/
#define NVIC_SUB_PRIORITY_0           ((uint8_t)0x00)
#define NVIC_SUB_PRIORITY_1           ((uint8_t)0x01)
#define NVIC_SUB_PRIORITY_2           ((uint8_t)0x02)
#define NVIC_SUB_PRIORITY_3           ((uint8_t)0x03)
#define NVIC_SUB_PRIORITY_4           ((uint8_t)0x04)
#define NVIC_SUB_PRIORITY_5           ((uint8_t)0x05)
#define NVIC_SUB_PRIORITY_6           ((uint8_t)0x06)
#define NVIC_SUB_PRIORITY_7           ((uint8_t)0x07)
#define NVIC_SUB_PRIORITY_8           ((uint8_t)0x08)
#define NVIC_SUB_PRIORITY_9           ((uint8_t)0x09)
#define NVIC_SUB_PRIORITY_10          ((uint8_t)0x0A)
#define NVIC_SUB_PRIORITY_11          ((uint8_t)0x0B)
#define NVIC_SUB_PRIORITY_12          ((uint8_t)0x0C)
#define NVIC_SUB_PRIORITY_13          ((uint8_t)0x0D)
#define NVIC_SUB_PRIORITY_14          ((uint8_t)0x0E)
#define NVIC_SUB_PRIORITY_15          ((uint8_t)0x0F)

/** SysTick_clock_source **/
#define SYSTICK_CLKSOURCE_HCLK_DIV8  (~SysTick_CTRL_CLKSOURCE)
#define SYSTICK_CLKSOURCE_HCLK       (SysTick_CTRL_CLKSOURCE)

/** MISC_Exported_Functions **/
void NVIC_Priority_Group_Set(uint32_t NVIC_priority_group);
void NVIC_Initializes(NVIC_InitType* NVIC_structure_initializes);
void NVIC_Vector_Table_Set(uint32_t NVIC_vecter_table, uint32_t offset);
void NVIC_System_LowPower_Enable(uint8_t low_power_mode);
void NVIC_System_Low_Power_Disable(uint8_t low_power_mode);
void SysTick_Clock_Source_Set(uint32_t systick_clock_source);

#ifdef __cplusplus
}
#endif

#endif /* __MISC_H__ */


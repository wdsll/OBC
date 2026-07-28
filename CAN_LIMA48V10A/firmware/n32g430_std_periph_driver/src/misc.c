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
*\*\file misc.c
*\*\author Nations
*\*\version v1.0.2
*\*\copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
**/
#include "misc.h"

/** MISC Driving Functions Declaration **/

/**
*\*\name    NVIC_Priority_Group_Set
*\*\fun     Configures the priority grouping: pre-emption priority and subpriority.
*\*\param   NVIC_priority_group :
*\*\            - NVIC_PER0_SUB4_PRIORITYGROUP    0 bits for pre-emption priority 4 bits for subpriority
*\*\            - NVIC_PER1_SUB3_PRIORITYGROUP    1 bits for pre-emption priority 3 bits for subpriority
*\*\            - NVIC_PER2_SUB2_PRIORITYGROUP    2 bits for pre-emption priority 2 bits for subpriority
*\*\            - NVIC_PER3_SUB1_PRIORITYGROUP    3 bits for pre-emption priority 1 bits for subpriority
*\*\            - NVIC_PER4_SUB0_PRIORITYGROUP    4 bits for pre-emption priority 0 bits for subpriority
*\*\return  none
**/
void NVIC_Priority_Group_Set(uint32_t NVIC_priority_group)
{
    /* Set the PRIGROUP[10:8] bits according to NVIC_priority_group value */
    SCB->AIRCR = AIRCR_VECTKEY_MASK | NVIC_priority_group;
}

/**
*\*\name    NVIC_Initializes
*\*\fun     Initializes the NVIC peripheral according to the specified parameters in the NVIC_structure_initializes.
*\*\param   NVIC_structure_initializes :
*\*\              - NVIC_IRQChannel :
*\*\                   -  IRQn_Type    N32G430 Interrupt Number Definition
*\*\        if NVIC_priority_group is NVIC_PER0_SUB4_PRIORITYGROUP :
*\*\              - NVIC_IRQChannelPreemptionPriority :
*\*\                   -  NVIC_PER_PRIORITY_0
*\*\              - NVIC_IRQChannelSubPriority :
*\*\                   -  NVIC_SUB_PRIORITY_0
*\*\                   -  NVIC_SUB_PRIORITY_1
*\*\                   -  NVIC_SUB_PRIORITY_2
*\*\                   -  NVIC_SUB_PRIORITY_3
*\*\                   -  NVIC_SUB_PRIORITY_4
*\*\                   -  NVIC_SUB_PRIORITY_5
*\*\                   -  NVIC_SUB_PRIORITY_6
*\*\                   -  NVIC_SUB_PRIORITY_7
*\*\                   -  NVIC_SUB_PRIORITY_8
*\*\                   -  NVIC_SUB_PRIORITY_9
*\*\                   -  NVIC_SUB_PRIORITY_10
*\*\                   -  NVIC_SUB_PRIORITY_11
*\*\                   -  NVIC_SUB_PRIORITY_12
*\*\                   -  NVIC_SUB_PRIORITY_13
*\*\                   -  NVIC_SUB_PRIORITY_14
*\*\                   -  NVIC_SUB_PRIORITY_15
*\*\        if NVIC_priority_group is NVIC_PER1_SUB3_PRIORITYGROUP :
*\*\              - NVIC_IRQChannelPreemptionPriority :
*\*\                   -  NVIC_PER_PRIORITY_0
*\*\                   -  NVIC_PER_PRIORITY_1
*\*\              - NVIC_IRQChannelSubPriority :
*\*\                   -  NVIC_SUB_PRIORITY_0 to NVIC_SUB_PRIORITY_7
*\*\        if NVIC_priority_group is NVIC_PER2_SUB2_PRIORITYGROUP :
*\*\              - NVIC_IRQChannelPreemptionPriority :
*\*\                   -  NVIC_PER_PRIORITY_0 to NVIC_PER_PRIORITY_3
*\*\              - NVIC_IRQChannelSubPriority :
*\*\                   -  NVIC_SUB_PRIORITY_0 to NVIC_SUB_PRIORITY_3
*\*\        if NVIC_priority_group is NVIC_PER3_SUB1_PRIORITYGROUP :
*\*\              - NVIC_IRQChannelPreemptionPriority :
*\*\                   -  NVIC_PER_PRIORITY_0 to NVIC_PER_PRIORITY_7
*\*\              - NVIC_IRQChannelSubPriority :
*\*\                   -  NVIC_SUB_PRIORITY_0
*\*\                   -  NVIC_SUB_PRIORITY_1
*\*\        if NVIC_priority_group is NVIC_PER4_SUB0_PRIORITYGROUP :
*\*\              - NVIC_IRQChannelPreemptionPriority :
*\*\                   -  NVIC_PER_PRIORITY_0
*\*\                   -  NVIC_PER_PRIORITY_1
*\*\                   -  NVIC_PER_PRIORITY_2
*\*\                   -  NVIC_PER_PRIORITY_3
*\*\                   -  NVIC_PER_PRIORITY_4
*\*\                   -  NVIC_PER_PRIORITY_5
*\*\                   -  NVIC_PER_PRIORITY_6
*\*\                   -  NVIC_PER_PRIORITY_7
*\*\                   -  NVIC_PER_PRIORITY_8
*\*\                   -  NVIC_PER_PRIORITY_9
*\*\                   -  NVIC_PER_PRIORITY_10
*\*\                   -  NVIC_PER_PRIORITY_11
*\*\                   -  NVIC_PER_PRIORITY_12
*\*\                   -  NVIC_PER_PRIORITY_13
*\*\                   -  NVIC_PER_PRIORITY_14
*\*\                   -  NVIC_PER_PRIORITY_15
*\*\              - NVIC_IRQChannelSubPriority :
*\*\                   -  NVIC_SUB_PRIORITY_0
*\*\              - NVIC_IRQChannelCmd :
*\*\                   -  ENABLE
*\*\                   -  DISABLE
*\*\return  none
**/
void NVIC_Initializes(NVIC_InitType* NVIC_structure_initializes)
{
    uint32_t temp_priority_value = 0x00, temp_pre_value = 0x00, temp_sub_value = 0x0F;

    if (NVIC_structure_initializes->NVIC_IRQChannelCmd != DISABLE)
    {
        /* Compute the Corresponding IRQ Priority */
        temp_priority_value = \
        (NVIC_PRIORITYGROUP_MASK - ((SCB->AIRCR) & NVIC_PRIORITYGROUP_MASK)) >> SCB_AIRCR_OFFSET;
        
        temp_pre_value      = (NVIC_PRE_SUB_SUM - temp_priority_value);
        temp_sub_value      = temp_sub_value >> temp_priority_value;

        temp_priority_value = \
        (uint32_t)NVIC_structure_initializes->NVIC_IRQChannelPreemptionPriority << temp_pre_value;

        temp_priority_value |= \
        NVIC_structure_initializes->NVIC_IRQChannelSubPriority & temp_sub_value;

        temp_priority_value = temp_priority_value << NVIC_IP_OFFSET;

        NVIC->IP[NVIC_structure_initializes->NVIC_IRQChannel] = temp_priority_value;

        /* Enable the Selected IRQ Channels */
        NVIC->ISER[NVIC_structure_initializes->NVIC_IRQChannel >> NVIC_ISER_BIT_LENGTH] = \
        (uint32_t)0x01 << (NVIC_structure_initializes->NVIC_IRQChannel & NVIC_IRQCHANNEL_MASK);
    }
    else
    {
        /* Disable the Selected IRQ Channels */
        NVIC->ICER[NVIC_structure_initializes->NVIC_IRQChannel >> NVIC_ISER_BIT_LENGTH] = \
        (uint32_t)0x01 << (NVIC_structure_initializes->NVIC_IRQChannel & NVIC_IRQCHANNEL_MASK);
    }
}

/**
*\*\name    NVIC_Vector_Table_Set
*\*\fun     Sets the vector table location and Offset.
*\*\param   NVIC_vecter_table specifies if the vector table is in RAM or FLASH memory.
*\*\        This parameter can be one of the following values:
*\*\            - NVIC_VECTTAB_RAM
*\*\            - NVIC_VECTTAB_FLASH
*\*\param   offset Vector Table base offset field. This value must be a multiple of 0x200.
*\*\return  none
**/
void NVIC_Vector_Table_Set(uint32_t NVIC_vecter_table, uint32_t offset)
{
    SCB->VTOR = NVIC_vecter_table | (offset & (uint32_t)NVIC_VECTTAB_MASK);
}

/**
*\*\name    NVIC_System_Low_Power_Enable
*\*\fun     Selects the condition for the system to enter low power mode.
*\*\param   low_power_mode Specifies the new mode for the system to enter low power mode.
*\*\        This parameter can be one of the following values:
*\*\            - NVIC_LP_SEVONPEND
*\*\            - NVIC_LP_SLEEPDEEP
*\*\            - NVIC_LP_SLEEPONEXIT
*\*\return  none
 */
void NVIC_System_LowPower_Enable(uint8_t low_power_mode)
{
    SCB->SCR |= low_power_mode;
}

/**
*\*\name    NVIC_System_Low_Power_Disable
*\*\fun     Selects the condition for the system to enter low power mode.
*\*\param   low_power_mode :
*\*\            - NVIC_LP_SEVONPEND
*\*\            - NVIC_LP_SLEEPDEEP
*\*\            - NVIC_LP_SLEEPONEXIT
*\*\return  none
 */
void NVIC_System_Low_Power_Disable(uint8_t low_power_mode)
{
    SCB->SCR &= (uint32_t)(~(uint32_t)low_power_mode);
}

/**
*\*\name  SysTick_Clock_Source_Set
*\*\fun   Configures the SysTick clock source.
*\*\param systick_clock_source :
*\*\       - SYSTICK_CLKSOURCE_HCLK_DIV8    External clock selected as SysTick clock source.
*\*\       - SYSTICK_CLKSOURCE_HCLK         AHB clock selected as SysTick clock source.
*\*\return  none
 */
void SysTick_Clock_Source_Set(uint32_t systick_clock_source)
{
    if (systick_clock_source == SYSTICK_CLKSOURCE_HCLK)
    {
        SysTick->CTRL |= SYSTICK_CLKSOURCE_HCLK;
    }
    else
    {
        SysTick->CTRL &= SYSTICK_CLKSOURCE_HCLK_DIV8;
    }
}



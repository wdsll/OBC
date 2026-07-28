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
*\*\file n32g430_iwdg.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
**/

#include "n32g430_iwdg.h"


/**
*\*\name    IWDG_Write_Protection_Disable.
*\*\fun     Disable write protection to IWDG_PREDIV and IWDG_RELV registers.
*\*\param   none
*\*\return  none
**/
void IWDG_Write_Protection_Disable(void)
{
    IWDG->KEY = IWDG_WRITE_CONFIG_ENABLE;
}


/**
*\*\name    IWDG_Write_Protection_Enable.
*\*\fun     Enable write protection to IWDG_PREDIV and IWDG_RELV registers.
*\*\param   none
*\*\return  none
**/
void IWDG_Write_Protection_Enable(void)
{
    IWDG->KEY = IWDG_WRITE_CONFIG_DISABLE;
}


/**
*\*\name    IWDG_Prescaler_Division_Set.
*\*\fun     IWDG_Prescaler specifies the IWDG prescaler value.
*\*\param   IWDG_prescaler :
*\*\          - IWDG_CONFIG_PRESCALER_DIV4 
*\*\          - IWDG_CONFIG_PRESCALER_DIV8
*\*\          - IWDG_CONFIG_PRESCALER_DIV16
*\*\          - IWDG_CONFIG_PRESCALER_DIV32
*\*\          - IWDG_CONFIG_PRESCALER_DIV64
*\*\          - IWDG_CONFIG_PRESCALER_DIV128
*\*\          - IWDG_CONFIG_PRESCALER_DIV256
*\*\return  none
**/
void IWDG_Prescaler_Division_Set(IWDG_CONFIG_PRESCALER IWDG_prescaler)
{
    IWDG->PREDIV = IWDG_prescaler;
}


/**
*\*\name    IWDG_Counter_Reload.
*\*\fun     Sets IWDG reload value.
*\*\param   reload_value :
*\*\          -0x000 ~ 0xFFF
*\*\return  none
**/
void IWDG_Counter_Reload(uint16_t reload_value)
{
    IWDG->RELV = reload_value;
}


/**
*\*\name    IWDG_Key_Reload.
*\*\fun     Reload IWDG counter with value defined in IWDG_RELV register.
*\*\return  none
**/
void IWDG_Key_Reload(void)
{
    IWDG->KEY = KEY_RELOAD_COUNTER;
}


/**
*\*\name    IWDG_Enable.
*\*\fun     Start watch dog counter.
*\*\return  none
**/
void IWDG_Enable(void)
{
    IWDG->KEY = KEY_ENABLE_COUNTER;
}

/**
*\*\name    IWDG_Freeze_Enable.
*\*\fun     Freeze and stop IWDG while IWDG is running.
*\*\return  none
**/
void IWDG_Freeze_Enable(void)
{
    IWDG->KEY = IWDG_FREEZE;
}


/**
*\*\name    IWDG_Restore_From_Freeze.
*\*\fun     Restore from run time feeze.
*\*\return  none
**/
void IWDG_Restore_From_Freeze(void)
{
    IWDG->KEY = IWDG_UNFREEZE;
}


/**
*\*\name    IWDG_Status_Get.
*\*\fun     Checks whether the specified IWDG flag is set or not.
*\*\param   IWDG_flag :
*\*\          - IWDG_PVU_FLAG
*\*\          - IWDG_CRVU_FLAG
*\*\return  FlagStatus :
*\*\          - RESET
*\*\          - SET
**/
FlagStatus IWDG_Status_Get(IWDG_STATUS_FLAG IWDG_flag)
{
    if((IWDG->STS & IWDG_flag) != (uint32_t)RESET)
    {
        return SET;
    }
    else
    {
        return RESET;
    }
}

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
*\*\file n32g430_pwr.c
*\*\author Nations
*\*\version v1.0.1
*\*\copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
**/

#include "n32g430_pwr.h"

/** PWR Private Defines **/


/** PWR Driving Functions Declaration **/

/**
*\*\name    PWR_Reset.
*\*\fun     Deinitializes the PWR peripheral registers to their default reset values.
*\*\param   none
*\*\return  none
**/
void PWR_Reset(void)
{
    RCC_APB1_Peripheral_Reset(RCC_APB1_PERIPH_PWR);
}

/**
*\*\name    PWR_RTC_Backup_Access_Enable.
*\*\fun     Enables access to the RTC and backup registers.
*\*\param   none
*\*\return  none
**/
void PWR_RTC_Backup_Access_Enable(void)
{
    *(__IO uint32_t*)PWR_CTRL_DBKP_BITBAND = (uint32_t)ENABLE;
}

/**
*\*\name    PWR_RTC_Backup_Access_Disable.
*\*\fun     Disables access to the RTC and backup registers.
*\*\param   none
*\*\return  none
**/
void PWR_RTC_Backup_Access_Disable(void)
{
    *(__IO uint32_t*)PWR_CTRL_DBKP_BITBAND = (uint32_t)DISABLE;
}

/**
*\*\name    PWR_PVD_Enable.
*\*\fun     Enables the Power Voltage Detector(PVD).
*\*\param   none
*\*\return  none
**/
void PWR_PVD_Enable(void)
{
    *(__IO uint32_t*)PWR_CTRL_PVDEN_BITBAND = (uint32_t)ENABLE;
}

/**
*\*\name    PWR_PVD_Disable.
*\*\fun     Disables the Power Voltage Detector(PVD).
*\*\param   none
*\*\return  none
**/
void PWR_PVD_Disable(void)
{
    *(__IO uint32_t*)PWR_CTRL_PVDEN_BITBAND = (uint32_t)DISABLE;
}

/**
*\*\name    PWR_PVD_Level_Config.
*\*\fun     Configures the voltage threshold detected by the Power Voltage Detector(PVD).
*\*\param   level :
*\*\          - PWR_PVD_LEVEL_2V18
*\*\          - PWR_PVD_LEVEL_2V28
*\*\          - PWR_PVD_LEVEL_2V38
*\*\          - PWR_PVD_LEVEL_2V48
*\*\          - PWR_PVD_LEVEL_2V58
*\*\          - PWR_PVD_LEVEL_2V68
*\*\          - PWR_PVD_LEVEL_2V78
*\*\          - PWR_PVD_LEVEL_2V88
*\*\          - PWR_PVD_LEVEL_1V78
*\*\          - PWR_PVD_LEVEL_1V88
*\*\          - PWR_PVD_LEVEL_1V98
*\*\          - PWR_PVD_LEVEL_2V08
*\*\          - PWR_PVD_LEVEL_3V28
*\*\          - PWR_PVD_LEVEL_3V38
*\*\          - PWR_PVD_LEVEL_3V48
*\*\          - PWR_PVD_LEVEL_3V58
*\*\return  none
**/
void PWR_PVD_Level_Config(uint32_t level)
{
    uint32_t temp_value = 0;
    temp_value = PWR->CTRL;
    /* Clear PLS[7:5] bits and MSB bit */
    temp_value &= PWR_CTRL_PLSMSB_MASK;
    /* Set PLS[7:5] bits according to level value */
    temp_value |= level;
    /* Store the new value */
    PWR->CTRL = temp_value;
}


/**
*\*\name    PWR_Wakeup_Pin_Enable.
*\*\fun     Enables the Wakeup Pin functionality.
*\*\param   pin :
*\*\          - WAKEUP_PIN1 PA8
*\*\          - WAKEUP_PIN2 PA0
*\*\          - WAKEUP_PIN3 PC13
*\*\param   polarity :
*\*\          - PWR_PIN_RISING
*\*\          - PWR_PIN_FALLING
*\*\return  none
**/
void PWR_Wakeup_Pin_Enable(WAKEUP_PINX pin, uint32_t polarity)
{
    switch(pin)
    {
        case WAKEUP_PIN1:
            *(__IO uint32_t*)PWR_CTRLSTS_WKUP1PS_BITBAND = polarity;
            *(__IO uint32_t*)PWR_CTRLSTS_WKUP1EN_BITBAND = (uint32_t)ENABLE;
            break;
        case WAKEUP_PIN2:
            *(__IO uint32_t*)PWR_CTRLSTS_WKUP2PS_BITBAND = polarity;
            *(__IO uint32_t*)PWR_CTRLSTS_WKUP2EN_BITBAND = (uint32_t)ENABLE;
            break;
        case WAKEUP_PIN3:
            *(__IO uint32_t*)PWR_CTRLSTS_WKUP3PS_BITBAND = polarity;
            *(__IO uint32_t*)PWR_CTRLSTS_WKUP3EN_BITBAND = (uint32_t)ENABLE;
            break;
        default:
            break;
    }
}

/**
*\*\name    PWR_Wakeup_Pin_Disable.
*\*\fun     Disables the WakeUp Pin functionality.
*\*\param   pin :
*\*\          - WAKEUP_PIN1 PA8
*\*\          - WAKEUP_PIN2 PA0
*\*\          - WAKEUP_PIN3 PC13
*\*\return  none
**/
void PWR_Wakeup_Pin_Disable(WAKEUP_PINX pin)
{
    switch(pin)
    {
        case WAKEUP_PIN1:
            *(__IO uint32_t*)PWR_CTRLSTS_WKUP1EN_BITBAND = (uint32_t)DISABLE;
            break;
        case WAKEUP_PIN2:
            *(__IO uint32_t*)PWR_CTRLSTS_WKUP2EN_BITBAND = (uint32_t)DISABLE;
            break;
        case WAKEUP_PIN3:
            *(__IO uint32_t*)PWR_CTRLSTS_WKUP3EN_BITBAND = (uint32_t)DISABLE;
            break;
        default:
            break;
    }
}

/**
*\*\name    PWR_RTC_Wakeup_Enable.
*\*\fun     Enables RTC internal wakeup.
*\*\param   none
*\*\return  none
**/
void PWR_RTC_Wakeup_Enable(void)
{
    *(__IO uint32_t*)PWR_CTRLSTS_WKUPRTCEN_BITBAND = (uint32_t)ENABLE;
}

/**
*\*\name    PWR_RTC_Wakeup_Disable.
*\*\fun     Disables RTC internal wakeup.
*\*\param   none
*\*\return  none
**/
void PWR_RTC_Wakeup_Disable(void)
{
    *(__IO uint32_t*)PWR_CTRLSTS_WKUPRTCEN_BITBAND = (uint32_t)DISABLE;
}

/**
*\*\name    PWR_SLEEP_Mode_Enter.
*\*\fun     Enters SLEEP mode.
*\*\param   status :
*\*\          - PWR_SLEEP_ON_EXIT
*\*\          - PWR_SLEEP_NOW
*\*\param   enter_mode :
*\*\          - PWR_SLEEP_ENTRY_WFI enter SLEEP mode with WFI instruction
*\*\          - PWR_SLEEP_ENTRY_WFE enter SLEEP mode with WFE instruction
*\*\return  none
**/
void PWR_SLEEP_Mode_Enter(PWR_SLEEPONEXIT_STATUS status, uint8_t enter_mode)
{
    /* CLEAR SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR &= (uint32_t) ~((uint32_t)SCB_SCR_SLEEPDEEP);
    
    /* Select SLEEPONEXIT mode entry */
    if (status == PWR_SLEEP_ON_EXIT)
    {
        /* the MCU enters Sleep mode as soon as it exits the lowest priority INTSTS */
        SCB->SCR |= SCB_SCR_SLEEPONEXIT;
    }
    else if (status == PWR_SLEEP_NOW)
    {
        /* Sleep-now */
        SCB->SCR &= (uint32_t) ~((uint32_t)SCB_SCR_SLEEPONEXIT);
    }

    /* Select SLEEP mode entry */
    if (enter_mode == PWR_SLEEP_ENTRY_WFI)
    {
        /* Request Wait For Interrupt */
        __WFI();
    }
    else
    {
        /* Request Wait For Event */
        __SEV();
        __WFE();
        __WFE();
    }
}

/**
*\*\name    PWR_STOP0_Mode_Enter.
*\*\fun     Enters STOP0 mode.
*\*\param   status :
*\*\          - PWR_REGULATOR_ON
*\*\          - PWR_REGULATOR_LOWPOWER
*\*\param   enter_mode :
*\*\          - PWR_STOP0_ENTRY_WFI (enter STOP0 mode with WFI instruction)
*\*\          - PWR_STOP0_ENTRY_WFE (enter STOP0 mode with WFE instruction)
*\*\return  none
**/
void PWR_STOP0_Mode_Enter(uint32_t status, uint8_t enter_mode)
{
    uint32_t temp_value = 0;
    /* Select the regulator state in STOP0 mode */
    temp_value = PWR->CTRL;
    /* Clear LPS and PDS bits */
    temp_value &= PWR_CTRL_LPSPDS_MASK;
    /* Set LPS bit according to PWR_Regulator value */
    temp_value |= status;
    /* Store the new value */
    PWR->CTRL = temp_value;
    /* Set SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR |= SCB_SCR_SLEEPDEEP;

    /* Select STOP0 mode entry */
    if (enter_mode == PWR_STOP0_ENTRY_WFI)
    {
        /* Request Wait For Interrupt */
        __WFI();
    }
    else
    {
        /* Request Wait For Event */
        __SEV();
        __WFE();
        __WFE();
    }
    /* Reset SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR &= (uint32_t) ~((uint32_t)SCB_SCR_SLEEPDEEP);
}

/**
*\*\name    PWR_STOP2_Mode_Enter.
*\*\fun     Enters STOP2 mode.
*\*\param   enter_mode :
*\*\          - PWR_STOP2_ENTRY_WFI (enter STOP2 mode with WFI instruction)
*\*\          - PWR_STOP2_ENTRY_WFE (enter STOP2 mode with WFE instruction)
*\*\return  none
**/
void PWR_STOP2_Mode_Enter(uint8_t enter_mode)
{
    uint32_t temp_value = 0;

    /* Select the regulator state in STOP2 mode */
    temp_value = PWR->CTRL;
    /* Clear PDS and LPS bits */
    temp_value &= PWR_CTRL_LPSPDS_MASK;
    /* Store the new value */
    PWR->CTRL = temp_value;
    /*STOP2 sleep mode control-stop2s*/
    PWR->CTRL2 |= PWR_STOP2_ENABLE;
    /* Set SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR |= SCB_SCR_SLEEPDEEP;

    /* Select STOP2 mode entry */
    if (enter_mode == PWR_STOP2_ENTRY_WFI)
    {
        /* Request Wait For Interrupt */
        __WFI();
    }
    else
    {
        /* Request Wait For Event */
        __SEV();
        __WFE();
        __WFE();
    }
    /* Reset SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR &= (uint32_t) ~((uint32_t)SCB_SCR_SLEEPDEEP);
}

/**
*\*\name    PWR_STANDBY_Mode_Enter.
*\*\fun     Enters STANDBY mode.
*\*\param   enter_mode :
*\*\          - PWR_STANDBY_ENTRY_WFI (enter STANDBY mode with WFI instruction)
*\*\          - PWR_STANDBY_ENTRY_WFE (enter STANDBY mode with WFE instruction)
*\*\return  none
**/
void PWR_STANDBY_Mode_Enter(uint8_t enter_mode)
{
    /* Clear Wake-up flag */
    PWR->CTRL |= PWR_CLEAR_WKUPF_ALL;
    /* Clear PDS and LPS bits */
    PWR->CTRL &= PWR_CTRL_LPSPDS_MASK;
    /* Select STANDBY mode */
    PWR->CTRL |= PWR_CTRL_PDS;
    /* Set SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR |= SCB_SCR_SLEEPDEEP;
    /* This option is used to ensure that store operations are completed */
#if defined(__CC_ARM)
    __force_stores();
#endif
    /* Select STANDBY mode entry */
    if (enter_mode == PWR_STANDBY_ENTRY_WFI)
    {
        /* Request Wait For Interrupt */
        __WFI();
    }
    else
    {
        /* Request Wait For Event */
        __SEV();
        __WFE();
        __WFE();
    }
}

/**
*\*\name    PWR_Flag_Status_Get.
*\*\fun     Checks whether the specified PWR flag is set or not.
*\*\param   flag :
*\*\          - PWR_WKUP_FLAG_PA8
*\*\          - PWR_WKUP_FLAG_PA0
*\*\          - PWR_WKUP_FLAG_PC13
*\*\          - PWR_WKUP_FLAG_RTC
*\*\          - PWR_STBY_FLAG
*\*\          - PWR_PVD_OUTPUT_FLAG
*\*\return  SET or RESET.
**/
FlagStatus PWR_Flag_Status_Get(uint32_t flag)
{
    /* Check the status of the PWR flag */
    if ((PWR->CTRLSTS & flag) == (uint32_t)RESET)
    {
        /* PWR Flag is reset */
        return RESET;
    }
    else
    {
        /* PWR Flag is set */
        return SET;
    }
}

/**
*\*\name    PWR_Flag_Status_Clear.
*\*\fun     Clears the PWR's pending flags.
*\*\param   flag :
*\*\          - PWR_WKUP_FLAG_PA8
*\*\          - PWR_WKUP_FLAG_PA0
*\*\          - PWR_WKUP_FLAG_PC13
*\*\          - PWR_WKUP_FLAG_RTC
*\*\          - PWR_STBY_FLAG
*\*\return  none
**/
void PWR_Flag_Status_Clear(uint32_t flag)
{
    if(flag == PWR_STBY_FLAG)
    {
        PWR->CTRL |= flag << PWR_BIT_SHIFT_2;
    }
    else
    {
        PWR->CTRL |= flag << PWR_BIT_SHIFT_7;
    }
}


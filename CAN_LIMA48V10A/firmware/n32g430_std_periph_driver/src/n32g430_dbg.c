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
*\*\file n32g430_dbg.c
*\*\author Nations
*\*\version v1.0.1
*\*\copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
**/

#ifndef __N32G430_DBG_H__
#define __N32G430_DBG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32g430.h"

#define IDCODE_DEVID_MASK ((uint32_t)0x00000FFF)
#define UCID2_BASE        ((uint32_t)0x1FFFF500)
#define UID2_BASE         ((uint32_t)0x1FFFF510)

/**
*\*\name    UCID_Get.
*\*\fun     get unique customer id.
*\*\param   UCIDbuf
*\*\return  none
**/
void UCID_Get(uint8_t *UCIDbuf)
{
    uint8_t num = 0;
    uint32_t* ucid_addr = (void*)0;
    uint32_t temp = 0;

    ucid_addr = (uint32_t*)UCID2_BASE;
    
    for (num = 0; num < UCID_LENGTH;)
    {   /* get ucid from UCID2_BASE */
        temp = *(__IO uint32_t*)(ucid_addr++); 
        UCIDbuf[num++] = (temp & 0xFF);
        UCIDbuf[num++] = (temp & 0xFF00) >> 8;
        UCIDbuf[num++] = (temp & 0xFF0000) >> 16;
        UCIDbuf[num++] = (temp & 0xFF000000) >> 24;
    }
    for (num = 0; num < UCID_LENGTH; num++)
    {   /* check ucid are all 0xFF */
        if (UCIDbuf[num] != 0xFF)
            break;
    }
    if (num == UCID_LENGTH && UCIDbuf[num-1] == 0xFF)
    {   /* get ucid from UCID_BASE */
        ucid_addr = (uint32_t*)UCID_BASE;
        for (num = 0; num < UCID_LENGTH;)
        {
            temp = *(__IO uint32_t*)(ucid_addr++); 
            UCIDbuf[num++] = (temp & 0xFF);
            UCIDbuf[num++] = (temp & 0xFF00) >> 8;
            UCIDbuf[num++] = (temp & 0xFF0000) >> 16;
            UCIDbuf[num++] = (temp & 0xFF000000) >> 24;
        }
    }
}

/**
*\*\name    UID_Get.
*\*\fun     get unique device id.
*\*\param   UIDbuf
*\*\return  none
**/
void UID_Get(uint8_t *UIDbuf)
{
    uint8_t num = 0;
    uint32_t* uid_addr = (void*)0;
    uint32_t temp = 0;
    
    uid_addr = (uint32_t*)UID2_BASE;
        
    for (num = 0; num < UID_LENGTH;)
    {   /* get uid from UID2_BASE */
        temp = *(__IO uint32_t*)(uid_addr++);
        UIDbuf[num++] = (temp & 0xFF);
        UIDbuf[num++] = (temp & 0xFF00) >> 8;
        UIDbuf[num++] = (temp & 0xFF0000) >> 16;
        UIDbuf[num++] = (temp & 0xFF000000) >> 24;
    }
    for (num = 0; num < UID_LENGTH;  num++)
    {   /* check uid are all 0xFF */
        if (UIDbuf[num] != 0xFF)
            break;
    }
    if (num == UID_LENGTH && UIDbuf[num-1] == 0xFF)
    {   /* get uid from UID_BASE */
        uid_addr = (uint32_t*)UID_BASE;
        for (num = 0; num < UID_LENGTH;)
        {
            temp = *(__IO uint32_t*)(uid_addr++);
            UIDbuf[num++] = (temp & 0xFF);
            UIDbuf[num++] = (temp & 0xFF00) >> 8;
            UIDbuf[num++] = (temp & 0xFF0000) >> 16;
            UIDbuf[num++] = (temp & 0xFF000000) >> 24;
        }
    }
}

/**
*\*\name    DBG_Revision_Number_Get.
*\*\fun     get the revision number.
*\*\param   none
*\*\return  revision number
**/
uint32_t DBG_Revision_Number_Get(void)
{
    return (DBG->ID & 0x00FF);
}

/**
*\*\name    DBG_Device_Number_Get.
*\*\fun     get the device identifer.
*\*\param   none
*\*\return  Device identifier
**/
uint32_t DBG_Device_Number_Get(void)
{
    uint32_t id = DBG->ID;
    return ((id & 0x00F00000) >> 20) | ((id & 0xFF00) >> 4);
}

/**
*\*\name    DBG_Peripheral_ON.
*\*\fun     Configures the specified peripheral run when the MCU under Debug mode.
*\*\param   DBG_Periph :
                - DBG_SLEEP             \* Keep debugger connection during SLEEP mode *\
                - DBG_STOP              \* Keep debugger connection during STOP mode *\
                - DBG_STANDBY           \* Keep debugger connection during STANDBY mode *\
                - DBG_IWDG_STOP         \* IWDG stopped when Core is halted. *\
                - DBG_WWDG_STOP         \* WWDG stopped when Core is halted. *\
                - DBG_TIM1_STOP         \* TIM1 counter stopped when Core is halted. *\
                - DBG_TIM2_STOP         \* TIM2 counter stopped when Core is halted. *\
                - DBG_TIM3_STOP         \* TIM3 counter stopped when Core is halted. *\
                - DBG_TIM4_STOP         \* TIM4 counter stopped when Core is halted. *\
                - DBG_CAN_STOP          \* CAN  stopped when Core is halted. *\
                - DBG_I2C1SMBUS_TIMEOUT \* I2C1 SMBUS timeout mode stopped when Core is halted. *\
                - DBG_I2C2SMBUS_TIMEOUT \* I2C2 SMBUS timeout mode stopped when Core is halted. *\
                - DBG_TIM8_STOP         \* TIM8 counter stopped when Core is halted. *\
                - DBG_TIM5_STOP         \* TIM5 counter stopped when Core is halted. *\
                - DBG_TIM6_STOP         \* TIM6 counter stopped when Core is halted. *\
*\*\return  none
**/
void DBG_Peripheral_ON(uint32_t DBG_Periph)
{
    DBG->CTRL |= DBG_Periph;
}

/**
*\*\name    DBG_Peripheral_OFF.
*\*\fun     Configures the specified peripheral run or stop when the MCU under Debug mode.
*\*\param   DBG_Periph :
                - DBG_SLEEP             \* Debugger disconnect during SLEEP mode *\
                - DBG_STOP              \* Debugger disconnect during STOP mode *\
                - DBG_STANDBY           \* Debugger disconnect during STANDBY mode *\
                - DBG_IWDG_STOP         \* IWDG don't stop when Core is halted. *\
                - DBG_WWDG_STOP         \* WWDG don't stop when Core is halted. *\
                - DBG_TIM1_STOP         \* TIM1 counter don't stop when Core is halted. *\
                - DBG_TIM2_STOP         \* TIM2 counter don't stop when Core is halted. *\
                - DBG_TIM3_STOP         \* TIM3 counter don't stop when Core is halted. *\
                - DBG_TIM4_STOP         \* TIM4 counter don't stop when Core is halted. *\
                - DBG_CAN_STOP          \* CAN  stopped when Core is halted. *\
                - DBG_I2C1SMBUS_TIMEOUT \* I2C1 SMBUS timeout mode don't stop when Core is halted. *\
                - DBG_I2C2SMBUS_TIMEOUT \* I2C2 SMBUS timeout mode don't stop when Core is halted. *\
                - DBG_TIM8_STOP         \* TIM8 counter don't stop when Core is halted. *\
                - DBG_TIM5_STOP         \* TIM5 counter don't stop when Core is halted. *\
                - DBG_TIM6_STOP         \* TIM6 counter don't stop when Core is halted. *\
*\*\return  none
**/
void DBG_Peripheral_OFF(uint32_t DBG_Periph)
{
    DBG->CTRL &= ~DBG_Periph;
}

#ifdef __cplusplus
}
#endif

#endif /*__N32G430_DBG_H__ */

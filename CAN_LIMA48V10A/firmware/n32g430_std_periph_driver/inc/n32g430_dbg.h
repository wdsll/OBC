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
*\*\file n32g430_dbg.h
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
**/

#ifndef __N32G430_DBG_H__
#define __N32G430_DBG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32g430.h"

#define DBG_SLEEP               DBG_CTRL_SLEEP
#define DBG_STOP                DBG_CTRL_STOP
#define DBG_STANDBY             DBG_CTRL_STDBY
#define DBG_IWDG_STOP           DBG_CTRL_IWDG_STOP
#define DBG_WWDG_STOP           DBG_CTRL_WWDG_STOP
#define DBG_TIM1_STOP           DBG_CTRL_TIM1_STOP
#define DBG_TIM2_STOP           DBG_CTRL_TIM2_STOP
#define DBG_TIM3_STOP           DBG_CTRL_TIM3_STOP
#define DBG_TIM4_STOP           DBG_CTRL_TIM4_STOP
#define DBG_CAN_STOP            DBG_CTRL_CAN_STOP
#define DBG_I2C1SMBUS_TIMEOUT   DBG_CTRL_I2C1SMBUS_TO
#define DBG_I2C2SMBUS_TIMEOUT   DBG_CTRL_I2C2SMBUS_TO
#define DBG_TIM8_STOP           DBG_CTRL_TIM8_STOP
#define DBG_TIM5_STOP           DBG_CTRL_TIM5_STOP
#define DBG_TIM6_STOP           DBG_CTRL_TIM6_STOP


void UCID_Get(uint8_t *UCIDbuf);
void UID_Get(uint8_t *UIDbuf);
uint32_t DBG_Revision_Number_Get(void);
uint32_t DBG_Device_Number_Get(void);
void DBG_Peripheral_ON(uint32_t DBG_Periph);
void DBG_Peripheral_OFF(uint32_t DBG_Periph);


#ifdef __cplusplus
}
#endif

#endif /* __N32G430_DBG_H__ */

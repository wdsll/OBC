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
*\*\file can_config.h
*\*\author Nations
*\*\version v1.0.1
*\*\copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
**/ 
 
#ifndef __CAN_CONFIG_H__
#define __CAN_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32g430.h"
#include "n32g430_conf.h"

typedef enum
{
    Fail   = (uint8_t)0,
    Pass   = (uint8_t)1
} Slave_Return;


#define  CAN_STD_ID_LIST_32BIT_H(STDID)       ((uint16_t) (((uint32_t)STDID<<21)>>16))
#define  CAN_STD_ID_LIST_32BIT_L(STDID)       ((uint16_t) (0x0000))

#define  CAN_EXT_ID_LIST_32BIT_H(EXTID)       ((uint16_t) ((((uint32_t)EXTID<<3)|0x04)>>16))
#define  CAN_EXT_ID_LIST_32BIT_L(EXTID)       ((uint16_t) (( (uint32_t)EXTID<<3)|0x04))

#define  CAN_FILTER_STDID(STDID)        ((STDID&0x7FF)<<5)

#define  CAN_FILTER_EXTID_H(EXTID)      ((uint16_t)  (((EXTID<<3)|0x04)>>16) )
#define  CAN_FILTER_EXTID_L(EXTID)      ((uint16_t)  (( EXTID<<3)|0x04) )

#define  CAN_STD_ID_H_MASK_CARE         0xFFE0
#define  CAN_STD_ID_L_MASK_CARE         0x0000
#define  CAN_STD_ID_H_MASK_DONT_CARE    0x0000
#define  CAN_STD_ID_L_MASK_DONT_CARE    0x0000

#define  CAN_EXT_ID_H_MASK_CARE         0xFFFF
#define  CAN_EXT_ID_L_MASK_CARE         0xFFF8
#define  CAN_EXT_ID_H_MASK_DONT_CARE    0x0000
#define  CAN_EXT_ID_L_MASK_DONT_CARE    0x0000

void CAN_NVIC_Configuration(void);
void CAN_GPIO_Configuration(void);
void CAN_CONFIG(void);

uint8_t Check_CanRecData(CanRxMessage* RxMessage, uint32_t StdId, uint32_t ExtId, uint8_t IDE, uint8_t RTR, uint8_t DLC,
                         uint8_t Data0, uint8_t Data1, uint8_t Data2, uint8_t Data3,
                         uint8_t Data4, uint8_t Data5, uint8_t Data6, uint8_t Data7, uint8_t FMI);

extern CanTxMessage CAN_TxMessage;
extern CanRxMessage CAN_RxMessage;

extern CAN_InitType        CAN_InitStructure;
extern CAN_FilterInitType  CAN_FilterInitStructure;

#ifdef __cplusplus
}
#endif

#endif /* __CAN_CONFIG_H__ */



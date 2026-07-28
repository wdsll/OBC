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
*\*\file n32g430_crc.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
**/

#include "n32g430_crc.h"

/** CRC Private Defines **/


/** CRC Driving Functions Declaration **/

/**
*\*\name    CRC32_Reset.
*\*\fun     Resets the CRC Data register (DAT).
*\*\param   none
*\*\return  none
**/
void CRC32_Reset(void)
{
    /* Reset CRC generator */
    CRC->CRC32CTRL = CRC_CRC32_RESET;
}

/**
*\*\name    CRC32_Calculate.
*\*\fun     Computes the 32-bit CRC of a given data word(32-bit).
*\*\param   data :
*\*\          - data word(32-bit) to compute its CRC
*\*\return  32-bit CRC
**/
uint32_t CRC32_Calculate(uint32_t data)
{
    /* Write data to the CRC32DAT register */
    CRC->CRC32DAT = data;
    /* Returns the computed CRC result */
    return (CRC->CRC32DAT);
}

/**
*\*\name    CRC32_Buffer_Calculate.
*\*\fun     Computes the 32-bit CRC of a given buffer of data word(32-bit).
*\*\param   p_buf :
*\*\          - pointer to the buffer containing the data to be computed
*\*\param   buf_len :
*\*\          - length of the buffer to be computed
*\*\return  32-bit CRC
**/
uint32_t CRC32_Buffer_Calculate(uint32_t p_buf[], uint32_t buf_len)
{
    uint32_t index = 0;

    for (index = 0; index < buf_len; index++)
    {
        /* Write data to the CRC32DAT register */
        CRC->CRC32DAT = p_buf[index];
    }
    /* Returns the computed CRC result */
    return (CRC->CRC32DAT);
}

/**
*\*\name    CRC32_Get.
*\*\fun     Returns the current CRC value.
*\*\param   none
*\*\return  32-bit CRC
**/
uint32_t CRC32_Get(void)
{
    /* Returns the computed CRC result */
    return (CRC->CRC32DAT);
}

/**
*\*\name    CRC32_Independent_Data_Save.
*\*\fun     Stores a 8-bit data in the Independent Data(ID) register.
*\*\param   value :
*\*\          - 8-bit value to be stored in the ID register
*\*\return  none
**/
void CRC32_Independent_Data_Save(uint8_t value)
{
    /* storage of 8-bit data */
    CRC->CRC32IDAT = value;
}

/**
*\*\name    CRC32_Independent_Data_Get.
*\*\fun     Returns the 8-bit data stored in the Independent Data(ID) register.
*\*\param   none
*\*\return  8-bit value of the ID register
**/
uint8_t CRC32_Independent_Data_Get(void)
{
    /* Returns the 8-bit data */
    return (CRC->CRC32IDAT);
}

/**
*\*\name    CRC16_Calculate.
*\*\fun     Computes the 16-bit CRC of a given data word(8-bit).
*\*\param   data :
*\*\          - data word(8-bit) to compute its CRC
*\*\return  16-bit CRC
**/
uint16_t CRC16_Calculate(uint8_t data)
{
    /* Write data to the CRC16DAT register */
    CRC->CRC16DAT = data;
    /* Returns the computed CRC16 result */
    return (CRC->CRC16D);
}

/**
*\*\name    CRC16_Buffer_Calculate.
*\*\fun     Computes the 16-bit CRC of a given buffer of data word(8-bit).
*\*\param   p_buf :
*\*\          - pointer to the buffer containing the data to be computed
*\*\param   buf_len :
*\*\          - length of the buffer to be computed
*\*\return  16-bit CRC
**/
uint16_t CRC16_Buffer_Calculate(uint8_t p_buf[], uint32_t buf_len)
{
    uint32_t index = 0;
    CRC->CRC16D = 0x00;
    for (index = 0; index < buf_len; index++)
    {
        /* Write data to the CRC16DAT register */
        CRC->CRC16DAT = p_buf[index];
    }
    /* Returns the computed CRC16 result */
    return (CRC->CRC16D);
}

/**
*\*\name    CRC16_Set.
*\*\fun     write a 8-bit data in the 16-bit CRC Data register.
*\*\param   data :
*\*\          - 8-bit data value to be writed in the 16-bit CRC Data register
*\*\return  none
**/
void CRC16_Set(uint8_t data)
{
    /* Writes 8-bit data to the CRC16DAT register */
    CRC->CRC16DAT = data;
}

/**
*\*\name    CRC16_Get.
*\*\fun     Returns the current CRC value.
*\*\param   none
*\*\return  16-bit CRC
**/
uint16_t CRC16_Get(void)
{
    /* Returns the CRC16 calculation result */
    return (CRC->CRC16D);
}

/**
*\*\name    CRC16_Little_Endian_Format_Set.
*\*\fun     Set the calculate data to little endian format.
*\*\param   none
*\*\return  none
**/
void CRC16_Little_Endian_Format_Set(void)
{
    CRC->CRC16CTRL |= CRC_LITTLE_ENDIAN_ENABLE;
}

/**
*\*\name    CRC16_Big_Endian_Format_Set.
*\*\fun     Set the calculate data to big endian format.
*\*\param   none
*\*\return  none
**/
void CRC16_Big_Endian_Format_Set(void)
{
    CRC->CRC16CTRL &= CRC_BIG_ENDIAN_ENABLE;
}

/**
*\*\name    CRC16_Value_Clean_Enable.
*\*\fun     Enable Clean CRC16 value.
*\*\param   none
*\*\return  none
**/
void CRC16_Value_Clean_Enable(void)
{
    CRC->CRC16CTRL |= CRC_CRC16_VALUE_CLEAR;
}

/**
*\*\name    CRC16_Value_Clean_Disable.
*\*\fun     Disable Clean CRC16 value.
*\*\param   none
*\*\return  none
**/
void CRC16_Value_Clean_Disable(void)
{
    CRC->CRC16CTRL &= CRC_CRC16_VALUE_NO_CLEAR;
}

/**
*\*\name    CRC_LRC_Set.
*\*\fun     Write LRC initial value.
*\*\param   data :
*\*\          - 8-bit data
*\*\return  none
**/
void CRC_LRC_Set(uint8_t data)
{
    /* Write LRC to verify the initial value */
    CRC->LRC = data;
}

/**
*\*\name    CRC_LRC_Get.
*\*\fun     Returns the current LRC value.
*\*\param   none
*\*\return  8-bit LRC
**/
uint8_t CRC_LRC_Get(void)
{
    /* Returns the LRC check value */
    return (CRC->LRC);
}


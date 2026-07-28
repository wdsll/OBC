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
*\*\file n32g430_exti.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
**/

#include "n32g430_exti.h"


/**
*\*\name    EXTI_Reset.
*\*\fun     Reset the EXTI registers.
*\*\return  none
**/
void EXTI_Reset(void)
{
    EXTI->IMASK  = 0x00000000;
    EXTI->EMASK  = 0x00000000;
    EXTI->RT_CFG = 0x00000000;
    EXTI->FT_CFG = 0x00000000;
    EXTI->PEND   = 0x00FFFFFF;
}

/**
*\*\name    EXTI_Peripheral_Initializes.
*\*\fun     Initializes the EXTI according to EXTI_InitStruct.
*\*\param   EXTI_InitStruct :
*\*\          - EXTI_Mode
*\*\            - EXTI_Mode_Interrupt
*\*\            - EXTI_Mode_Event
*\*\          - EXTI_Trigger
*\*\            - EXTI_Trigger_Rising
*\*\            - EXTI_Trigger_Falling
*\*\            - EXTI_Trigger_Rising_Falling
*\*\          - EXTI_Line
*\*\            - EXTI_LINE0              
*\*\            - EXTI_LINE1            
*\*\            - EXTI_LINE2              
*\*\            - EXTI_LINE3             
*\*\            - EXTI_LINE4              
*\*\            - EXTI_LINE5 
*\*\            - EXTI_LINE6              
*\*\            - EXTI_LINE7                
*\*\            - EXTI_LINE8              
*\*\            - EXTI_LINE9              
*\*\            - EXTI_LINE10              
*\*\            - EXTI_LINE11            
*\*\            - EXTI_LINE12            
*\*\            - EXTI_LINE13   
*\*\            - EXTI_LINE14
*\*\            - EXTI_LINE15   
*\*\            - EXTI_LINE16   
*\*\            - EXTI_LINE17   
*\*\            - EXTI_LINE18   
*\*\            - EXTI_LINE19     
*\*\            - EXTI_LINE20   
*\*\            - EXTI_LINE21   
*\*\            - EXTI_LINE22 
*\*\            - EXTI_LINE23  
*\*\          - EXTI_LineCmd
*\*\            - ENABLE
*\*\            - DISABLE
*\*\return  none
**/
void EXTI_Peripheral_Initializes(EXTI_InitType* EXTI_InitStruct)
{
    EXTI_Work_Mode_Config(EXTI_InitStruct->EXTI_Line,EXTI_InitStruct->EXTI_Mode);
    EXTI_Trigger_Config(EXTI_InitStruct->EXTI_Line,EXTI_InitStruct->EXTI_Trigger);
    EXTI_LineCmd_Disable(EXTI_InitStruct->EXTI_Line,EXTI_InitStruct->EXTI_LineCmd,EXTI_InitStruct->EXTI_Mode);
}

/**
*\*\name    EXTI_Work_Mode_Config.
*\*\fun     Config the EXTI according to EXTI_Mode.
*\*\param   exti_mode :
*\*\            - EXTI_Mode_Interrupt
*\*\            - EXTI_Mode_Event
*\*\param   exti_line :
*\*\            - EXTI_LINE0              
*\*\            - EXTI_LINE1            
*\*\            - EXTI_LINE2              
*\*\            - EXTI_LINE3             
*\*\            - EXTI_LINE4              
*\*\            - EXTI_LINE5 
*\*\            - EXTI_LINE6              
*\*\            - EXTI_LINE7                
*\*\            - EXTI_LINE8              
*\*\            - EXTI_LINE9              
*\*\            - EXTI_LINE10              
*\*\            - EXTI_LINE11            
*\*\            - EXTI_LINE12            
*\*\            - EXTI_LINE13   
*\*\            - EXTI_LINE14
*\*\            - EXTI_LINE15   
*\*\            - EXTI_LINE16   
*\*\            - EXTI_LINE17   
*\*\            - EXTI_LINE18   
*\*\            - EXTI_LINE19     
*\*\            - EXTI_LINE20   
*\*\            - EXTI_LINE21   
*\*\            - EXTI_LINE22 
*\*\            - EXTI_LINE23 
*\*\return  none
**/
void EXTI_Work_Mode_Config(uint32_t exti_line,uint32_t exti_mode)
{
    uint32_t temp_value = 0;    
    temp_value = (uint32_t)EXTI_BASE;    
    /* Clear work mode configuration */
    EXTI->IMASK &= ~exti_line;
    EXTI->EMASK &= ~exti_line;
   /* Select the work mode */
    temp_value += exti_mode;
    *(__IO uint32_t*)temp_value |= exti_line;
}

/**
*\*\name    EXTI_EXTI_Trigger_Config.
*\*\fun     Config the EXTI according to EXTI_Trigger.
*\*\param   exti_trigger :
*\*\            - EXTI_Trigger_Rising
*\*\            - EXTI_Trigger_Falling
*\*\            - EXTI_Trigger_Rising_Falling
*\*\param   exti_line :
*\*\            - EXTI_LINE0              
*\*\            - EXTI_LINE1            
*\*\            - EXTI_LINE2              
*\*\            - EXTI_LINE3             
*\*\            - EXTI_LINE4              
*\*\            - EXTI_LINE5 
*\*\            - EXTI_LINE6              
*\*\            - EXTI_LINE7                
*\*\            - EXTI_LINE8              
*\*\            - EXTI_LINE9              
*\*\            - EXTI_LINE10              
*\*\            - EXTI_LINE11            
*\*\            - EXTI_LINE12            
*\*\            - EXTI_LINE13   
*\*\            - EXTI_LINE14
*\*\            - EXTI_LINE15   
*\*\            - EXTI_LINE16   
*\*\            - EXTI_LINE17   
*\*\            - EXTI_LINE18   
*\*\            - EXTI_LINE19     
*\*\            - EXTI_LINE20   
*\*\            - EXTI_LINE21  
*\*\            - EXTI_LINE22
*\*\            - EXTI_LINE23
*\*\return  none
**/
void EXTI_Trigger_Config(uint32_t exti_line,uint32_t exti_trigger)
{
    uint32_t temp_value = 0;    
    temp_value = (uint32_t)EXTI_BASE;      
    /* Clear Rising Falling edge configuration */
    EXTI->RT_CFG &= ~exti_line;
    EXTI->FT_CFG &= ~exti_line;

   /* Select the trigger method */
    if(exti_trigger == EXTI_Trigger_Rising_Falling)
    {
        EXTI->RT_CFG |= exti_line;
        EXTI->FT_CFG |= exti_line;
    }
    else
    {
        temp_value = (uint32_t)EXTI_BASE;
        temp_value += exti_trigger;

        *(__IO uint32_t*)temp_value |= exti_line;      
    }    

}

/**
*\*\name    EXTI_EXTI_LineCmd_Disable.
*\*\fun     Config the EXTI according to EXTI_LineCmd.
*\*\param   exti_linecmd :   
*\*\            - ENABLE
*\*\            - DISABLE
*\*\param   exti_mode :
*\*\            - EXTI_Mode_Interrupt
*\*\            - EXTI_Mode_Event
*\*\param   exti_line :
*\*\            - EXTI_LINE0              
*\*\            - EXTI_LINE1            
*\*\            - EXTI_LINE2              
*\*\            - EXTI_LINE3             
*\*\            - EXTI_LINE4              
*\*\            - EXTI_LINE5 
*\*\            - EXTI_LINE6              
*\*\            - EXTI_LINE7                
*\*\            - EXTI_LINE8              
*\*\            - EXTI_LINE9              
*\*\            - EXTI_LINE10              
*\*\            - EXTI_LINE11            
*\*\            - EXTI_LINE12            
*\*\            - EXTI_LINE13   
*\*\            - EXTI_LINE14
*\*\            - EXTI_LINE15   
*\*\            - EXTI_LINE16   
*\*\            - EXTI_LINE17   
*\*\            - EXTI_LINE18   
*\*\            - EXTI_LINE19     
*\*\            - EXTI_LINE20   
*\*\            - EXTI_LINE21 
*\*\            - EXTI_LINE22
*\*\            - EXTI_LINE23 
*\*\return  none
**/
void EXTI_LineCmd_Disable(uint32_t exti_line,uint32_t exti_linecmd,uint32_t exti_mode)
{
    uint32_t temp_value = 0;    
    temp_value = (uint32_t)EXTI_BASE;    
     /* Disable the selected external lines */
    if(exti_linecmd == DISABLE)
    {
        temp_value += exti_mode;
        /* Disable the selected external lines */
        *(__IO uint32_t*)temp_value &= ~exti_line;             
    }
}


/**
*\*\name    EXTI_Structure_Initializes.
*\*\fun     Fills each EXTI_InitStruct member with its default value.
*\*\param   EXTI_InitStruct :   
*\*\            - EXTI_Line
*\*\            - EXTI_Mode
*\*\            - EXTI_Trigger
*\*\            - EXTI_LineCmd  
*\*\return  none
**/
void EXTI_Structure_Initializes(EXTI_InitType* EXTI_InitStruct)
{
    EXTI_InitStruct->EXTI_Line    = EXTI_LINENONE;
    EXTI_InitStruct->EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct->EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct->EXTI_LineCmd = DISABLE;
}


/**
*\*\name    EXTI_Software_Interrupt_Trigger.
*\*\fun     Generates a Software interrupt.
*\*\param   exti_line :   
*\*\            - EXTI_LINE0              
*\*\            - EXTI_LINE1            
*\*\            - EXTI_LINE2              
*\*\            - EXTI_LINE3             
*\*\            - EXTI_LINE4              
*\*\            - EXTI_LINE5 
*\*\            - EXTI_LINE6              
*\*\            - EXTI_LINE7                
*\*\            - EXTI_LINE8              
*\*\            - EXTI_LINE9              
*\*\            - EXTI_LINE10              
*\*\            - EXTI_LINE11            
*\*\            - EXTI_LINE12            
*\*\            - EXTI_LINE13   
*\*\            - EXTI_LINE14
*\*\            - EXTI_LINE15   
*\*\            - EXTI_LINE16   
*\*\            - EXTI_LINE17   
*\*\            - EXTI_LINE18   
*\*\            - EXTI_LINE19     
*\*\            - EXTI_LINE20   
*\*\            - EXTI_LINE21 
*\*\            - EXTI_LINE22
*\*\            - EXTI_LINE23
*\*\return  none
**/
void EXTI_Software_Interrupt_Trigger(uint32_t exti_line)
{
    /* Set the EXTI_SWIE ON bit, The EXTI generates a Software interrupt */
    EXTI->SWIE |= exti_line;
}



/**
*\*\name    EXTI_Flag_Status_Get.
*\*\fun     Get EXTI line flag status.
*\*\param   exti_line :   
*\*\            - EXTI_LINE0              
*\*\            - EXTI_LINE1            
*\*\            - EXTI_LINE2              
*\*\            - EXTI_LINE3             
*\*\            - EXTI_LINE4              
*\*\            - EXTI_LINE5 
*\*\            - EXTI_LINE6              
*\*\            - EXTI_LINE7                
*\*\            - EXTI_LINE8              
*\*\            - EXTI_LINE9              
*\*\            - EXTI_LINE10              
*\*\            - EXTI_LINE11            
*\*\            - EXTI_LINE12            
*\*\            - EXTI_LINE13   
*\*\            - EXTI_LINE14
*\*\            - EXTI_LINE15   
*\*\            - EXTI_LINE16   
*\*\            - EXTI_LINE17   
*\*\            - EXTI_LINE18   
*\*\            - EXTI_LINE19     
*\*\            - EXTI_LINE20   
*\*\            - EXTI_LINE21 
*\*\            - EXTI_LINE22
*\*\            - EXTI_LINE23
*\*\return  SET or RESET
**/
FlagStatus EXTI_Flag_Status_Get(uint32_t exti_line)
{
    if ((EXTI->PEND & exti_line) != (uint32_t)RESET)
    {
        /* EXTI line flag status is set */
        return SET;
    }
    else
    {
        /* EXTI line flag status is reset */
        return RESET;
    }
}

/**
*\*\name    EXTI_Flag_Status_Clear.
*\*\fun     Clear EXTI line flag status.
*\*\param   exti_line :   
*\*\            - EXTI_LINE0              
*\*\            - EXTI_LINE1            
*\*\            - EXTI_LINE2              
*\*\            - EXTI_LINE3             
*\*\            - EXTI_LINE4              
*\*\            - EXTI_LINE5 
*\*\            - EXTI_LINE6              
*\*\            - EXTI_LINE7                
*\*\            - EXTI_LINE8              
*\*\            - EXTI_LINE9              
*\*\            - EXTI_LINE10              
*\*\            - EXTI_LINE11            
*\*\            - EXTI_LINE12            
*\*\            - EXTI_LINE13   
*\*\            - EXTI_LINE14
*\*\            - EXTI_LINE15   
*\*\            - EXTI_LINE16   
*\*\            - EXTI_LINE17   
*\*\            - EXTI_LINE18   
*\*\            - EXTI_LINE19     
*\*\            - EXTI_LINE20   
*\*\            - EXTI_LINE21 
*\*\            - EXTI_LINE22
*\*\            - EXTI_LINE23
*\*\return  none
**/
void EXTI_Flag_Status_Clear(uint32_t exti_line)
{
    /* Clear EXTI line flag status. */
    EXTI->PEND = exti_line;
}




/**
*\*\name    EXTI_Interrupt_Status_Get.
*\*\fun     GET EXTI line interrupt status.
*\*\param   exti_line :   
*\*\            - EXTI_LINE0              
*\*\            - EXTI_LINE1            
*\*\            - EXTI_LINE2              
*\*\            - EXTI_LINE3             
*\*\            - EXTI_LINE4              
*\*\            - EXTI_LINE5 
*\*\            - EXTI_LINE6              
*\*\            - EXTI_LINE7                
*\*\            - EXTI_LINE8              
*\*\            - EXTI_LINE9              
*\*\            - EXTI_LINE10              
*\*\            - EXTI_LINE11            
*\*\            - EXTI_LINE12            
*\*\            - EXTI_LINE13   
*\*\            - EXTI_LINE14
*\*\            - EXTI_LINE15   
*\*\            - EXTI_LINE16   
*\*\            - EXTI_LINE17   
*\*\            - EXTI_LINE18   
*\*\            - EXTI_LINE19     
*\*\            - EXTI_LINE20   
*\*\            - EXTI_LINE21 
*\*\            - EXTI_LINE22
*\*\            - EXTI_LINE23
*\*\return  SET or RESET
**/
INTStatus EXTI_Interrupt_Status_Get(uint32_t exti_line)
{


    if (((EXTI->PEND & exti_line) != (uint32_t)RESET) && ((EXTI->IMASK & exti_line) != (uint32_t)RESET))
    {
        /* EXTI line interrupt status is set */
        return SET;
    }
    else
    {
        /* EXTI line interrupt status is reset */
        return RESET;
    }

}


/**
*\*\name    EXTI_Interrupt_Status_Clear.
*\*\fun     Clear EXTI line interrupt pend bit.
*\*\param   exti_line :   
*\*\            - EXTI_LINE0              
*\*\            - EXTI_LINE1            
*\*\            - EXTI_LINE2              
*\*\            - EXTI_LINE3             
*\*\            - EXTI_LINE4              
*\*\            - EXTI_LINE5 
*\*\            - EXTI_LINE6              
*\*\            - EXTI_LINE7                
*\*\            - EXTI_LINE8              
*\*\            - EXTI_LINE9              
*\*\            - EXTI_LINE10              
*\*\            - EXTI_LINE11            
*\*\            - EXTI_LINE12            
*\*\            - EXTI_LINE13   
*\*\            - EXTI_LINE14
*\*\            - EXTI_LINE15   
*\*\            - EXTI_LINE16   
*\*\            - EXTI_LINE17   
*\*\            - EXTI_LINE18   
*\*\            - EXTI_LINE19     
*\*\            - EXTI_LINE20   
*\*\            - EXTI_LINE21 
*\*\            - EXTI_LINE22
*\*\            - EXTI_LINE23
*\*\return  none
**/
void EXTI_Interrupt_Status_Clear(uint32_t exti_line)
{

    /* Clear EXTI line interrupt pend bit. */
    EXTI->PEND = exti_line;
}

/**
*\*\name    EXTI_RTC_Time_Stamp_Select.
*\*\fun     Select the input of TimeStamp event.
*\*\param   exti_tssel_line :   
*\*\            - EXTI_TSSEL_Line0             
*\*\            - EXTI_TSSEL_Line1            
*\*\            - EXTI_TSSEL_Line2             
*\*\            - EXTI_TSSEL_Line3             
*\*\            - EXTI_TSSEL_Line4               
*\*\            - EXTI_TSSEL_Line5 
*\*\            - EXTI_TSSEL_Line6              
*\*\            - EXTI_TSSEL_Line7                
*\*\            - EXTI_TSSEL_Line8              
*\*\            - EXTI_TSSEL_Line9              
*\*\            - EXTI_TSSEL_Line10              
*\*\            - EXTI_TSSEL_Line11            
*\*\            - EXTI_TSSEL_Line12            
*\*\            - EXTI_TSSEL_Line13   
*\*\            - EXTI_TSSEL_Line14
*\*\            - EXTI_TSSEL_Line15   
*\*\return  none
**/
void EXTI_RTC_Time_Stamp_Select(uint32_t exti_tssel_line)
{
    /* Select the input of TimeStamp event */
    EXTI->TS_SEL &= EXTI_TSSEL_LINE_MASK;
    EXTI->TS_SEL |= exti_tssel_line;
}



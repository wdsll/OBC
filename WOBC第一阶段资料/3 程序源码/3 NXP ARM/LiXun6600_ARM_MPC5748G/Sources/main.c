/*******************************************************************
** @file main.c
** @version 01.00
** @brief
**         Main module.
**         This module contains user's application code.
*/
/*!
**  @addtogroup main_module main module documentation
**  @{
*/
/* MODULE main */

/* Including needed modules to compile this module/procedure */

/* User includes (#include below this line is not maintained by Processor Expert) */
#include <main.h>

/* Error message displayed at the console, in case data is received erroneously */

uint8_t exit_code=0;
status_t status;
uint32_t bytesRemaining;

status_t ret;        /* Store the driver APIs return code */
SYS_INFO_Data SysInfoData;
/******************************************************************\
 * Read DFlash Data
 * Para:
 * addr:address
 * size:Read acount size
 * pDataArray:the result data of read
 ******************************************************************/
void FLASH_DRV_Read(uint32_t addr,uint8_t * pDataArray,uint16_t size)
{
	uint16_t i=0;
	uint32_t addr_temp=0;

	 addr_temp = addr;
	 for (i = 0U; i < size; i++)
		{
		 addr_temp = addr + i;
		 pDataArray[i] = *(uint8_t *)addr_temp;
		}
}

void System_Init(void)
{
  /* Initialize and configure clocks
   *     -    see clock manager component for details
   */
  CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
				 g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
  CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

  /* Initialize pins
   *    -    See PinSettings component for more info
   */
  PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr);
}


/*!
 \brief The main function for the project.
 \details The startup initialization sequence is the following:
 * - startup asm routine
 * - main()
 */
int main(void)
{
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
#ifdef PEX_RTOS_INIT
  PEX_RTOS_INIT(); /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
#endif
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  /* For example: for(;;) { } */
  System_Init();   //
//  Wdog_Init();
  Ftm_Iint();      //
  FtmCapture_Iint();
  Mscan_Iint();    //
  MscanHV_Iint();
  Gpio_Init();     //
  Adc_Iint();      //
  vJ1772Init();
//  ret = FLASH_DRV_Init(&Flash_InitConfig0, &flashSSDConfig);//Flash模锟斤拷锟绞硷拷锟�???
//  DEV_ASSERT(STATUS_SUCCESS == ret);

  while (1)
  {
	MscanReceiveHandle(); //
	MscanHV_ReceiveHandle();
    if(taskPeriod)//10ms
    {
      taskPeriod = FALSE;
      ADC_ConvertHandle();
      vJ1772Handle();
      MsacnSendMsgHandle();
      MsacnHV_SendMsgHandle();
      Ftm_TaskHandle();
    }
  }

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;) {
    if(exit_code != 0) {
      break;
    }
  }
  return exit_code;
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
 ** @}
 */
/*
 ** ###################################################################
 **
 **     This file was created by Processor Expert 10.1 [05.21]
 **     for the Freescale S32K series of microcontrollers.
 **
 ** ###################################################################
 */


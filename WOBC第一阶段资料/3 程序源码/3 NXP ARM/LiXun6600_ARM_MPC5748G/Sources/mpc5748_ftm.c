/*
 * MPC5748_ftm.c
 *
 *  Created on: 2019閿熸枻鎷�4閿熸枻鎷�2閿熸枻鎷�
 *      Author: haishengliu
 */

#include <main.h>


/* Flex Timer Instance interrupt service routine
 *  - this will toggle LED 1 and clear the timer overflow flag of the FTM
 *  instance
 */
uint8_t delay_10msCnt = 0;
uint8_t taskPeriod = FALSE;
uint8_t delay_sciCnt = 0;
uint8_t sicTxPeriod = FALSE;
uint32_t FTM0_Freq;
uint16_t inputCP_Freq;
MAIN_STATUS_DATA MainStatusData;

void PIT_Ch0_IRQHandler(void)//period 10ms
{
    /* Static variable for storing the timer overflow count */
	taskPeriod = TRUE;
	delay_10msCnt++;
	if(delay_10msCnt == 20)
	{
		SET_PIN_LED0_TOGLE();
	}
	else if(delay_10msCnt == 40)
	{
		SET_PIN_LED0_TOGLE();
		SET_PIN_LED1_TOGLE();
	}
	else if(delay_10msCnt == 60)
	{
		SET_PIN_LED1_TOGLE();
		SET_PIN_LED2_TOGLE();
	}
	else if(delay_10msCnt == 80)
	{
		SET_PIN_LED2_TOGLE();
		SET_PIN_LED3_TOGLE();
	}
	else if(delay_10msCnt == 100)
	{
		delay_10msCnt = 0;
		SET_PIN_LED3_TOGLE();
		SET_PIN_LED1_TOGLE();
	}
//	AppUds_AddTimer();
	PIT_DRV_ClearStatusFlags(INST_PIT1, pit1_ChnConfig0.hwChannel);     /* Clear channel 0 interrupt flag */
}

void Ftm_Iint(void)
{
	/* Initialize PIT */
	PIT_DRV_Init(INST_PIT1, &pit1_InitConfig);
	/* Initialize channel 0 */
	PIT_DRV_InitChannel(INST_PIT1, &pit1_ChnConfig0);
	/* Start channel 0 counting */
	PIT_DRV_StartChannel(INST_PIT1, pit1_ChnConfig0.hwChannel);
}


void FtmCapture_Iint(void)
{
    /* initialize the input capture mode */
    IC_Init(&ic_pal1_instance, &ic_pal1_InitConfig);
    IC_SetChannelMode(&ic_pal1_instance, 4, IC_MEASURE_RISING_EDGE_PERIOD);
    CLOCK_SYS_GetFreq(eMIOS0_CLK, &FTM0_Freq);
    FTM0_Freq = FTM0_Freq / 8;
}

void FtmCapture_GetValue(void)//10ms
{
	static uint32_t inputCaptureMeas_F = 0;
	static uint32_t inputCaptureMeas_D = 0;
	static uint8_t  idexFtm = 0;
	
	idexFtm++;
    /* Get values */
	if(idexFtm == 10)
	{
		inputCaptureMeas_F = IC_GetMeasurement(&ic_pal1_instance, 4);//CH6
		IC_SetChannelMode(&ic_pal1_instance, 4, IC_MEASURE_PULSE_HIGH);
		inputCP_Freq = (inputCaptureMeas_F > 0)?(FTM0_Freq / inputCaptureMeas_F):(0);//1HZ
	}
	else if(idexFtm == 20)
	{
		idexFtm = 0;
		inputCaptureMeas_D = IC_GetMeasurement(&ic_pal1_instance, 4);//CH6
		IC_SetChannelMode(&ic_pal1_instance, 4, IC_MEASURE_RISING_EDGE_PERIOD);
		xCp.usDutyRaw = (inputCaptureMeas_F > 0)?(inputCaptureMeas_D * 1000 / inputCaptureMeas_F):(0);//0.1%
		xCp.usDutyRaw = (xCp.usDutyRaw < 1000)?(1000 - xCp.usDutyRaw):(0);
		xCp.usFreqRaw = inputCP_Freq;
		if((xCp.usDutyRaw == 0)||(xCp.usDutyRaw == 1000))//0% or 100%
			xCp.usFreqRaw = 0;
	}
}

void Ftm_TaskHandle(void)//10ms
{
	uint16_t wDidDataTemp = 0;//
	uint16_t usInI = 0;//0.1A/bit
	static uint16_t delaySleep = 0;
	static uint8_t  delayCPFreq = 0;
	static uint8_t  delayCPDuty = 0;
	static uint8_t  delayCPVolt = 0;
	static uint8_t  delayInit = 0;
/******************** fault check ********************************/
	if(MainStatusData.BMS_OBC_CANTcnt > 0)
	{
		MainStatusData.BMS_OBC_CANTcnt--;
		SysInfoData.SYS_OBC_FaultCode.bits.OBC_CAN_MissBMS = false;
	}
	else
		SysInfoData.SYS_OBC_FaultCode.bits.OBC_CAN_MissBMS = true;

	if(MainStatusData.VCU_DCDC_CANTcnt > 0)
	{
		MainStatusData.VCU_DCDC_CANTcnt--;
		SysInfoData.SYS_OBC_FaultCode.bits.DC_CAN_VCUloss = false;
	}
	else
		SysInfoData.SYS_OBC_FaultCode.bits.DC_CAN_VCUloss = true;

	if(MainStatusData.DSP_BOOT_CANTcnt > 0)
		MainStatusData.DSP_BOOT_CANTcnt--;
	else
		MSCAN_TO_SCI_CHN = 0;

	if(MainStatusData.DSP_OBC_SCITcnt > 0)
	{
		MainStatusData.DSP_OBC_SCITcnt--;
		SysInfoData.SYS_OBC_FaultCode.bits.OBC_SCIloss = false;
	}
	else
		SysInfoData.SYS_OBC_FaultCode.bits.OBC_SCIloss = true;

	if(MainStatusData.DSP_DCDC_SCITcnt > 0)
	{
		MainStatusData.DSP_DCDC_SCITcnt--;
		SysInfoData.SYS_OBC_FaultCode.bits.DC_SCIloss = false;
	}
	else
		SysInfoData.SYS_OBC_FaultCode.bits.DC_SCIloss = true;
/******************** OBC CPFrequencyAbnormal **************************/
	if((xCp.usFreqRaw > 1030)||(xCp.usFreqRaw < 970)) //1030Hz  970Hz
	{
		if(++delayCPFreq > 20) //200ms
		{
			delayCPFreq = 20;
			SysInfoData.SYS_OBC_FaultCode.bits.OBC_CPFrequencyAbnormal = true;
		}
	}
	else
	{
		delayCPFreq = 0;
		SysInfoData.SYS_OBC_FaultCode.bits.OBC_CPFrequencyAbnormal = false;
	}
/******************** OBC OBC_CPVoltAbnormal **************************/
	if(xCp.usValueRaw > 12000) //12V
	{
		if(++delayCPVolt > 20) //200ms
		{
			delayCPVolt = 20;
			SysInfoData.SYS_OBC_FaultCode.bits.OBC_CPVoltAbnormal = true;
		}
	}
	else
	{
		delayCPVolt = 0;
		SysInfoData.SYS_OBC_FaultCode.bits.OBC_CPVoltAbnormal = false;
	}
/******************** OBC OBC_CPDutyAbnormal **************************/
	if((xCp.usDutyRaw > 970)||(xCp.usDutyRaw < 80)) //97%  8%
	{
		if(++delayCPDuty > 20) //200ms
		{
			delayCPDuty = 20;
			SysInfoData.SYS_OBC_FaultCode.bits.OBC_CPDutyAbnormal = true;
		}
	}
	else
	{
		delayCPDuty = 0;
		SysInfoData.SYS_OBC_FaultCode.bits.OBC_CPDutyAbnormal = false;
	}
	/******************** TJA1145 INIT **************************/
    if(State_TJA1145_MODE == TJA1145_INIT)//
	{
		if(++delayInit > 5) //50ms
		{
			delayInit = 0;
			TJA1145_Init();
		}
	}
	/******************** TJA1145 SLEEP **************************/
    if((GET_PIN_VCU_OBC_EN())&&(MSCAN_TO_SCI_CHN == 0))
	{
		if(++delaySleep > 1000) //10S
		{
			delaySleep = 0;
			//TJA1145_SetModel(TJA1145_SLEEP);
		}
	}
	else
		delaySleep = 0;

}
















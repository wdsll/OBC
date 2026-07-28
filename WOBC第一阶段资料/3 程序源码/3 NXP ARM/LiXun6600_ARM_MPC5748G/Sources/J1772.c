/*
 * J1772.c
 *
 *  Created on: May 13, 2021
 *      Author: Administrator1
 */

#include "main.h"

//CC閿熸枻鎷峰洿
#define j1772CC_S3_CLOSE_63A (100) //
#define j1772CC_S3_CLOSE_32A (220) //
#define j1772CC_S3_CLOSE_16A (680) //
#define j1772CC_S3_CLOSE_10A (1500) //
#define j1772CC_S3_OPEN_MIN  (3300) //
#define j1772CC_S3_OPEN_MAX  (3520) //
//CC
#define j1772CC_S3_CLOSE (1) //
#define j1772CC_S3_OPEN (2)	 //
#define j1772CC_UNPLG (0)	 //
#define j1772CC_ERR (3)	 //
//cp
#define j1772CP_DIG -1 //閿熸枻鎷烽敓鏂ゆ嫹閫氳妯″紡
#define j1772CP_ERR -2 //閿熸枻鎷烽敓鏂ゆ嫹鐘舵�侀敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿燂拷
//CP閿熸枻鎷峰洿
#define j1772CP_0V_L	(0)//0V
#define j1772CP_0V_H	(1000)//1V
#define j1772CP_9V_L	(8000)
#define j1772CP_9V_H	(10000)
#define j1772CP_6V_L	(5000)
#define j1772CP_6V_H	(7000)
#define j1772CP_3V_L	(2000)
#define j1772CP_3V_H	(4000)

j1772_cc_data_t xCc;
j1772_cp_data_t xCp;
j1772_data_t xJ1772Dt;

static void vGetCcValue(void)
{
	// Check if the value has significantly changed since last sample
	xCc.usIsChange = TRUE;
	xCc.usValueRaw = (uint32_t)AdcData.analog.Rcc_mcu*4700/(AdcData.analog.Rcc_mcu + 1000);//0.001V
}
//tolerance limits
static int16_t sIsToleranceRange(int32_t lIdel, int32_t lLimtL, int32_t lLimtH)
{
	if ((lIdel >= lLimtL) && (lIdel <= lLimtH))
	{
		return 1;
	}
	else
		return 0;
}
static int32_t lGetCcStatus(void)
{
	int32_t lCcStatus = j1772CC_UNPLG;

	if(AdcData.analog.Rcc_mcu < j1772CC_S3_CLOSE_10A)
	{
		lCcStatus = j1772CC_S3_CLOSE;
		xCc.usAcLimtPer = 100;//10A
		if(AdcData.analog.Rcc_mcu < j1772CC_S3_CLOSE_16A)
			xCc.usAcLimtPer = 160;//16A
		if(AdcData.analog.Rcc_mcu < j1772CC_S3_CLOSE_32A)
			xCc.usAcLimtPer = 320;//32A
		if(AdcData.analog.Rcc_mcu < j1772CC_S3_CLOSE_63A)
			xCc.usAcLimtPer = 630;//63A
	}
	else if (sIsToleranceRange(AdcData.analog.Rcc_mcu, j1772CC_S3_OPEN_MIN, j1772CC_S3_OPEN_MAX))
	 {
	 	lCcStatus = j1772CC_S3_OPEN; //CC閿熻緝鍖℃嫹
	 }
	else if(AdcData.analog.Rcc_mcu > j1772CC_S3_OPEN_MAX)
	 {
	 	lCcStatus = j1772CC_UNPLG; //CC閿熻緝鍖℃嫹
	 }
	return lCcStatus;
}

//閿熸枻鎷峰彇chg 閿熸枻鎷烽敓鏂ゆ嫹
static int32_t lGetCpChgRate(void)
{
	int32_t lChgRateAmps = 0;
	if (xCp.usDutyRaw < 30)
	{
		lChgRateAmps = j1772CP_ERR;
	}
	else if (xCp.usDutyRaw < 70)
	{
		lChgRateAmps = j1772CP_DIG;
	}
	else if (xCp.usDutyRaw < 80)
	{
		lChgRateAmps = j1772CP_ERR;
	}
	else if (xCp.usDutyRaw < 100)
	{
		lChgRateAmps = 60; //6A
	}
	else if (xCp.usDutyRaw < 850)
	{
		// Available current = (% duty cycle) x 0.6 A
		lChgRateAmps = xCp.usDutyRaw * 6/10;
	}
	else if (xCp.usDutyRaw < 960)
	{
		// Available current = (% duty cycle - 64) x 2.5 A
		lChgRateAmps = (xCp.usDutyRaw - 640) * 25/10; //
	}
	else if (xCp.usDutyRaw < 970)
	{
		lChgRateAmps = 800; //
	}
	else
	{
		lChgRateAmps = j1772CP_ERR;
	}

	return (lChgRateAmps);
}

void vJ1772Init(void)
{
	xCc.usIsChange = 0;
	xCc.usValuePre = 0;
	xCc.usValueRaw = 0;
	xCp.usDutyRaw = 0;
	xCp.usIsChange = 0;
	xJ1772Dt.sCable = j1772CC_UNPLG;
	xJ1772Dt.sChgAmp = 0;
	xJ1772Dt.sState = jSTATUS_A;
	xJ1772Dt.xCtl.usAll = 0x00;
}

void vJ1772Handle(void) //10ms?
{
    FtmCapture_GetValue();
	vGetCcValue();
	xJ1772Dt.sCable = lGetCcStatus();
	xJ1772Dt.sChgAmp = lGetCpChgRate();

	xCp.usValueRaw = (uint32_t)AdcData.analog.Vcp_mcu * 5.4433;//0.001V--Vcc=4.7V
	
	if (sIsToleranceRange(xCp.usValueRaw, j1772CP_0V_L, j1772CP_0V_H))
	{
		xJ1772Dt.sState = jSTATUS_A;
	}
	else if (sIsToleranceRange(xCp.usValueRaw, j1772CP_9V_L, j1772CP_9V_H))
	{
		if(xJ1772Dt.sCable == j1772CC_S3_CLOSE)
			xJ1772Dt.sState = jSTATUS_B2;
		else
			xJ1772Dt.sState = jSTATUS_B1;

	}
	else if (sIsToleranceRange(xCp.usValueRaw, j1772CP_6V_L, j1772CP_6V_H))
	{
		//xJ1772Dt.sState = jSTATUS_C;//charing
	}
	else if (sIsToleranceRange(xCp.usValueRaw, j1772CP_3V_L, j1772CP_3V_H))
	{
		xJ1772Dt.sState = jSTATUS_D;
	}
	
	xJ1772Dt.sInMaxI = (xCc.usAcLimtPer < xJ1772Dt.sChgAmp)?(xCc.usAcLimtPer):(xJ1772Dt.sChgAmp);
	xJ1772Dt.sIoMaxI = (uint32_t)OBC_InfoData_HV.OBC_RX_PFC_GETVAC * xJ1772Dt.sInMaxI / OBC_InfoData_HV.OBC_RX_GETVOLT;
	if((xJ1772Dt.sState == jSTATUS_B2)&&(Bms_ReceiveReq.BMS_OBC_StsModeReq == 0x0A))// EN EVSE_S2 or EN Charge
	//if(Bms_ReceiveReq.BMS_OBC_StsModeReq == 0x0A)// EN EVSE_S2 or EN Charge--TEST
	{
		xJ1772Dt.xCtl.bits.S2E = 1; //S2 close,enable AC input
		SET_PIN_S2_CTRL_H();
	}
//	else
//	{
//		xJ1772Dt.xCtl.bits.S2E = 0; //S2 open
//		SET_PIN_S2_CTRL_L();
//	}
}







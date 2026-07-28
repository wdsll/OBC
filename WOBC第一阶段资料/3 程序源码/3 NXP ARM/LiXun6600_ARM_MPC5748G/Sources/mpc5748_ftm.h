/*
 * MPC5748_ftm.h
 *
 *  Created on: 2019��4��2��
 *      Author: haishengliu
 */

#ifndef MPC5748_FTM_H_
#define MPC5748_FTM_H_
#include <main.h>

#define runAC_IN_I_MAX		(150)//0.1A/bit

typedef struct
{
	uint16_t  BMS_OBC_CANTcnt;//
	uint16_t  VCU_DCDC_CANTcnt;//
	uint16_t  DSP_BOOT_CANTcnt;//

	uint8_t   DSP_OBC_SCITcnt;//
	uint8_t   DSP_DCDC_SCITcnt;//
	uint8_t   DSP_OBC_SCISend;//
	uint8_t   DSP_DCDC_SCISend;//
} MAIN_STATUS_DATA;

extern MAIN_STATUS_DATA MainStatusData;
extern uint8_t taskPeriod;
extern uint8_t sicTxPeriod;

void Ftm_Iint(void);
void FtmCapture_Iint(void);
void FtmCapture_GetValue(void);
void Ftm_TaskHandle(void);

#endif /* MPC5748_FTM_H_ */

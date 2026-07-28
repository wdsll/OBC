/*
 * main.h
 *
 *  Created on: 2019��4��3��
 *      Author: haishengliu
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "Cpu.h"
#include "pin_mux.h"
#include "dmaController1.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <MPC5748_mscan.h>
#include <MPC5748_mscan_HV.h>
#include <MPC5748_ftm.h>
#include <MPC5748_adc.h>
#include <MPC5748_gpio.h>
#include <MPC5748_tja1145.h>
#include <J1772.h>

#define FALSE  0
#define TRUE   1

#define FLASH_CALCULATION_ADDR     (uint32_t)0x00016C00
#define FLASH_BOOTLOADER_SINGERPRINT (uint32_t)0x00017C00
#define DTC_CODEandSTATUS_ADDR		 (uint32_t)0x00018000

typedef struct
{
    union{
    	uint16_t word;
    	struct{
    		uint16_t OBC_CPFrequencyAbnormal :1;//
    		uint16_t OBC_CPVoltAbnormal :1;//
    		uint16_t OBC_CPDutyAbnormal :1;//
    		uint16_t OBC_S2ControlFault :1;//
    		uint16_t OBC_CAN_LostBMSFrame :1;//
    		uint16_t OBC_CAN_MissBMS :1;//
    		uint16_t OBC_CAN_MissVCU :1;//
    		uint16_t OBC_CAN_BusOff :1;//

    		uint16_t OBC_ElockFault :1;//
    		uint16_t OBC_SCIloss :1;//
    		uint16_t DC_CAN_VCUloss :1;//
    		uint16_t DC_CAN_BUSOFF :1;//
    		uint16_t DC_WakeUpInMiss :1;//
    		uint16_t DC_12VFault :1;//
    		uint16_t DC_SCIloss :1;//
    		uint16_t Reserve :1;//
    	}bits;
    }SYS_OBC_FaultCode;

}SYS_INFO_Data;

extern SYS_INFO_Data SysInfoData;
extern status_t status;
extern uint32_t bytesRemaining;
extern status_t ret;

void FLASH_DRV_Read(uint32_t addr,uint8_t * pDataArray,uint16_t size);

#endif /* MAIN_H_ */











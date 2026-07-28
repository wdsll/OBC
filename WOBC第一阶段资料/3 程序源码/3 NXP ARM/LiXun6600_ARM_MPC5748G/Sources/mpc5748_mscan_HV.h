/*
 * MPC5748_mscan.h
 *
 *  Created on: 2019��4��2��
 *      Author: haishengliu
 */

#ifndef MPC5748_MSCAN_HV_H_
#define MPC5748_MSCAN_HV_H_
#include <main.h>

#define TX_MAILBOX_HV              0U
#define RX_MAILBOX_OBC1_HV         1UL
#define RX_MAILBOX_OBC2_HV         2UL
#define RX_MAILBOX_ATE_HV          3UL

#define RX_MSG_ID_OBC1_HV      0x301    //
#define RX_MSG_ID_OBC2_HV      0x302
#define RX_MSG_ID_ATE_HV       0x558

typedef struct
{
	uint8_t   OBC_TX_LLCONOFF;
	uint16_t  OBC_TX_LLCSETVOLT;
	uint16_t  OBC_TX_LLCSETCURR;

	uint16_t OBC_RX_PFC_PROTECT;
	uint16_t OBC_RX_PFC_GETVAC;
	uint16_t OBC_RX_PFC_GETIAC;
	uint16_t OBC_RX_ACFreq;
	uint16_t OBC_RX_PROTECT;
	uint16_t OBC_RX_GETVOLT;
	uint16_t OBC_RX_GETCURR;
	uint8_t  OBC_RX_PFCT;
	uint8_t  OBC_RX_TemperatureAVG;
}OBC_INFO_Data_HV;

extern OBC_INFO_Data_HV  OBC_InfoData_HV;

void MscanHV_Iint(void);
void MscanHV_ReceiveHandle(void);
void MsacnHV_SendMsgHandle(void);
#endif /* MPC5748_MSCAN_H_ */

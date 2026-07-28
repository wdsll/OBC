/*
 * MPC5748_mscan.h
 *
 *  Created on: 2019��4��2��
 *      Author: haishengliu
 */

#ifndef MPC5748_MSCAN_H_
#define MPC5748_MSCAN_H_
#include <main.h>

#define TX_MAILBOX              5U
#define RX_MAILBOX_OBC          6UL
#define RX_UDS_PHYS_MAILBOX     7UL
#define RX_UDS_FUNC_MAILBOX     8UL
#define RX_MAILBOX_ATE          9UL
#define RX_MAILBOX_BOOT         10UL

#define RX_MSG_ID_OBC       0x200
#define RX_MSG_ID_ATE       0x557
#define RX_MSG_ID_BOOT      0x001

typedef struct
{
	uint8_t  BMS_OBC_StsModeReq;
	uint16_t BMS_OBC_MaxVoltReq;
	uint16_t BMS_OBC_MaxCurReq;

	uint8_t  VCU_DCDC_StsModeReq;
	uint8_t  VCU_DCDC_VoltReq;
	uint16_t VCU_DCDC_CurrentLim;
	uint16_t VCU_DCDC_PwrPermit;
}BMS_RECEIVE_REQ;

typedef struct
{
	uint8_t  OBC_BMS_ChgFaultReq;
	uint8_t  OBC_StsMode;
	uint8_t  OBC_StsChgCC;
	uint8_t  OBC_StsCP;
	uint8_t  OBC_BMS_PWMCP;
	uint8_t  OBC_StsElock;

	uint16_t OBC_BMS_MaxCurDC;
	uint16_t OBC_BMS_MaxCurAC;
	uint16_t OBC_BMS_ActVoltDC;
	uint16_t OBC_BMS_ActCurDC;
	uint16_t OBC_BMS_ActVoltAC;
	uint16_t OBC_BMS_ActCurAC;

	uint8_t  OBC_OBCTemp;
	uint8_t  OBC_InletTemp;
	uint8_t  OBC_ACInletLTerminalTemp;
	uint8_t  OBC_ACInletNTerminalTemp;
	uint8_t  OBC_TempFault;

	uint8_t  OBC_FaultCode;
}OBC_INFO_Data;

extern OBC_INFO_Data  OBC_InfoData;
extern BMS_RECEIVE_REQ Bms_ReceiveReq;
extern can_message_t recvMsg;
extern can_message_t recvMsgUdsBuff;
extern uint16_t Can_Rx_TimeOutCnt;
extern uint8_t MSCAN_TO_SCI_CHN;

void Mscan_Iint(void);
void MscanReceiveHandle(void);
void MsacnSendMsgHandle(void);
#endif /* MPC5748_MSCAN_H_ */

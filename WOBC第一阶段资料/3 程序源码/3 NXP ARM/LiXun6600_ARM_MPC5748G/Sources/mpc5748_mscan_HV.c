/*
 * MPC5748_mscan.c
 *
 *  Created on: 2019閿熸枻鎷�4閿熸枻鎷�2閿熸枻鎷�
 *      Author: haishengliu
 */
/* Including needed modules to compile this module/procedure */

#include <main.h>

OBC_INFO_Data_HV  OBC_InfoData_HV;
can_message_t recvMsgOBC;
static bool mscan_rxcallback_complete;
static can_message_t messageTX;
/******************** CAN ************************************/
void MscanHVrxCallback(void *driverState, can_event_t event, void *userData)
{
    /* Unused parameters */
    (void)driverState;
    (void)userData;

	if (event == CAN_EVENT_RX_COMPLETE) //
	{
		if((recvMsgOBC.id == RX_MSG_ID_OBC1_HV)||(recvMsgOBC.id == RX_MSG_ID_OBC2_HV)
				||(recvMsgOBC.id == RX_MSG_ID_ATE_HV))  //
		{
			mscan_rxcallback_complete = true;
		}
		/* Start receiving data in RX_MAILBOX. */
		CAN_Receive(&can_pal2_instance, RX_MAILBOX_OBC1_HV, &recvMsgOBC);
		status = CAN_GetTransferStatus(&can_pal2_instance, RX_MAILBOX_OBC1_HV);
		CAN_Receive(&can_pal2_instance, RX_MAILBOX_OBC2_HV, &recvMsgOBC);
		status = CAN_GetTransferStatus(&can_pal2_instance, RX_MAILBOX_OBC2_HV);
		CAN_Receive(&can_pal2_instance, RX_MAILBOX_ATE_HV, &recvMsgOBC);
		status = CAN_GetTransferStatus(&can_pal2_instance, RX_MAILBOX_ATE_HV);
	}
}


void MscanHV_Iint(void)
{
	can_buff_config_t buffCfg;
	can_buff_config_t buffCfgExt;
	can_buff_config_t txbuffCfg;

	CAN_Init(&can_pal2_instance, &can_pal2_Config0);

	buffCfg.enableFD = false;
	buffCfg.enableBRS = false;
	buffCfg.fdPadding = 0U;
	buffCfg.idType = CAN_MSG_ID_STD;
	buffCfg.isRemote = false;
	/* Configure RX buffer with index RX_MAILBOX */

	CAN_ConfigRxBuff(&can_pal2_instance, RX_MAILBOX_OBC1_HV, &buffCfg, RX_MSG_ID_OBC1_HV);
	CAN_ConfigRxBuff(&can_pal2_instance, RX_MAILBOX_OBC2_HV, &buffCfg, RX_MSG_ID_OBC2_HV);
	CAN_ConfigRxBuff(&can_pal2_instance, RX_MAILBOX_ATE_HV, &buffCfg, RX_MSG_ID_ATE_HV);
    CAN_InstallEventCallback(&can_pal2_instance,MscanHVrxCallback,NULL);

    /* Start receiving data in RX_MAILBOX. */
	CAN_Receive(&can_pal2_instance, RX_MAILBOX_OBC1_HV, &recvMsg);
	status = CAN_GetTransferStatus(&can_pal2_instance, RX_MAILBOX_OBC1_HV);
	CAN_Receive(&can_pal2_instance, RX_MAILBOX_OBC2_HV, &recvMsg);
	status = CAN_GetTransferStatus(&can_pal2_instance, RX_MAILBOX_OBC2_HV);
	CAN_Receive(&can_pal2_instance, RX_MAILBOX_ATE_HV, &recvMsg);
	status = CAN_GetTransferStatus(&can_pal2_instance, RX_MAILBOX_ATE_HV);

	/* Configure TX buffer with index TX_MAILBOX_HV*/
    txbuffCfg.enableFD = false;
    txbuffCfg.enableBRS = true;
    txbuffCfg.fdPadding = 0U;
    txbuffCfg.idType = CAN_MSG_ID_STD;
    txbuffCfg.isRemote = false;
	CAN_ConfigTxBuff(&can_pal2_instance, TX_MAILBOX_HV, &txbuffCfg);
}

void MsCANSendHV_Period(void)//period=10ms
{
	static uint8_t delay_TxCnt = 0;
    static uint8_t TxRC = 0;
	can_buff_config_t txbuffCfg;

	if(++delay_TxCnt > 1);//20ms
	else return;

	delay_TxCnt = 0;
	/* Configure TX buffer with index TX_MAILBOX_HV*/
   txbuffCfg.enableFD = false;
   txbuffCfg.enableBRS = false;
   txbuffCfg.fdPadding = 0U;
   txbuffCfg.idType = CAN_MSG_ID_STD;
   txbuffCfg.isRemote = false;
   CAN_ConfigTxBuff(&can_pal2_instance, TX_MAILBOX_HV, &txbuffCfg);

	if((xJ1772Dt.xCtl.bits.S2E)&&(SysInfoData.SYS_OBC_FaultCode.bits.OBC_CAN_MissBMS == false)&&
	   (GET_PIN_VCU_OBC_EN() == false))
		OBC_InfoData_HV.OBC_TX_LLCONOFF = 1;
	else
		OBC_InfoData_HV.OBC_TX_LLCONOFF = 0;
	OBC_InfoData_HV.OBC_TX_LLCSETVOLT = Bms_ReceiveReq.BMS_OBC_MaxVoltReq;
	OBC_InfoData_HV.OBC_TX_LLCSETCURR = Bms_ReceiveReq.BMS_OBC_MaxVoltReq;
//	OBC_InfoData_HV.OBC_TX_LLCSETCURR = (Bms_ReceiveReq.BMS_OBC_MaxVoltReq < xJ1772Dt.sIoMaxI)?
//								(Bms_ReceiveReq.BMS_OBC_MaxVoltReq):(xJ1772Dt.sIoMaxI);

	messageTX.id = 0x303;
	messageTX.length = 8U;
	/* Prepare message to be sent */
	messageTX.data[0] = OBC_InfoData_HV.OBC_TX_LLCONOFF;
	messageTX.data[1] = (uint8_t)(OBC_InfoData_HV.OBC_TX_LLCSETVOLT >> 8);
	messageTX.data[2] = (uint8_t)(OBC_InfoData_HV.OBC_TX_LLCSETVOLT);
	messageTX.data[3] = (uint8_t)(OBC_InfoData_HV.OBC_TX_LLCSETCURR >> 8);
	messageTX.data[4] = (uint8_t)(OBC_InfoData_HV.OBC_TX_LLCSETCURR);
	messageTX.data[5] = 0x00;
	messageTX.data[6] = 0x00;
	TxRC = (TxRC < 250)?(TxRC + 1):(0);
	messageTX.data[7] = TxRC;
	CAN_SendBlocking(&can_pal2_instance, TX_MAILBOX_HV, &messageTX,1);//
}

void MsacnHV_SendMsgHandle(void)
{
	if(MSCAN_TO_SCI_CHN == 0)
		MsCANSendHV_Period();
}

/******************************* VCU*****************************/
void MscanHV_ReceiveHandle(void)
{
   if(mscan_rxcallback_complete == true) ;
   else return;
   mscan_rxcallback_complete = false;

   if(recvMsg.id == RX_MSG_ID_OBC1_HV)
   {
	   MainStatusData.BMS_OBC_CANTcnt = 500;
	   OBC_InfoData_HV.OBC_RX_PFC_PROTECT = ((uint16_t)recvMsg.data[0] << 8) + (uint16_t)recvMsg.data[1];//uint 0.1V
	   OBC_InfoData_HV.OBC_RX_PFC_GETVAC = ((uint16_t)recvMsg.data[2] << 8) + (uint16_t)recvMsg.data[3];//uint 0.1A
	   OBC_InfoData_HV.OBC_RX_PFC_GETIAC = ((uint16_t)recvMsg.data[4] << 8) + (uint16_t)recvMsg.data[5];//uint 0.1A
	   OBC_InfoData_HV.OBC_RX_ACFreq = ((uint16_t)recvMsg.data[6] << 8) + (uint16_t)recvMsg.data[7];//uint 0.1A
   }
   else if(recvMsg.id == RX_MSG_ID_OBC2_HV)
   {
	   MainStatusData.BMS_OBC_CANTcnt = 500;
	   OBC_InfoData_HV.OBC_RX_PROTECT = ((uint16_t)recvMsg.data[0] << 8) + (uint16_t)recvMsg.data[1];//uint 0.1A
	   OBC_InfoData_HV.OBC_RX_GETVOLT = ((uint16_t)recvMsg.data[2] << 8) + (uint16_t)recvMsg.data[3];//uint 0.1A
	   OBC_InfoData_HV.OBC_RX_GETCURR = ((uint16_t)recvMsg.data[4] << 8) + (uint16_t)recvMsg.data[5];//uint 0.1A
	   OBC_InfoData_HV.OBC_RX_PFCT = (uint8_t)recvMsg.data[6];//uint 0.1A
	   OBC_InfoData_HV.OBC_RX_TemperatureAVG = (uint8_t)recvMsg.data[7];//uint 0.1A
   }
   else if(recvMsg.id == RX_MSG_ID_ATE_HV)
   {

   }
}









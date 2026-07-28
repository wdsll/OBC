/*
 * MPC5748_mscan.c
 *
 *  Created on: 2019閿熸枻鎷�4閿熸枻鎷�2閿熸枻鎷�
 *      Author: haishengliu
 */
/* Including needed modules to compile this module/procedure */

#include <main.h>

#define SOFTWARE_VERSION   (01)
#define HARDWARE_VERSION   (01)

BMS_RECEIVE_REQ Bms_ReceiveReq;
OBC_INFO_Data  OBC_InfoData;
can_message_t recvMsg;
can_message_t recvMsgUdsBuff;
bool mscan_rxcallback_complete;
static can_message_t messageTX;
uint8_t MSCAN_TO_SCI_CHN = 0;
uint8_t canTxsci_buffer[12];
/******************** CAN ************************************/
void MscanrxCallback(void *driverState, can_event_t event, void *userData)
{
    /* Unused parameters */
    (void)driverState;
    (void)userData;

	if (event == CAN_EVENT_RX_COMPLETE) //
	{
		if((recvMsg.id == RX_MSG_ID_OBC)||(recvMsg.id == RX_MSG_ID_ATE)||(recvMsg.id == RX_MSG_ID_BOOT))  //VCU
		{
			mscan_rxcallback_complete = true;
		}
//		else if((recvMsg.id == APP_UDS_ID_PHY_RECV)||(recvMsg.id == APP_UDS_ID_FUNC_RECV))
//		{
//			Can_Rec_Diagnotor_Frame_Flag = true;
//			memcpy(&recvMsgUdsBuff, &recvMsg ,sizeof(can_message_t));
//		}
		/* Start receiving data in RX_MAILBOX. */
		CAN_Receive(&can_pal1_instance, RX_MAILBOX_OBC, &recvMsg);
		status = CAN_GetTransferStatus(&can_pal1_instance, RX_MAILBOX_OBC);
		CAN_Receive(&can_pal1_instance, RX_MAILBOX_ATE, &recvMsg);
		status = CAN_GetTransferStatus(&can_pal1_instance, RX_MAILBOX_ATE);
		CAN_Receive(&can_pal1_instance, RX_MAILBOX_BOOT, &recvMsg);
		status = CAN_GetTransferStatus(&can_pal1_instance, RX_MAILBOX_BOOT);
		CAN_Receive(&can_pal1_instance, RX_UDS_PHYS_MAILBOX, &recvMsg);
		status = CAN_GetTransferStatus(&can_pal1_instance, RX_UDS_PHYS_MAILBOX);
		CAN_Receive(&can_pal1_instance, RX_UDS_FUNC_MAILBOX, &recvMsg);
		status = CAN_GetTransferStatus(&can_pal1_instance, RX_UDS_FUNC_MAILBOX);
	}
}


void Mscan_Iint(void)
{
	can_buff_config_t buffCfg;
	can_buff_config_t txbuffCfg;

	CAN_Init(&can_pal1_instance, &can_pal1_Config0);

	buffCfg.enableFD = false;
	buffCfg.enableBRS = false;
	buffCfg.fdPadding = 0U;
	buffCfg.idType = CAN_MSG_ID_STD;
	buffCfg.isRemote = false;
	/* Configure RX buffer with index RX_MAILBOX */
	CAN_ConfigRxBuff(&can_pal1_instance, RX_MAILBOX_OBC, &buffCfg, RX_MSG_ID_OBC);
	CAN_ConfigRxBuff(&can_pal1_instance, RX_MAILBOX_ATE, &buffCfg, RX_MSG_ID_ATE);
	CAN_ConfigRxBuff(&can_pal1_instance, RX_MAILBOX_BOOT, &buffCfg, RX_MSG_ID_BOOT);
//	CAN_ConfigRxBuff(&can_pal1_instance, RX_UDS_PHYS_MAILBOX, &buffCfgExt, APP_UDS_ID_PHY_RECV);
//	CAN_ConfigRxBuff(&can_pal1_instance, RX_UDS_FUNC_MAILBOX, &buffCfgExt, APP_UDS_ID_FUNC_RECV);
    CAN_InstallEventCallback(&can_pal1_instance,MscanrxCallback,NULL);

    /* Start receiving data in RX_MAILBOX. */
	CAN_Receive(&can_pal1_instance, RX_MAILBOX_OBC, &recvMsg);
	status = CAN_GetTransferStatus(&can_pal1_instance, RX_MAILBOX_OBC);
	CAN_Receive(&can_pal1_instance, RX_MAILBOX_ATE, &recvMsg);
	status = CAN_GetTransferStatus(&can_pal1_instance, RX_MAILBOX_ATE);
	CAN_Receive(&can_pal1_instance, RX_MAILBOX_BOOT, &recvMsg);
	status = CAN_GetTransferStatus(&can_pal1_instance, RX_MAILBOX_BOOT);
    CAN_Receive(&can_pal1_instance, RX_UDS_PHYS_MAILBOX, &recvMsg);
    status = CAN_GetTransferStatus(&can_pal1_instance, RX_UDS_PHYS_MAILBOX);
    CAN_Receive(&can_pal1_instance, RX_UDS_FUNC_MAILBOX, &recvMsg);
    status = CAN_GetTransferStatus(&can_pal1_instance, RX_UDS_FUNC_MAILBOX);

	/* Configure TX buffer with index TX_MAILBOX*/
    txbuffCfg.enableFD = false;
    txbuffCfg.enableBRS = false;
    txbuffCfg.fdPadding = 0U;
    txbuffCfg.idType = CAN_MSG_ID_STD;
    txbuffCfg.isRemote = false;
	CAN_ConfigTxBuff(&can_pal1_instance, TX_MAILBOX, &txbuffCfg);
}

void MsCANSend_Period100ms(void)//period=100ms
{
	static uint8_t delay_TxCnt = 0;
    static uint8_t TxRC = 0;
    static uint8_t TxRC1 = 0;
	can_buff_config_t txbuffCfg;

	if(++delay_TxCnt>=8);
	else return;

   /* Configure TX buffer with index TX_MAILBOX*/
   txbuffCfg.enableFD = false;
   txbuffCfg.enableBRS = false;
   txbuffCfg.fdPadding = 0U;
   txbuffCfg.idType = CAN_MSG_ID_STD;
   txbuffCfg.isRemote = false;
   CAN_ConfigTxBuff(&can_pal1_instance, TX_MAILBOX, &txbuffCfg);

   if(delay_TxCnt == 8)//-----for TEST
   {
	messageTX.id = 0x234;
	messageTX.length = 8U;
	/* Prepare message to be sent */
	messageTX.data[0] = (uint8_t)(xCp.usValueRaw/100);//0.1V/Div
	messageTX.data[1] = (uint8_t)(xCp.usFreqRaw);//1Hz/Div
	messageTX.data[2] = (uint8_t)(xCp.usFreqRaw >> 8);//1Hz/Div
	messageTX.data[3] = (uint8_t)(xCp.usDutyRaw/10);//1%/Div
	messageTX.data[4] = (uint8_t)(AdcData.analog.Rcc_mcu);
	messageTX.data[5] = (uint8_t)(AdcData.analog.Rcc_mcu >> 8);
	CAN_SendBlocking(&can_pal1_instance, TX_MAILBOX, &messageTX,1);//
   }
   else if(delay_TxCnt == 9)
   {
	messageTX.id = 0x230;
	messageTX.length = 8U;
	/* Prepare message to be sent */
	memset(&messageTX.data, 0x00, 8);
	TxRC = (TxRC<15)?(TxRC+1):(0);

	OBC_InfoData.OBC_StsMode = (OBC_InfoData_HV.OBC_RX_PROTECT & 0x0080)?(2):(5);
	OBC_InfoData.OBC_BMS_ChgFaultReq = 1;
	OBC_InfoData.OBC_StsCP = xJ1772Dt.sState;
	OBC_InfoData.OBC_StsChgCC = xJ1772Dt.sCable;
	OBC_InfoData.OBC_BMS_PWMCP = xCp.usDutyRaw * 5 / 20;  //solution = 0.4
	OBC_InfoData.OBC_StsElock = 0;

	messageTX.data[0] = ((uint8_t)OBC_InfoData.OBC_StsMode);
	messageTX.data[0] |= ((uint8_t)OBC_InfoData.OBC_BMS_ChgFaultReq << 4);
	messageTX.data[1] = ((uint8_t)OBC_InfoData.OBC_StsCP << 1);
	messageTX.data[1] |= ((uint8_t)OBC_InfoData.OBC_StsChgCC << 6);
	messageTX.data[2] = ((uint8_t)OBC_InfoData.OBC_BMS_PWMCP);
	messageTX.data[3] = ((uint8_t)OBC_InfoData.OBC_StsElock << 2);
	messageTX.data[7] = TxRC & 0x0F;
	CAN_SendBlocking(&can_pal1_instance, TX_MAILBOX, &messageTX,1);//
   }
   else if(delay_TxCnt == 10)
   {
	delay_TxCnt = 0;
	messageTX.id = 0x231;
	messageTX.length = 8U;
	/* Prepare message to be sent */
	memset(&messageTX.data, 0x00, 8);

	OBC_InfoData.OBC_BMS_MaxCurDC = xJ1772Dt.sInMaxI * OBC_InfoData_HV.OBC_RX_PFC_GETVAC / OBC_InfoData_HV.OBC_RX_GETVOLT;  //solution = 0.1
	OBC_InfoData.OBC_BMS_MaxCurAC = xJ1772Dt.sInMaxI;  //solution = 0.1
	OBC_InfoData.OBC_BMS_ActVoltDC = OBC_InfoData_HV.OBC_RX_GETVOLT / 5;  //solution = 0.5
	OBC_InfoData.OBC_BMS_ActCurDC = OBC_InfoData_HV.OBC_RX_GETCURR;  //solution = 0.1
	OBC_InfoData.OBC_BMS_ActVoltAC = OBC_InfoData_HV.OBC_RX_PFC_GETVAC / 5;  //solution = 0.5
	OBC_InfoData.OBC_BMS_ActCurAC = OBC_InfoData_HV.OBC_RX_PFC_GETIAC;  //solution = 0.1

	messageTX.data[0] = (uint8_t)(OBC_InfoData.OBC_BMS_MaxCurDC >> 2);
	messageTX.data[1] = (uint8_t)((OBC_InfoData.OBC_BMS_MaxCurDC & 0x03) << 6);
	messageTX.data[1] |= (uint8_t)(OBC_InfoData.OBC_BMS_MaxCurAC >> 4);
	messageTX.data[2] = (uint8_t)((OBC_InfoData.OBC_BMS_MaxCurAC & 0x0F) << 4);
	messageTX.data[2] |= (uint8_t)(OBC_InfoData.OBC_BMS_ActVoltDC >> 8);
	messageTX.data[3] = ((uint8_t)OBC_InfoData.OBC_BMS_ActVoltDC);
	messageTX.data[4] = (uint8_t)(OBC_InfoData.OBC_BMS_ActCurDC >> 2);
	messageTX.data[5] = (uint8_t)((OBC_InfoData.OBC_BMS_ActCurDC & 0x03) << 6);
	messageTX.data[5] |= (uint8_t)(OBC_InfoData.OBC_BMS_ActVoltAC >> 4);
	messageTX.data[6] = (uint8_t)((OBC_InfoData.OBC_BMS_ActVoltAC & 0x0F) << 4);
	messageTX.data[6] |= (uint8_t)(OBC_InfoData.OBC_BMS_ActCurAC >> 6);
	messageTX.data[7] = ((uint8_t)(OBC_InfoData.OBC_BMS_ActCurAC & 0x3F) << 2);
	CAN_SendBlocking(&can_pal1_instance, TX_MAILBOX, &messageTX,1);//
   }
}

void MsCANSend_Period500ms(void)//period=500ms
{
	static uint8_t delay_TxCnt = 0;
    static uint8_t TxRC = 0;
	can_buff_config_t txbuffCfg;

	if(++delay_TxCnt>=49);
	else return;

	/* Configure TX buffer with index TX_MAILBOX*/
   txbuffCfg.enableFD = false;
   txbuffCfg.enableBRS = false;
   txbuffCfg.fdPadding = 0U;
   txbuffCfg.idType = CAN_MSG_ID_STD;
   txbuffCfg.isRemote = false;
   CAN_ConfigTxBuff(&can_pal1_instance, TX_MAILBOX, &txbuffCfg);

   if(delay_TxCnt == 49)
   {
	messageTX.id = 0x232;
	messageTX.length = 8U;
	/* Prepare message to be sent */
	memset(&messageTX.data, 0x00, 8);
//	OBC_InfoData.OBC_OBCTemp = 50 + 40;
//	OBC_InfoData.OBC_InletTemp = 60 + 40;
//	OBC_InfoData.OBC_ACInletLTerminalTemp = 50 + 40;
//	OBC_InfoData.OBC_ACInletNTerminalTemp = 60 + 40;
//	OBC_InfoData.OBC_TempFault = 0x78;
	OBC_InfoData.OBC_OBCTemp = OBC_InfoData_HV.OBC_RX_PFCT;
	OBC_InfoData.OBC_InletTemp = OBC_InfoData_HV.OBC_RX_TemperatureAVG;

	messageTX.data[0] = (uint8_t)(OBC_InfoData.OBC_OBCTemp);
	messageTX.data[1] = (uint8_t)(OBC_InfoData.OBC_InletTemp);

	messageTX.data[6] = (uint8_t)(HARDWARE_VERSION);
	messageTX.data[7] = (uint8_t)(SOFTWARE_VERSION);
	CAN_SendBlocking(&can_pal1_instance, TX_MAILBOX, &messageTX,1);//
   }
   else if(delay_TxCnt == 50)
   {
	delay_TxCnt = 0;
	messageTX.id = 0x233;
	messageTX.length = 8U;
	/* Prepare message to be sent */
	memset(&messageTX.data, 0x00, 8);
	TxRC = (TxRC<15)?(TxRC+1):(0);

	OBC_InfoData.OBC_FaultCode = 0;
	OBC_InfoData.OBC_FaultCode = (OBC_InfoData_HV.OBC_RX_PROTECT & 0x0002)?(OBC_InfoData.OBC_FaultCode | 0x01):(OBC_InfoData.OBC_FaultCode & 0xFE);//OBC OUT UVP
	OBC_InfoData.OBC_FaultCode = (OBC_InfoData_HV.OBC_RX_PROTECT & 0x0004)?(OBC_InfoData.OBC_FaultCode | 0x02):(OBC_InfoData.OBC_FaultCode & 0xFD);//OBC OUT OVP
	OBC_InfoData.OBC_FaultCode = (OBC_InfoData_HV.OBC_RX_PROTECT & 0x0010)?(OBC_InfoData.OBC_FaultCode | 0x04):(OBC_InfoData.OBC_FaultCode & 0xFB);//PFC&LLC UART
	OBC_InfoData.OBC_FaultCode = (OBC_InfoData_HV.OBC_RX_PROTECT & 0x0800)?(OBC_InfoData.OBC_FaultCode | 0x08):(OBC_InfoData.OBC_FaultCode & 0xF7);//LLC&ARM UART
	OBC_InfoData.OBC_FaultCode = (OBC_InfoData_HV.OBC_RX_PROTECT & 0x01)?(OBC_InfoData.OBC_FaultCode | 0x10):(OBC_InfoData.OBC_FaultCode & 0xEF);//OBC IN UVP
	OBC_InfoData.OBC_FaultCode = (OBC_InfoData_HV.OBC_RX_PROTECT & 0x02)?(OBC_InfoData.OBC_FaultCode | 0x20):(OBC_InfoData.OBC_FaultCode & 0xDF);//OBC IN OVP
	OBC_InfoData.OBC_FaultCode = (OBC_InfoData_HV.OBC_RX_PROTECT & 0x0008)?(OBC_InfoData.OBC_FaultCode | 0x40):(OBC_InfoData.OBC_FaultCode & 0xBF);////OBC OUT SCP
	OBC_InfoData.OBC_FaultCode = (OBC_InfoData_HV.OBC_RX_PROTECT & 0x04)?(OBC_InfoData.OBC_FaultCode | 0x80):(OBC_InfoData.OBC_FaultCode & 0x7F);//OBC IN OCP
	messageTX.data[0] = (uint8_t)(OBC_InfoData.OBC_FaultCode);

	OBC_InfoData.OBC_FaultCode = 0;
	OBC_InfoData.OBC_FaultCode = (0)?(0x01):(0xFE);//null
	OBC_InfoData.OBC_FaultCode = (SysInfoData.SYS_OBC_FaultCode.bits.OBC_CPFrequencyAbnormal)?(OBC_InfoData.OBC_FaultCode | 0x02):(OBC_InfoData.OBC_FaultCode & 0xFD);//OBC CPFrequencyAbnormal
	OBC_InfoData.OBC_FaultCode = (SysInfoData.SYS_OBC_FaultCode.bits.OBC_CPVoltAbnormal)?(OBC_InfoData.OBC_FaultCode | 0x04):(OBC_InfoData.OBC_FaultCode & 0xFB);//OBC CPVoltAbnormal
	OBC_InfoData.OBC_FaultCode = (SysInfoData.SYS_OBC_FaultCode.bits.OBC_CPDutyAbnormal)?(OBC_InfoData.OBC_FaultCode | 0x08):(OBC_InfoData.OBC_FaultCode & 0xF7);//OBC CPDutyAbnormal
	OBC_InfoData.OBC_FaultCode = (SysInfoData.SYS_OBC_FaultCode.bits.OBC_S2ControlFault)?(OBC_InfoData.OBC_FaultCode | 0x10):(OBC_InfoData.OBC_FaultCode & 0xEF);//OBC S2ControlFault
	OBC_InfoData.OBC_FaultCode = (SysInfoData.SYS_OBC_FaultCode.bits.OBC_ElockFault)?(OBC_InfoData.OBC_FaultCode | 0x20):(OBC_InfoData.OBC_FaultCode & 0xDF);//OBC ElockFault
	OBC_InfoData.OBC_FaultCode = (SysInfoData.SYS_OBC_FaultCode.bits.OBC_CAN_MissBMS)?(OBC_InfoData.OBC_FaultCode | 0x40):(OBC_InfoData.OBC_FaultCode & 0xBF);////OBC CAN MissBMS
	OBC_InfoData.OBC_FaultCode = (0)?(OBC_InfoData.OBC_FaultCode | 0x80):(OBC_InfoData.OBC_FaultCode & 0x7F);//null
	messageTX.data[1] = (uint8_t)(OBC_InfoData.OBC_FaultCode);

	messageTX.data[2] = (uint8_t)(OBC_InfoData_HV.OBC_RX_PFC_PROTECT >> 8);
	messageTX.data[3] = (uint8_t)(OBC_InfoData_HV.OBC_RX_PFC_PROTECT);
	messageTX.data[4] = (uint8_t)(OBC_InfoData_HV.OBC_RX_PROTECT >> 8);
	messageTX.data[5] = (uint8_t)(OBC_InfoData_HV.OBC_RX_PROTECT);

	messageTX.data[7] = TxRC & 0x0F;
	CAN_SendBlocking(&can_pal1_instance, TX_MAILBOX, &messageTX,1);//
   }
}

void MsacnSendMsgHandle(void)
{
	//   if(AppUds_GetCommSendFlag()) ;
	//   else return;
	if((State_TJA1145_MODE == TJA1145_NORMAL)&&(MSCAN_TO_SCI_CHN == 0))
	{
		MsCANSend_Period100ms();
		MsCANSend_Period500ms();
	}
}

/******************************* VCU*****************************/
void MscanReceiveHandle(void)
{
   if(mscan_rxcallback_complete == true) ;
   else return;
   mscan_rxcallback_complete = false;
//   if(AppUds_GetCommRecvFlag()) ;
//   else return;

   if(recvMsg.id == RX_MSG_ID_OBC)
   {
	   MainStatusData.BMS_OBC_CANTcnt = 500;
	   Bms_ReceiveReq.BMS_OBC_StsModeReq = recvMsg.data[0] & 0x0F;//Enable
	   Bms_ReceiveReq.BMS_OBC_MaxVoltReq = ((uint16_t)recvMsg.data[1] << 8) + (uint16_t)recvMsg.data[2];//uint 0.1V
	   Bms_ReceiveReq.BMS_OBC_MaxCurReq = ((uint16_t)recvMsg.data[3] << 8) + (uint16_t)recvMsg.data[4];//uint 0.1A
	   Bms_ReceiveReq.BMS_OBC_MaxCurReq = (Bms_ReceiveReq.BMS_OBC_MaxCurReq > 10000)?(Bms_ReceiveReq.BMS_OBC_MaxCurReq - 10000):(0);
   }
   else if(recvMsg.id == RX_MSG_ID_ATE)
   {
	    MainStatusData.DSP_BOOT_CANTcnt = 300;
		if((recvMsg.data[0] == 0x55)&&(recvMsg.data[1] == 0x55)&&(recvMsg.data[2] == 0x55))
			SystemSoftwareReset();
		else if((recvMsg.data[0] == 0x77)&&(recvMsg.data[1] == 0x77)&&(recvMsg.data[2] == 0x77))
			MSCAN_TO_SCI_CHN = 2;//send to obc

		else if(MSCAN_TO_SCI_CHN == 2)//send to obc
		{
			canTxsci_buffer[0] = 0xFF;
			canTxsci_buffer[1] = 0x55;
			canTxsci_buffer[2] = 8;
			canTxsci_buffer[11] = 0x57;
			memcpy(&canTxsci_buffer[3], &recvMsg.data, 8);
			//LPUART_DRV_SendData(INST_LPUART2, canTxsci_buffer, 12);
		}
   }
   else if(recvMsg.id == RX_MSG_ID_BOOT)
   {
	   MainStatusData.DSP_BOOT_CANTcnt = 300;
	   if(MSCAN_TO_SCI_CHN == 2)
		{
			canTxsci_buffer[0] = 0xFF;
			canTxsci_buffer[1] = 0x55;
			canTxsci_buffer[2] = recvMsg.length;
			canTxsci_buffer[11] = 0x01;
			memcpy(&canTxsci_buffer[3], &recvMsg.data, 8);
			//LPUART_DRV_SendData(INST_LPUART2, canTxsci_buffer, 12);
		}
   }
}









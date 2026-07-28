/*
 * CAN_Transmit_Receive.c
 *
 *  Created on: 11 juli 2021
 *      Author: Daniel Mårtensson
 */

 /* Layer */
#include "Hardware.h"
#include "includeAll.h"

extern CanTxMessage CAN_TxMessage;
extern CanRxMessage CAN_RxMessage;


ENUM_J1939_STATUS_CODES CAN_Send_Message(uint32_t ID, uint8_t data[]) {
	ENUM_J1939_STATUS_CODES status = STATUS_SEND_BUSY;
	uint8_t mailbox_queue = CAN_TXSTS_MAILBOX0;

	/*
	CAN_TxHeaderTypeDef TxHeader;
	TxHeader.DLC = 8;											
	TxHeader.RTR = CAN_RTR_DATA;							
	TxHeader.IDE = CAN_ID_EXT;									
	TxHeader.TransmitGlobalTime = DISABLE;
	TxHeader.ExtId = ID;
	TxHeader.StdId = 0x00; 										
	status = STM32_PLC_CAN_Transmit(data, &TxHeader);
*/
    CAN_TxMessage.StdId   = 0x00;       
    CAN_TxMessage.ExtId   = ID;        //  PF PGN ; PS F4 ; SA 56
    CAN_TxMessage.IDE     = CAN_EXTENDED_ID;           /* CAN_ID_STD / CAN_ID_EXT */
    CAN_TxMessage.RTR     = CAN_RTRQ_DATA;           /* CAN_RTR_DATA / CAN_RTR_REMOTE */
    CAN_TxMessage.DLC     = 8;           /* 0 to 8 */
    CAN_TxMessage.Data[0] = data[0];
    CAN_TxMessage.Data[1] = data[1];
    CAN_TxMessage.Data[2] = data[2];
    CAN_TxMessage.Data[3] = data[3];
    CAN_TxMessage.Data[4] = data[4];
    CAN_TxMessage.Data[5] = data[5];
    CAN_TxMessage.Data[6] = data[6];
    CAN_TxMessage.Data[7] = data[7];	
	
	mailbox_queue=CAN_Transmit_Message_initializes(CAN,&CAN_TxMessage);
	
	if (mailbox_queue != CAN_TXSTS_NOMAILBOX)
		status = STATUS_SEND_OK;

	/* Display traffic */
//	if (Callback_Function_Traffic != NULL) {
//		Callback_Function_Traffic(ID, 8, data, true); /* ID, 8 bytes of data, data array, TX = true */
//	}

	return status;
}

/* Send a PGN request
 * PGN: 0x00EA00 (59904)
 */
ENUM_J1939_STATUS_CODES CAN_Send_Request(uint32_t ID, uint8_t PGN[]) {
	ENUM_J1939_STATUS_CODES status = STATUS_SEND_BUSY;
	uint8_t mailbox_queue = CAN_TXSTS_MAILBOX0;

    CAN_TxMessage.StdId   = 0x00;       
    CAN_TxMessage.ExtId   = ID;        //  PF PGN ; PS F4 ; SA 56
    CAN_TxMessage.IDE     = CAN_EXTENDED_ID;           /* CAN_ID_STD / CAN_ID_EXT */
    CAN_TxMessage.RTR     = CAN_RTRQ_DATA;           /* CAN_RTR_DATA / CAN_RTR_REMOTE */
    CAN_TxMessage.DLC     = 3;           /* 0 to 8 */
    CAN_TxMessage.Data[0] = PGN[0];
    CAN_TxMessage.Data[1] = PGN[1];
    CAN_TxMessage.Data[2] = PGN[2];
    CAN_TxMessage.Data[3] = 0;
    CAN_TxMessage.Data[4] = 0;
    CAN_TxMessage.Data[5] = 0;
    CAN_TxMessage.Data[6] = 0;
    CAN_TxMessage.Data[7] = 0;	
	
	mailbox_queue=CAN_Transmit_Message_initializes(CAN,&CAN_TxMessage);
	
	if (mailbox_queue != CAN_TXSTS_NOMAILBOX)
		status = STATUS_SEND_OK;	
	
	return status;
}

/* Read the current CAN-bus message. Returning false if the message has been read before, else true */

unsigned char CAN_Read_Message(uint32_t* ID, uint8_t data[]) {
	unsigned char is_new_message = 0;
	//STM32_PLC_CAN_Get_ID_Data(ID, data, &is_new_message);
	return is_new_message;
}


extern volatile uint32_t systick ;
void CAN_Delay(uint8_t milliseconds) {
uint32_t tick=systick;
while((systick-tick)<milliseconds*10);
}

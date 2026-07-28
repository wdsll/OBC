/**
*     Copyright (c) 2022, Nations Technologies Inc.
* 
*     All rights reserved.
*
*     This software is the exclusive property of Nations Technologies Inc. (Hereinafter 
* referred to as NATIONS). This software, and the product of NATIONS described herein 
* (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
* of the People's Republic of China and other applicable jurisdictions worldwide.
*
*     NATIONS does not grant any license under its patents, copyrights, trademarks, or other 
* intellectual property rights. Names and brands of third party may be mentioned or referred 
* thereto (if any) for identification purposes only.
*
*     NATIONS reserves the right to make changes, corrections, enhancements, modifications, and 
* improvements to this software at any time without notice. Please contact NATIONS and obtain 
* the latest version of this software before placing orders.

*     Although NATIONS has attempted to provide accurate and reliable information, NATIONS assumes 
* no responsibility for the accuracy and reliability of this software.
* 
*     It is the responsibility of the user of this software to properly design, program, and test 
* the functionality and safety of any application made of this information and any resulting product. 
* In no event shall NATIONS be liable for any direct, indirect, incidental, special,exemplary, or 
* consequential damages arising in any way out of the use of this software or the Product.
*
*     NATIONS Products are neither intended nor warranted for usage in systems or equipment, any
* malfunction or failure of which may cause loss of human life, bodily injury or severe property 
* damage. Such applications are deemed, "Insecure Usage".
*
*     All Insecure Usage shall be made at user's risk. User shall indemnify NATIONS and hold NATIONS 
* harmless from and against all claims, costs, damages, and other liabilities, arising from or related 
* to any customer's Insecure Usage.

*     Any express or implied warranty with regard to this software or the Product, including,but not 
* limited to, the warranties of merchantability, fitness for a particular purpose and non-infringement
* are disclaimed to the fullest extent permitted by law.

*     Unless otherwise explicitly permitted by NATIONS, anyone may not duplicate, modify, transcribe
* or otherwise distribute this software for any purposes, in whole or in part.
*
*     NATIONS products and technologies shall not be used for or incorporated into any products or systems
* whose manufacture, use, or sale is prohibited under any applicable domestic or foreign laws or regulations. 
* User shall comply with any applicable export control laws and regulations promulgated and administered by 
* the governments of any countries asserting jurisdiction over the parties or transactions.
**/

/**
*\*\file main.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
**/

#include "main.h"
#include "includeAll.h"
uint16_t Rx_Flag = DISABLE;





/**
*\*\name   main
*\*\fun    Main program.
*\*\param  none
*\*\return none
**/
int main2(void)
{
    uint32_t wait_slak;
		uint32_t   PGN=0x260000;
//    SetSysClockTo48M();
    /* CAN configure */
    CAN_CONFIG();
    
    /* Transmit assign */

    CAN_TxMessage.StdId   = 0x0400;       
    CAN_TxMessage.ExtId   = PGN|0xF456;        //  PF PGN ; PS F4 ; SA 56
    CAN_TxMessage.IDE     = CAN_EXTENDED_ID;           /* CAN_ID_STD / CAN_ID_EXT */
    CAN_TxMessage.RTR     = CAN_RTRQ_DATA;           /* CAN_RTR_DATA / CAN_RTR_REMOTE */
    CAN_TxMessage.DLC     = 8;           /* 0 to 8 */
    CAN_TxMessage.Data[0] = 0x00;
    CAN_TxMessage.Data[1] = 0x01;
    CAN_TxMessage.Data[2] = 0x02;
    CAN_TxMessage.Data[3] = 0x03;
    CAN_TxMessage.Data[4] = 0x04;
    CAN_TxMessage.Data[5] = 0x05;
    CAN_TxMessage.Data[6] = 0x06;
    CAN_TxMessage.Data[7] = 0x07;
    while(1)
    {
        /* Transmit */
        CAN_Transmit_Message_initializes(CAN,&CAN_TxMessage);
        while(Rx_Flag == DISABLE)
        {
            Rx_Flag = Check_CanRecData(&CAN_RxMessage, 0x0400, PGN|0xF456, CAN_EXTENDED_ID, CAN_RTRQ_DATA, 8,
                         0x00,0x01, 0x02, 0x03, 
                         0x04,0x05, 0x06, 0x07, 0);
        }
        
        /* Delay */
        wait_slak = 0xFFFF;
        while(wait_slak>0)
        {
            wait_slak--;
        }
        Rx_Flag = DISABLE;
    }
}

extern unsigned char can_tx_flag;
extern volatile unsigned char can_rx_flag;
extern uint16_t requested_current;
extern const uint8_t RATED_CURRENT;
extern const uint16_t ADJ_CURRENTx4[];
extern uint16_t requested_voltage;



extern volatile uint32_t systick ;
void CAN_Delay() {
    uint16_t    wait_slak = 1200;
        while(wait_slak>0)
        {
            wait_slak--;
        }
}

void can_delay_send(void) {

	static uint32_t can_tick = 0;
	if(can_tx_flag ==1) {
    can_tick = systick;
		can_tx_flag = 2;
	}
	else if(can_tx_flag == 2) {
		uint32_t spend =  systick-can_tick;
		if(spend > 2*1000) {
      can_tx_flag=0;
			CAN_Transmit_Message_initializes(CAN,&CAN_TxMessage); 
		}
	}
}
uint16_t ADC_GetData(uint8_t ADC_Channel);


extern uint8_t charging_control_bits;
unsigned char bBmsReadyF=0;
unsigned char bChgReadyF=0;
void can_tx_rx(void)
{

	static unsigned char bmsVersionOK=0;
	static uint16_t batMaxI=100; //最高允许充电电流 16
	static uint16_t batMaxV=MAX_LIMITED_VOLTAGE; //最高允许充电总电压 16
	static uint8_t  batMaxT=120; //最高允许温度  8
	static uint8_t  batSOC=0; //整车动力蓄电池荷电状态SOC(%) 8
	static uint16_t batV=MIN_LIMITED_VOLTAGE; //整车动力蓄电池当前电池电压 16
//	static uint16_t batAH=100; //容量

	uint32_t temp;
 if(can_rx_flag>0)
 {
	 
	
	 if(CAN_RxMessage.ExtId ==  0x182756f4) // BMS握手
	 {  
		  batMaxV=CAN_RxMessage.Data[0]+(CAN_RxMessage.Data[1]<<8);  //最高电压
		 
    CAN_TxMessage.StdId   = 0x0000;       
    CAN_TxMessage.ExtId   = 0x1826f456;      //充电器握手
    CAN_TxMessage.IDE     = CAN_EXTENDED_ID;      
    CAN_TxMessage.RTR     = CAN_RTRQ_DATA;        
    CAN_TxMessage.DLC     = 8;   
    CAN_TxMessage.Data[0] = 1;
    CAN_TxMessage.Data[1] = 1;
    CAN_TxMessage.Data[2] = 0;
    CAN_TxMessage.Data[3] = 0xFF;
    CAN_TxMessage.Data[4] = 0xFF;
    CAN_TxMessage.Data[5] = 0xFF;
    CAN_TxMessage.Data[6] = 0xFF;            
    CAN_TxMessage.Data[7] = 0xFF;	
    CAN_Delay();	
    CAN_Transmit_Message_initializes(CAN,&CAN_TxMessage); 	
    can_tx_flag =1;				
	 }	 
	 
	 
	 
	 	 if(CAN_RxMessage.ExtId ==  0x1C0256f4) // BMS辨识
	 {  //bChgReadyF=0xAA;
			if(CAN_RxMessage.Data[0]==1 && CAN_RxMessage.Data[1]==1 && CAN_RxMessage.Data[2]==0 )  bmsVersionOK =1;	 //version 0.1.1
		  else bmsVersionOK=0;
		//  if(CAN_RxMessage.Data[3]!=3) bChgReadyF =0; //03H:磷酸铁锂电池
		//  temp=CAN_RxMessage.Data[4]+ (CAN_RxMessage.Data[5]<<8);  
		//  if(temp<10) bChgReadyF =0; //容量AH
		//  batAH = temp ;
		// 	temp=CAN_RxMessage.Data[6]+ (CAN_RxMessage.Data[7]<<8);  
		//  if(temp!=480) bChgReadyF =0; //额定电压48V
		 
    CAN_TxMessage.StdId   = 0x0000;       
    CAN_TxMessage.ExtId   = 0x1801f456;      //充电器辨识
    CAN_TxMessage.IDE     = CAN_EXTENDED_ID;      
    CAN_TxMessage.RTR     = CAN_RTRQ_DATA;        
    CAN_TxMessage.DLC     = 8;   
		 if(bmsVersionOK)
    CAN_TxMessage.Data[0] = 0xAA;
		 else     CAN_TxMessage.Data[0] =0;
    CAN_TxMessage.Data[1] = 0x55;
    CAN_TxMessage.Data[2] = 0x55;
    CAN_TxMessage.Data[3] = 0x55;
    CAN_TxMessage.Data[4] = 0x55;
    CAN_TxMessage.Data[5] = 0xFF;
    CAN_TxMessage.Data[6] = 0xFF;            
    CAN_TxMessage.Data[7] = 0xFF;	
    CAN_Delay();	
    CAN_Transmit_Message_initializes(CAN,&CAN_TxMessage); 	
    can_tx_flag =1;				
	 }	 
	 
	 
 
	 	 if(CAN_RxMessage.ExtId ==  0x1C0656f4) //BCP 动力蓄电池充电参数
	 {   		 
    batMaxI=CAN_RxMessage.Data[0]+(CAN_RxMessage.Data[1]<<8); //最高允许充电电流 16
    batMaxV=CAN_RxMessage.Data[2]+(CAN_RxMessage.Data[3]<<8); //最高允许充电总电压 16
    batMaxT=CAN_RxMessage.Data[4]; //最高允许温度  8
    batSOC=CAN_RxMessage.Data[5]; //整车动力蓄电池荷电状态SOC(%) 8
    batV=CAN_RxMessage.Data[6]+(CAN_RxMessage.Data[7]<<8); //整车动力蓄电池当前电池电压 16
		 
 	  charging_control_bits &= ~NNS_BMS_BATTERY_TYPE_ERR;
    if((batMaxI<10)|| (batMaxV<420) || (batMaxT<50))
	  charging_control_bits |= NNS_BMS_BATTERY_TYPE_ERR;
 
 		if(batV <= MAX_LIMITED_VOLTAGE) {
			charging_control_bits &= ~NNS_BMS_VOLTAGE_ERR; //电压相符
		} else {
			charging_control_bits |= NNS_BMS_VOLTAGE_ERR;  //电压不符,禁止充电
		}		 
		

		 
    CAN_TxMessage.StdId   = 0x0000;       
    CAN_TxMessage.ExtId   = 0x1807f456;      //充电器最大输出能力
    CAN_TxMessage.IDE     = CAN_EXTENDED_ID;      
    CAN_TxMessage.RTR     = CAN_RTRQ_DATA;        
    CAN_TxMessage.DLC     = 8;   
    CAN_TxMessage.Data[0] = (uint8_t)MAX_LIMITED_VOLTAGE; //最高输出电压(V) 数据分辨率:0.1V/位
    CAN_TxMessage.Data[1] = (uint8_t)(MAX_LIMITED_VOLTAGE>>8); 
    CAN_TxMessage.Data[2] = (uint8_t)MIN_LIMITED_VOLTAGE; //最低输出电压(V)数据分辨率:0.1V/位
    CAN_TxMessage.Data[3] = (uint8_t)(MIN_LIMITED_VOLTAGE>>8);
    CAN_TxMessage.Data[4] = (uint8_t)MAX_LIMITED_CURRENT; //最大输出电流(A) 数据分辨率:0.1A/位
    CAN_TxMessage.Data[5] = (uint8_t)(MAX_LIMITED_CURRENT>>8);
    CAN_TxMessage.Data[6] = (uint8_t)MIN_LIMITED_CURRENT; //最小输出电流(A) 数据分辨率:0.1A/位          
    CAN_TxMessage.Data[7] = (uint8_t)(MIN_LIMITED_CURRENT>>8);	
    CAN_Delay();	
    CAN_Transmit_Message_initializes(CAN,&CAN_TxMessage); 	
    can_tx_flag =1;				
	 }	 
	 	 	 

	 	 if(CAN_RxMessage.ExtId ==  0x100956f4) // 电池充电准备就绪状态
	 {   		 
    if(CAN_RxMessage.Data[0] == 0xAA)   bBmsReadyF=0xAA; //BMS完成充电准备
		 else bBmsReadyF = 0;
		 
if(((charging_control_bits & (NNS_BMS_VOLTAGE_ERR | NNS_BMS_BATTERY_TYPE_ERR ) )>0)||(bmsVersionOK == 0)) 	bChgReadyF=0;
else 		bChgReadyF=0xAA; 		
		 
		 
    CAN_TxMessage.StdId   = 0x0000;       
    CAN_TxMessage.ExtId   = 0x100af456;     //充电器输出准备就绪状态
    CAN_TxMessage.IDE     = CAN_EXTENDED_ID;      
    CAN_TxMessage.RTR     = CAN_RTRQ_DATA;        
    CAN_TxMessage.DLC     = 8;   
    CAN_TxMessage.Data[0] = bChgReadyF; 
    CAN_TxMessage.Data[1] = 0xFF;	
    CAN_TxMessage.Data[2] = 0xFF;	
    CAN_TxMessage.Data[3] = 0xFF;	
    CAN_TxMessage.Data[4] = 0xFF;	//charging_control_bits;	
    CAN_TxMessage.Data[5] = 0xFF;	//bmsVersionOK;	
    CAN_TxMessage.Data[6] = 0xFF;	//batV;	          
    CAN_TxMessage.Data[7] = 0xFF;	//batV>>8;	
    CAN_Delay();	
    CAN_Transmit_Message_initializes(CAN,&CAN_TxMessage); 	
    can_tx_flag =1;				
	 }	 	 
	 
	 
	 
	 if(CAN_RxMessage.ExtId == 0x181056F4) // BCL 电池充电需求
	 {
		  temp=CAN_RxMessage.Data[0]+(CAN_RxMessage.Data[1]<<8);  //请求电压
			if(temp>MAX_LIMITED_VOLTAGE) temp=MAX_LIMITED_VOLTAGE;
		 	if(temp<MIN_LIMITED_VOLTAGE) temp=MIN_LIMITED_VOLTAGE;
	    requested_voltage=temp; 		 
		 
		 
		  temp=CAN_RxMessage.Data[2]+(CAN_RxMessage.Data[3]<<8); //请求电流
			if(temp>RATED_CURRENT) temp=RATED_CURRENT;
	    requested_current=ADJ_CURRENTx4[temp]; 
		 
    CAN_TxMessage.StdId   = 0x0000;       
    CAN_TxMessage.ExtId   = 0x1812F456;      //CCS  充电器充电状态
    CAN_TxMessage.IDE     = CAN_EXTENDED_ID;      
    CAN_TxMessage.RTR     = CAN_RTRQ_DATA;        
    CAN_TxMessage.DLC     = 8;   
		 temp=  adcGet(ADC_CHANNEL_BAT_VOLTAGE)/737; //adcGet(ADC_CHANNEL_BAT_VOLTAGE)/1000; //电压
    CAN_TxMessage.Data[0] = temp;
    CAN_TxMessage.Data[1] = temp>>8;
		 temp=  adcGet(ADC_CHANNEL_CHARGING_CURRENT)/355; //icGet(adcGet(ADC_CHANNEL_CHARGING_CURRENT));   //电流
    CAN_TxMessage.Data[2] = temp;
    CAN_TxMessage.Data[3] = temp>>8;
		 temp=nns_get_step_time_min();              //时长
    CAN_TxMessage.Data[4] = temp;
    CAN_TxMessage.Data[5] = temp>>8;
    CAN_TxMessage.Data[6] = 1;             //允许
    CAN_TxMessage.Data[7] = 0xFF;	
    CAN_Delay();	
    CAN_Transmit_Message_initializes(CAN,&CAN_TxMessage); 	
     can_tx_flag =1;
	 }


 
	 
	 if(CAN_RxMessage.ExtId == 0x1C1156F4) // BCS 电池充电总状态
	 {
//充电电压测量值(V) 16
//充电电流测量值(A) 16
batSOC=CAN_RxMessage.Data[4]; //当前荷电状态SOC(%) 8
//估算剩余充电时间(min) 16

	 }

	 if(CAN_RxMessage.ExtId == 0x181356f4) // BSM 动力蓄电池状态信息
	 {
//0  最高动力蓄电池温度	
//15~8	最低动力蓄电池温度	
//17-16	整车动力蓄电池荷电状态 SOC 过高/过低	"<00>:=正常;<01>:=过高;<10>:=过低"
//19-18	动力蓄电池充电过电流	"<00>:=正常;<01>:=过流;<10>:=不可信状态"
//21-20	动力蓄电池温度过高	"<00>:=正常;<01>:=过高; <10>:=不可信状态"
//23-22	"动力蓄电池绝缘状态(<00>:=正常;<01>:=不正 常;<10>:=不可信状态) "	
//25-24	动力蓄电池组输出MOS或连接器连接状态	"<00>:= 正常; <01>:=不正常;<10>:=不可信状态"
//27-26	充电允许	"<00>:=禁止;<01>:=允许"
//63-28	

//27-26	充电允许	"<00>:=禁止;<01>:=允许"		 
	if((CAN_RxMessage.Data[3] & 0x0C) == 0x04 ) charging_control_bits &= ~NNS_BMS_FORBID_ERR;//允许充电
	if((CAN_RxMessage.Data[3] & 0x0C) == 0x00 ) charging_control_bits |= NNS_BMS_FORBID_ERR;//禁止充电
		 
	 }
	 
	 if(CAN_RxMessage.ExtId == 0x101956f4) //  BST BMS中止充电
	 {

//第1~2位:达到所需求的SOC目标值 <00>:=未达到所需SOC目标值;<01>:=达到所需 SOC目标值;<10>:=不可信状态; 
//第3~4位:达到总电压的设定值 <00>:=未达到总电压设定值;<01>:=达到总电压设定值;<10>:=不可信状态; 
//第5~6位:达到单体电压的设定值 <00>:=未达到单体电压设定值;<01>:=达到单体电压设定值;<10>:=不可信状态; 
//第7~8位:充电器主动中止 <00>:=正常;<01>:=充电器中止(收到 CST 帧);<10>:=不可信状态。		 		 
//8-31
//第1~2位:绝缘故障 <00>:=正常;<01>:=故障;<10>:=不可信状态; 
//第3~4位:输出MOS或连接器过温故障 <00>:=正常;<01>:=故障;<10>:=不可信状态; 
//第5~6位:BMS元件、输出MOS或连接器过温 <00>:=正常;<01>:=故障;<10>:=不可信状态; 
//第7~8位:充电MOS或连接器故障 <00>:=充电MOS或连接器正常;<01>:=充电MOS或连接器故障;<10>:=不可信状态; 
//第9~10位:电池组温度过高故障 <00>:=电池组温度正常;<01>:=电池组温度过高;<10>:=不可信状态; 
//第11~12位:高压继电器故障 <00>:=正常;<01>:=故障;<10>:=不可信状态; 
//第13~14位:检测点2电压检测故障 <00>:=正常;<01>:=故障;<10>:=不可信状态; 
//第15~16位:其他故障 <00>:=正常;<01>:=故障;<10>:=不可信状态。
//32-39
//第1~2位:电流过大 <00>:=电流正常;<01>:=电流超过需求值;<10>:=不可信状态; 
//第3~4位:电压异常 <00>:=正常;<01>:=电压异常;<10>:=不可信状态。


	charging_control_bits |= NNS_BMS_FORBID_ERR;//禁止充电		 

//0x101af456 充电器中止充电
//7-0 充电器中止充电原因
//"第1~2位:达到充电器设定的条件中止 <00>:=正常;<01>:=达到充电器设定条件中止;<10>:=不可信状态; 
//第3~4位:人工中止 <00>:=正常;<01>:=人工中止;<10>:=不可信状态; 
//第5~6位:故障中止 <00>:=正常;<01>:=故障中止;<10>:=不可信状态; 
//第7~8位:BMS主动中止 <00>:=正常;<01>:=BMS中止(收到 BST 帧);<10>:=不可信状态。"
//31~8 充电器中止充电故障原因
//"第1~2位:充电器过温故障 <00>:=充电器温度正常;<01>:=充电器过温;<10>:=不可信状态;
//第3~4位:充电MOS或连接器故障 <00>:=充电MOS或连接器正常;<01>:=充电MOS或连接器故障;<10>:=不可信状态; 
//第5~6位:充电器内部过温故障 <00>:=充电器内部温度正常;<01>:=充电器内部过温;<10>:=不可信状态; 
//第7~8位:所需电量不能传送 <00:=电量传送正常;<01>:电量不能传送;<10>:=不可信状态; 
//第9~10位:充电器急停故障 <00>:=正常;<01>:=充电器急停;<10>:=不可信状态; 
//第11~12位:其他故障 <00>:=正常;<01>:=故障;<10>:=不可信状态。"
//39-32 充电器中止充电错误原因
//"第1~2位:电流不匹配 <00>:=电流匹配;<01>:=电流不匹配;<10>:=不可信状态; 
//第3~4位:电压异常 <00>:=正常;<01>:=电压异常;<10>:=不可信状态。" 		

	if((CAN_RxMessage.Data[0] & 0xC0 ) != 0x40)	 //充电器主动中止的就不重发了 第7~8位:充电器主动中止 <00>:=正常;<01>:=充电器中止(收到 CST 帧);<10>:=不可信状态。	
	{	 
    CAN_TxMessage.StdId   = 0x0000;       
    CAN_TxMessage.ExtId   = 0x101af456;      // CST 充电器中止充电
    CAN_TxMessage.IDE     = CAN_EXTENDED_ID;      
    CAN_TxMessage.RTR     = CAN_RTRQ_DATA;        
    CAN_TxMessage.DLC     = 8;   
    CAN_TxMessage.Data[0] = 0x40; //第7~8位:BMS主动中止 <00>:=正常;<01>:=BMS中止(收到 BST 帧);<10>:=不可信状态。
    CAN_TxMessage.Data[1] = 0;
    CAN_TxMessage.Data[2] = 0;
    CAN_TxMessage.Data[3] = 0;
    CAN_TxMessage.Data[4] = 0xFF;
    CAN_TxMessage.Data[5] = 0xFF;
    CAN_TxMessage.Data[6] = 0xFF;      
    CAN_TxMessage.Data[7] = 0xFF;	
    CAN_Delay();	
    CAN_Transmit_Message_initializes(CAN,&CAN_TxMessage); 	
     can_tx_flag =1;		 
	}		 

	
	 }	 
	 
	can_rx_flag=0;	 
	 
	if(batSOC<100)  charging_control_bits &= ~NNS_BMS_FULL; //未充满
	else charging_control_bits |= NNS_BMS_FULL; //已充满	 
	 
 }
	




 
}








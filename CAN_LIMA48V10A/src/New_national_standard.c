/******************************************************************************
Copyright (c) 2023-2027, Luxshare WuZhong Power R&D Team
FileName : New_national_standard.c
Author : Jeffery
Date : 2023年8月25日
Version : 
- 16 -
IDE Version : 
Description : 
: XXXXXX
History : 
<Author> <Date> <Version > // 修改文件的作者 时间 修改后的版本号
Ayma 20/07/05 V1.1 //
<Desc> 1. Add func // 修改描述 增加、删除、修改函数名称
2. Delete func
3. Modify func
<Author> <Date> <Version > // 修改文件的作者 时间 修改后的版本号
Ayma 20/07/06 V1.2 //
<Desc> 1. Add func
2. Delete func
3. Modify func
******************************************************************************/


/************************************************************
* Header File
************************************************************/
#include "includeAll.h"

/************************************************************
* Macro Definition
************************************************************/


/************************************************************
* Enum And Structure Definition
************************************************************/
uint8_t received_counter = 0 ;
static volatile uint8_t nns_rx[257]={0};
static volatile uint8_t nns_rx_idx = 0;
volatile uint8_t 		pack_pos=0;
volatile uint8_t 		pack_flag=0;
static uint8_t nns_blind_charge_allowed=0;
//static uint8_t nns_enable_charge=1;  //0 blind,1 smart.
//static uint8_t nns_rx_idx_head = 0;
//static beTrue_t haveNewData = 0;
static uint32_t charger_state = 0;
//struct {
//	uint8_t start0;
//	uint8_t start1;
//	uint8_t id;
//	uint8_t length;
//	uint8_t
//};
//static uint8_t has_communication=0;

static volatile uint32_t rx_tick_marker = 0;
static uint32_t rx_tick_received_marker = 0;
volatile uint32_t systick_100us = 0;
//#define NNS_INITIAL_MESSAGE_HVC    0
//#define NNS_INITIAL_MESSAGE_PID		1
//#define NNS_INITIAL_MESSAGE_HVN   2
//#define NNS_INITIAL_MESSAGE_SVN  3
//#define NNS_INITIAL_MESSAGE_MSVN 4
//#define NNS_INITIAL_MESSAGE_
//#define NNS_INITIAL_MESSAGE_
//#define NNS_INITIAL_MESSAGE_
//#define NNS_INITIAL_MESSAGE_
//#define NNS_INITIAL_MESSAGE_
//#define NNS_INITIAL_MESSAGE_
static enable1_t sendFlag = 0;
//static uint32_t receiveTickMarker = 0;
static uint8_t responseFrameLength = 0;
static uint8_t responseBuff[32];
/************************************************************
* Internal Parameter And Function
************************************************************/
uint8_t charging_control_bits = NNS_BMS_HANDSHAKE_ERR;
static uint32_t stage_take_time = 0;

extern uint16_t requested_current;
extern const uint8_t RATED_CURRENT;
extern const uint16_t ADJ_CURRENTx4[];
uint8_t get_charging_control_bits(void)
{return charging_control_bits; }

uint8_t get_blind_charge(void)
{return nns_blind_charge_allowed; }

void reset_charging_control_bits(void)
{charging_control_bits = NNS_BMS_HANDSHAKE_ERR; }

/*
 * 新国标初始报文应答
 */
 void nns_initial_response_fill(void) {
/*    	硬件厂商代码        	B0
		产品型号              	B1
		硬件版本序号          	B2
		地区                 	B3
		次软件版本号          	B4
		主软件版本号          	B5
		BOOT底层软件版本      	B6
		BOOT升级协议版本      	B7
		次通信协议版本        	B8
		主通信协议版本   		B9			*/
	responseFrameLength = 0;
	responseBuff[responseFrameLength++] = 0x59;
	responseBuff[responseFrameLength++] = 0x44;
	responseBuff[responseFrameLength++] = 0xA8;
	responseBuff[responseFrameLength++] = 16;

	responseBuff[responseFrameLength++] =		7;	//B0   硬件厂商代码
	responseBuff[responseFrameLength++]	= 0x01;			//B1     产品型号
	responseBuff[responseFrameLength++]	= 0x01;			//B2     硬件版本序号
	responseBuff[responseFrameLength++]	= 0x00;			//B3     地区

	responseBuff[responseFrameLength++]	= 0x00;			//B4     次软件版本
	responseBuff[responseFrameLength++]	= 0x01;			//B5     主软件版本
	responseBuff[responseFrameLength++]	= 0xFF;		//B6      boot低层软件
	responseBuff[responseFrameLength++]	= 0xFF;	//B7		boot升级协议版本

	responseBuff[responseFrameLength++]	 = 0x00;	//B8		次通讯协议版本
	responseBuff[responseFrameLength++]	 = 0x01;		//B9		主通讯协议版本
	responseBuff[responseFrameLength++]	 = 0xFF;	//B10
	responseBuff[responseFrameLength++]	 = 0xFF;//B11

	responseBuff[responseFrameLength++]	= 0xFF;//B12
	responseBuff[responseFrameLength++] = 0xFF;		//B13
	responseBuff[responseFrameLength++] = 0xFF;		//B14
	responseBuff[responseFrameLength++] = 0xFF;	//B15
	{int i;
	uint8_t checksum = 0;
	for(i = 2 ; i < 20 ; i++) {
		checksum += responseBuff[i];
	}
	responseBuff[responseFrameLength++] = checksum	;//checksum
	}
	responseBuff[responseFrameLength++] = 0x4B	;
	responseBuff[responseFrameLength++] = 0x4A	;
	sendFlag = 1;
}
/*
 * 新国标周期报文应答
 */
static void nns_periodic_response_fill(void) {

/*    充电器类型		    B0	B0.0	4	    B0. 3
			充电器状态	 	    B0	B0.4	2	    B0. 5
			充电器输出状态 	    B0	B0.6	2	    B0. 7

			充电完成状态	    B1	B1.0	2	    B1. 1
			过温保护状态	    B1	B1.2	2	    B1.3
			环境过温状态	    B1	B1.4	2	    B1. 5
			充电过流状态	    B1	B1.6	2	    B1. 7

			充电过压状态	    B2	B2.0	2	    B2. 1
			充电欠压状态	    B2	B2.2	2	    B2.3
			充电阶段		    B2	B0.4	2	    B0. 5
			预留			    B2	B2.6	2	    B2. 7

			阶段时长	        B3	B3.0	8X2	    B4.7
			额定电压	        B5	B5.0	8	    B5. 7
			额定电流	        B6	B6.0	8	    B6. 7
			实时电压	        B7	B7.0	8X2	    B8. 7
			实时电流	        B9	B9.0	8X2	    B10. 7
			实时环温	        B11	B11.0	8	    B11. 7
			内部温度	        B12	B12.0	8	    B12. 7
			  预留			B13	B13.0	8X3	    B15. 7*/
	uint8_t temp = 0 , temp1 = 0 , temp2 = 0;
	
	responseFrameLength = 0;
	responseBuff[responseFrameLength++] = 0x59;
	responseBuff[responseFrameLength++] = 0x44;
	responseBuff[responseFrameLength++] = 0xA9;
	responseBuff[responseFrameLength++] = 16;
	{
		
		if(charger_state & (NNS_CHARGER_STATE_D102_OTP | NNS_CHARGER_STATE_ENV_OTP | NNS_CHARGER_STATE_NTC_OPEN
				| NNS_CHARGER_STATE_NTC_SHORT | NNS_CHARGER_STATE_FAN_ERR | NNS_CHARGER_STATE_5V_FAULT)) {
			temp1 = 1;//充电器异常
		}
		if(charger_state & NNS_CHARGER_STATE_SCR) {
			temp2 = 1;//充电器输出
		}
		responseBuff[responseFrameLength++] =	 (temp2 << 6) |	(temp1 << 4) | 0;	//B0 temp2 bit7 bit6   temp1 bit5 bit4   temp bit3 bit2 bit1 bit0
	}
	{
		//uint8_t temp = 0 , temp1 = 0 , temp2 = 0; //base on yadi's standard, triple will be treat as finished.
		if(charger_state & (NNS_CHARGER_STATE_FINISHED|NNS_CHARGER_STATE_TRI)) {
			temp = 1;//充电完成
		} else temp = 0;  // not finished.
		if(charger_state & (NNS_CHARGER_STATE_D102_OTP | NNS_CHARGER_STATE_ENV_OTP)) {
			temp1 = 2;//过温保护状态,warning=1,protect=2
		} else temp1=0;
		if(charger_state & NNS_CHARGER_STATE_ENV_OTP) {
			temp2 = 2;//环境过温状态,warning=1, protect=2;
		}else temp2=0;
		responseBuff[responseFrameLength++]	= (temp) | (temp1 << 2) | (temp2 << 4);			//B1
	}
	{
		temp = nns_get_charge_step();
    	stage_take_time= nns_get_step_time_min();	
		
		responseBuff[responseFrameLength++]	= temp << 4;			//B2
		responseBuff[responseFrameLength++]	= (uint8_t)(stage_take_time & 0xFF);			//B3	阶段时长低字节
		responseBuff[responseFrameLength++]	= (uint8_t)(stage_take_time >> 8);			//B4	阶段时长高字节
	}
	{
		uint32_t vol = (FW_RECOGNIZE >> 16);
		uint32_t ic = FW_RECOGNIZE >>8;
		vol &= 0xFFF;
		vol /= 10;
		ic &= 0xFF;
		responseBuff[responseFrameLength++]	= (uint8_t)vol;			//B5	额定电压
		responseBuff[responseFrameLength++]	= (uint8_t)ic;		//B6	额定电流
	}
	{
		uint32_t vol_real = adcGet(ADC_CHANNEL_BAT_VOLTAGE)*4/199;   //187;
		if(vol_real > 65535) {
			vol_real = 65535;
		}
		responseBuff[responseFrameLength++]	= (uint8_t)(vol_real & 0xFF)	;	//B7	电压低字节
		responseBuff[responseFrameLength++]	= (uint8_t)(vol_real >> 8)	;	//B8	电压高字节
	}
	{
	uint32_t cur_=0;
	cur_= icGet(adcGet(ADC_CHANNEL_CHARGING_CURRENT))+5000; // bias=5000
	responseBuff[responseFrameLength++]	=(uint8_t)(cur_ & 0xFF);		//B9	电流低字节
	responseBuff[responseFrameLength++]	=(uint8_t)(cur_ >> 8)	;	//B10	电流高字节
	}
	{
		int temper = ad2temperature(adcGet(ADC_CHANNEL_TEMP_ENV));
		responseBuff[responseFrameLength++]	= 	temper + 40	;//B11  实时环温
	}
//ad2temperature(temperatureENVADC.sum)
	{
		int temper = ad2temperature(adcGet(ADC_CHANNEL_TEMP_MOS));
		responseBuff[responseFrameLength++]	= temper + 40		;//B12   器件温度
	}
	responseBuff[responseFrameLength++] = 0xFF	;		//B13
	responseBuff[responseFrameLength++] = charger_state	;		//B14
	responseBuff[responseFrameLength++] = charging_control_bits;	//B15
	{
		int i;
		uint8_t checksum = 0;
		for(i = 2 ; i < 20 ; i++) {
			checksum += responseBuff[i];
		}
		responseBuff[responseFrameLength++] = checksum	;//checksum
	}
	responseBuff[responseFrameLength++] = 0x4B	;
	responseBuff[responseFrameLength++] = 0x4A	;
	sendFlag = 1;
}
/*
 * 解初始化报文
 * 填充初始应答报文
 * */
static void nns_Initial_message(uint8_t rx_info[]) {
	if(rx_info[9]) {
		nns_initial_response_fill();//主通信协议版本号=1
	}
}

/**
 * 解周期报文
 * 填充周期应答报文
 */
static void nns_Periodic_messages(uint8_t rx_info[]) {
	//uint16_t request_vol;
	{
	//FW_RECOGNIZE 是 “Firmware Recognition（固件识别字）” 的缩写，是一个打包在 32 位整数里的“充电器身份/额定参数”常量
		uint32_t voltage = FW_RECOGNIZE ; 
		voltage >>= 16;
		voltage &= 0xFFF;
		if(rx_info[0] * 10 == voltage) {
			charging_control_bits &= ~NNS_BMS_VOLTAGE_ERR;//电压相符
		} else {
			charging_control_bits |= NNS_BMS_VOLTAGE_ERR;//额定电压,电压不符,禁止充电
		}
	}
	if(rx_info[1]) {
		//额定容量
	}
	switch((rx_info[2] >> 6) & 3) {
	case 1: charging_control_bits &= ~NNS_BMS_HANDSHAKE_ERR;break;//握手成功
	default: charging_control_bits |= NNS_BMS_HANDSHAKE_ERR;break;//无握手,握手不成功,禁止充电
	}
		//盲充电0，智能充电1, function canceled.
  		//nns_enable_charge=(rx_info[3] & 3);

	
	//电池类型不符,禁止充电
	switch((rx_info[3] >> 2) & 3) {
	case 0:charging_control_bits &= ~NNS_BMS_BATTERY_TYPE_ERR;break;//铅酸电池
	default:charging_control_bits |= NNS_BMS_BATTERY_TYPE_ERR;break;
	}
	//已充满,要么不能进行充电,或者立即充电结束
	switch((rx_info[5]) & 3) {
	case 0:charging_control_bits &= ~NNS_BMS_FULL;break;//未充满
	case 1:charging_control_bits |= NNS_BMS_FULL;break;//已充满
	default:break;

	}
	//禁止充电,就类似过温保护处理,临时关闭充电管,等待禁止充电标志清除,如果是在待机状态,就一直维持在待机状态
	switch((rx_info[5] >> 2) & 3) {
	case 0:charging_control_bits &= ~NNS_BMS_FORBID_ERR;break;//允许充电
	case 1:charging_control_bits |= NNS_BMS_FORBID_ERR;break;//禁止充电
	default:break;
	}
	/*
	request_vol = rx_info[12];
	request_vol <<= 8;
	request_vol += rx_info[11];
	if(request_vol) {
//如果需求电压不能匹配充电器的电压,铅酸忽略
	}
	if(rx_info[13]) {  //请求电流,铅酸忽略

	}
	if(rx_info[14]) {  //SOC

	}
	*/
	//	if(rx_info[13]) {  //请求电流,铅酸忽略
    requested_current=rx_info[13];
			if(requested_current>RATED_CURRENT) requested_current=RATED_CURRENT;
	  requested_current=ADJ_CURRENTx4[requested_current];
//	}
	
	if(sendFlag ==0)
	nns_periodic_response_fill();
}


/************************************************************
* API Function Implementation
************************************************************/
/**
 * 这种错误不用等待,直接结束充电过程
 * @return
 */
beTrue_t nns_have_obj_errs(void) {
	return (charging_control_bits & (NNS_BMS_VOLTAGE_ERR  | NNS_BMS_BATTERY_TYPE_ERR )) > 0;
}
/*
 * 这种错误是充电过程中,暂时发生的,可以等待恢复
 */
beTrue_t nns_have_running_errs(void) {
	return (charging_control_bits & ( NNS_BMS_HANDSHAKE_ERR  | NNS_BMS_FORBID_ERR)) > 0;
}
/**
 * Whether the battery is full or not
 * @return
 */
beTrue_t nns_is_full(void) {
	return (charging_control_bits & NNS_BMS_FULL) > 0;
}
/**
 * 电池包是否存在
 * @return
 */
beTrue_t nns_is_battery_not_connected(void) {
	return (charging_control_bits & NNS_BMS_BATTERY_CONNECTED) ==0;
}

beTrue_t nns_is_battery_can_charge(void) {  // no protocal error ,or in blind mode with default-error.
//	if (charging_control_bits == NNS_BMS_BATTERY_CONNECTED || nns_blind_charge_allowed==1) return 1;
	if ((charging_control_bits & NNS_BMS_BATTERY_CONNECTED)>0 || nns_blind_charge_allowed==1) return 1;
	return 0;
}


beTrue_t nns_is_battery_in_error(void) {

//	if((charging_control_bits & (NNS_BMS_VOLTAGE_ERR | NNS_BMS_BATTERY_TYPE_ERR | NNS_BMS_FORBID_ERR) )>0) return 2;
	return 0;	
}

/*
beTrue_t nns_is_battery_in_error(void) {

	if((charging_control_bits & (NNS_BMS_VOLTAGE_ERR | NNS_BMS_BATTERY_TYPE_ERR | NNS_BMS_FORBID_ERR) )>0) return 2;
	if((charging_control_bits & NNS_BMS_HANDSHAKE_ERR)>0)
		if((charging_control_bits & NNS_BMS_BATTERY_CONNECTED)>0) return 1;  // not default error.	
	return 0;	
}
*/

/*
 * 数据入缓存
 */
 #define START2END 21
void nns_rx_data_put(uint8_t newData) {
	rx_tick_marker = systick_100us;  //① 记录本字节到达的时间戳

	if((nns_rx[nns_rx_idx]==0x4B)&& newData==0x4A)  //② 检测到帧尾 0x4B 0x4A
	{
		unsigned char i=nns_rx_idx-START2END;  //② 检测到帧尾 0x4B 0x4A
		unsigned char j=i+1;
		if((nns_rx[i]==0x59)&& (nns_rx[j]==0x44))   //④ 帧头匹配→确认真帧
		pack_pos=j+1; //⑤ 指向命令字(0xA8/0xA9)
		pack_flag=1;  //⑥ 通知解帧阶段：有帧就绪
	}		
	nns_rx_idx++;  //⑦ 写指针后移
	nns_rx[nns_rx_idx] = newData; //⑧ 存入新字节
}
/*
 * 解帧  新国标协议轮询的总管家
 */

 void nns_deframe_ex(void) {
	uint8_t rx_info[16]; 
	uint32_t spendTime = systick_100us - rx_tick_marker; 
	uint32_t spendTime1 = systick_100us - rx_tick_received_marker;
	static uint32_t rx_short_tick =0;
	static uint32_t bat_error_tick =0;
	uint32_t de=0;
	static uint8_t cnt=0; 
	//int32_t n = 16;
	uint8_t rx_idx=0;
	uint8_t err=0;	 
	//static uint8_t rx_base_idx = 0;
	int  i;
	
	err=nns_is_battery_in_error();
  if(err>0)	// 错误3秒红灯闪。
	{    //voltage and battery type error will report immediately.
		de=systick_100us-bat_error_tick;
		if(err >1 || de>3000*10) {   
			 LED_Set_State(RED_LED, LED_TWINKLE_1HZ);
       LED_Set_State(GREEN_LED, LED_OFF);
		}
	}
	else { bat_error_tick=systick_100us;}	
	
	//if(err>0){ //if(nns_enable_charge>0||err>0){   // 0 blind mode (canceled in new protocol) ,1 smart mode; blind mode 0不判断正常通讯超时。
	if(spendTime1 > 3000*10) {  // >3s 没收到合法帧
		charging_control_bits &= ~ NNS_BMS_BATTERY_CONNECTED;
			if(spendTime1 > 5000*10) requested_current=RATED_CURRENT*4;
		//has_communication=0;
		//reset_charging_control_bits();
	} 

	// short blind charged canceled? PA1 短路到地>3s,允许盲充。
	if(GPIO_Input_Pin_Data_Get(GPIOA , GPIO_PIN_1) == PIN_RESET)  // RX2 PA1 short > 3s, allow blind charge.
	{ cnt=0;
		de=systick_100us-rx_short_tick;
		if(de>3000*10) nns_blind_charge_allowed=1;
	}
	else { cnt++; 
	if (cnt>5) { rx_short_tick=systick_100us; nns_blind_charge_allowed=0;}  //软件滤毛刺
	}

	
	if(spendTime > 30*10) {
		//30ms无数据接收,说明主控在等答复或者无主控,这种情况也可能是收到了半个帧
		//  if(nns_rx_idx>0 ) 	uart2_send(nns_rx_idx);
		//  rx_base_idx = nns_rx_idx ;	
	}


	
		if(pack_flag == 0) {
			return;
		}
		pack_flag = 0;
		
		rx_idx=pack_pos;
		

{
			uint8_t id = nns_rx[rx_idx++];
			uint8_t length =  nns_rx[rx_idx++];
			uint8_t checksum;
			uint8_t cal_checksum = 0;
	
			if(length != 16) {
				return;
			}
			if(id == 0xA0 || id == 0xA1) {

			} else {
				return;
				//不是发给我的
			}
			cal_checksum += id;
			cal_checksum += length;
			for(i = 0 ; i < length ; i++	) {
				rx_info[i] = nns_rx[rx_idx++];
				cal_checksum += rx_info[i];
			}

			checksum =  nns_rx[rx_idx++];
			if(checksum == cal_checksum ) {
				if(id == 0xA0) {
					received_counter++;
					nns_Initial_message(rx_info);//根据id 执行不同的应答
					rx_tick_received_marker = systick_100us;
				} else if(id == 0xA1) {
					nns_Periodic_messages(rx_info);
					rx_tick_received_marker = systick_100us;
				}
				charging_control_bits |= NNS_BMS_BATTERY_CONNECTED;//有电池包连接
				//has_communication=1;
				//rx_base_idx = nns_rx_idx;
				return;
			} 
		}


}
 

/*
 * 延时发送应答帧
 */
void nns_Response_ex(void) {

	static uint8_t send_idx = 0;
	if(sendFlag == 2) {
			if(uart2_txOverFlag_get() == 0) {
				return;//发送寄存器空?
			}
			uart2_send(responseBuff[send_idx++]);
			if(send_idx >= responseFrameLength) {
				sendFlag = 0;
			}
			return;
	}
	if(sendFlag == 1) {
		uint32_t spend = systick_100us - rx_tick_marker;
		if(spend > 10*10) {
			sendFlag = 2;
			send_idx = 0;
		}
	}
}
void nns_charger_state_reset(uint32_t event) {
	charger_state &= ~event;
}
void nns_charger_state_all_reset(void) {
	charger_state = 0;
}
void nns_charger_state_set(uint32_t new_event) {
//	uint32_t clean_bits = NNS_CHARGER_STATE_CC | NNS_CHARGER_STATE_CV | NNS_CHARGER_STATE_TRI | NNS_CHARGER_STATE_FINISHED
//									| NNS_CHARGER_STATE_D102_OTP	| NNS_CHARGER_STATE_ENV_OTP | NNS_CHARGER_STATE_FAN_ERR;
uint32_t clean_bits = NNS_CHARGER_STATE_CC | NNS_CHARGER_STATE_CV | NNS_CHARGER_STATE_TRI | NNS_CHARGER_STATE_FINISHED;
	if(new_event & clean_bits) {
		charger_state &= ~clean_bits;
    }
//	if(new_event & NNS_CHARGER_STATE_FINISHED) {
//		charger_state &= ~clean_bits;//清除环境过温,D102过温,风扇堵转,横流,恒压 , 涓流
//	}
//	if(new_event & NNS_CHARGER_STATE_CC) {
//		charger_state &= ~clean_bits;//清除环境过温 D102过温,风扇堵转,恒压,涓流,充电完成
//	}
//	if(new_event & NNS_CHARGER_STATE_CV) {
//		charger_state &= ~clean_bits;
//	}
//	if(new_event & NNS_CHARGER_STATE_TRI) {
//		charger_state &= ~clean_bits;
//	}
	charger_state |= new_event;
//	if(new_event & NNS_CHARGER_STATE_D102_OTP) {
//
//	}
//	if(new_event & NNS_CHARGER_STATE_ENV_DERATING) {
//
//	}
//	if(new_event & NNS_CHARGER_STATE_ENV_OTP) {
//
//	}
//	if(new_event & NNS_CHARGER_STATE_FAN_ERR) {
//
//	}
//	if(new_event & NNS_CHARGER_STATE_NTC_OPEN) {
//
//	}
//	if(new_event & NNS_CHARGER_STATE_NTC_SHORT) {
//
//	}
}
void nns_tx_test(void) {
	if(uart2_txOverFlag_get() == 0) {
		return;//发送寄存器空?
	}
	uart2_send(0x5A);
}
/************************************************************
* Internal Function Implementation
************************************************************/





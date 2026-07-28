/***
 *
 */
#define _MAIN_C_
#include "includeAll.h"
void sendBuff(void);
void writeFlash(void);

/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
/***
 * 5组AD是
 * 充电电流
 * 充电电压
 * 环境温度
 * mos温度
 * 风扇电流
 */
unsigned char can_tx_flag=0;

typedef struct {
	uint8_t cnt;
	uint16_t adc16[16];
	uint32_t sum;
}ADCType_T;

static ADCType_T chargingCurrentADC = {0 , {0 } , 0};
static ADCType_T chargingVoltageADC = {0 , {0 } , 0};
static ADCType_T temperatureMOSADC = {0 , {0 } , 0};
static ADCType_T temperatureENVADC = {0 , {0 } , 0};
static ADCType_T fanCurrentADC = {0 , {0 } , 0};
static ADCType_T chargingBatADC = {0 , {0 } , 0};

//static  ADCType_T *const adcChannelSet[] = {&fanCurrentADC , &chargingCurrentADC , &chargingVoltageADC , &temperatureMOSADC , &temperatureENVADC};
static  ADCType_T *const adcChannelSet[] = {&chargingBatADC, &chargingCurrentADC , &chargingVoltageADC , &temperatureMOSADC , &temperatureENVADC, &fanCurrentADC};
#ifdef FAN_BLOCK_WAY2
 uint32_t fanCurBuff[8] , fanCurBuff1[8];
static uint8_t fanCurBuffIdx = 0;
 uint32_t lostTm = 0;
#endif
static uint32_t acc = 0;
/***
 * channelSel   :  ADC_CHANNEL_CHARGING_CURRENT
 * 						ADC_CHANNEL_CHARGING_VOLTAGE
 * 						ADC_CHANNEL_TEMP_MOS
 * 						ADC_CHANNEL_TEMP_ENV
 * 						ADC_CHANNEL_FAN_CURRENT
 *
 */
/*
uint32_t voGet(uint32_t vo_ad) {
	uint32_t temp;
	temp = 2046 * vo_ad;
	temp = temp / 10000;
	return temp;
}
*/

uint32_t voGet(uint32_t vo_ad) {
	uint32_t temp;
	temp = 100 * vo_ad;
	temp = vo_ad / 103;
	return temp;
}

uint32_t icGet(uint32_t ic_ad) {
	uint32_t temp;
	if(ic_ad<=ADV_CURRENT_OFFSET)
		return 0;
	temp = ic_ad- ADV_CURRENT_OFFSET;
	temp = temp/1024;
	return temp;
}
uint32_t adcGet(uint32_t  channelSel ) {
	uint32_t realChannel = channelSel + 1;
	if(realChannel >= ADC_CHANNEL_QTY) {
		realChannel = 0;
	}
	return adcChannelSet[realChannel]->sum;
}
static void adcSaveIn(ADCType_T * pt , uint32_t inputDat) {
	int i = 0;
	int x = pt->cnt / 16;
	int y = pt->cnt % 16;
	pt->cnt++;
	if(y == 0) {
		pt->adc16[x] = 0;
		pt->sum = 0;
		for(i = 0 ; i < 16 ; i++) {
			pt->sum += pt->adc16[i];
		}
	}
	pt->adc16[x] += inputDat;

#ifdef FAN_BLOCK_WAY2
	if(pt == &fanCurrentADC) {
		if(inputDat > 255) {
			inputDat = 255;
		}
		fanDatPut((uint8_t*)fanCurBuff , inputDat);
		if(fanCurBuffIdx < 31) {
			fanCurBuffIdx++;
		} else {
			fanCurBuffIdx = 0;
		}
//		((uint8_t*)fanCurBuff)[fanCurBuffIdx] = inputDat;
	}
#endif
}
/***
 * 0----选择充电电流检测通道
 * 1----选择充电电压检测通道
 * 2----MOS管温度检测通道
 * 3----环境温度检测通道
 * 4----风扇电流检测通道
 */
void adcRunning2(void) {
	uint32_t substep = acc % 2;
	uint32_t buffSel = acc / 2;
	uint32_t newDat = 0;
	if(substep == 0) {
		if(adc_overFlag() == 0) {//读一下ADC转换完成标记,正常情况下，ADC肯定是完成了
			__nop();
			return ;
		}
		newDat = adc_get() ; //这是上一个ADC,
		adc_channel_select(buffSel);//切换下一个通道
		adcSaveIn(adcChannelSet[buffSel] , newDat);//把数据保存好

	} else{
		adc_overFlagClear();
		adc_Start();//启动ADC转换
	}


	acc++;
	acc = acc % (ADC_CHANNEL_QTY*2);
}

void adcRunning(void) {
	uint32_t substep = acc % 2;
	uint32_t buffSel = acc / 2;
	uint32_t newDat = 0;
	if(substep == 0) {
	//	if(adc_overFlag() == 0) {//读一下ADC转换完成标记,正常情况下，ADC肯定是完成了
	//		__nop();
	//		return ;
	//	}
	//	adc_overFlagClear();
		newDat = adc_get() ; //这是上一个ADC,
		adcSaveIn(adcChannelSet[buffSel] , newDat);//把数据保存好
		adc_channel_select(buffSel);//切换下一个通道
		adc_overFlagClear();
		adc_Start();//启动ADC转换
	} else{
		
	}


	acc++;
	acc = acc % (ADC_CHANNEL_QTY*2);
}
/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
uint8_t calibrationInit(void);
volatile uint32_t systick = 0 ;
extern volatile uint32_t systick_100us;
uint32_t maintick = 0;

static uint16_t enterCalibrationTimeslot = 0;
static uint16_t calibrationCountDown = 0;
void enterCalibration(void) {
	if(enterCalibrationTimeslot < ONE_SECOND * 60) {
		calibrationCountDown += 100;
		if(calibrationCountDown > 300) {
			calibrationCountDown = 300;
		}
	}
}

extern unsigned char fflag;

int main(void) {
uint8_t errFlag = 0;
static uint8_t swd_delay_init_counter = 0;
	//初始化环境
	initAll();

  //setLvr();
	setPVD();
	errFlag = calibrationInit();//读校准值
	chargeState_Init();
  init_ACinput( ); 

	//IWDG_Config(IWDG_CONFIG_PRESCALER_DIV8, 0xFF0);
	while(1) {
		static uint16_t timeCnt = 1000;
		int i;
		uint8_t sendBuff[64];
		lcdtest();
		if(maintick != systick) {
			maintick++;
			IWDG_Feed();
			led_ctr_exec();//指示灯管理
			if(maintick == 33 && swd_delay_init_counter == 0) {   //330ms后初始化UART2 SWD
				//swd_delay_init();
				swd_delay_init_counter++;
			}

			uart_deframeRunning();


			if(enterCalibrationTimeslot < ONE_SECOND * 100) {
				enterCalibrationTimeslot++;
			}

			if(calibrationCountDown) {
				if(calibrationCountDown <= 100) {
					calibrationCountDown--;
				} else {
					calibrationRunning();//执行校准函数
				}
				continue;
			}

	//debug info
			
			if(timeCnt % 50 == 0) {
				extern uint32_t calibrationPWM[];//发送参数，
				static uint32_t runTime = 0;
				if(timeCnt / 50 == 0) {//0.5s
#ifndef FAN_CUR_TABLE
					uint32_t vi = adcGet(ADC_CHANNEL_CHARGING_CURRENT)/355;
					snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "电流AD：%-6u  ", vi);
#else
					snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "%5d %5d %5d %5d", ((uint8_t*)fanCurBuff1)[0] , ((uint8_t*)fanCurBuff1)[1] , ((uint8_t*)fanCurBuff1)[2] , ((uint8_t*)fanCurBuff1)[3]);
#endif
				}	else if(timeCnt / 50 == 1) {//0.5s
#ifndef FAN_CUR_TABLE
					uint32_t vc = adcGet(ADC_CHANNEL_CHARGING_VOLTAGE)/737;
		      uint32_t vb = adcGet(ADC_CHANNEL_BAT_VOLTAGE)/737;
					snprintf((char*)sendBuff , sizeof(sendBuff) - 2 , "电压AD：%-6u BAT %-6u ", vc, vb ); //voGet(chargingVoltageADC.sum)
#else
					snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "%5d %5d %5d %5d", ((uint8_t*)fanCurBuff1)[4] , ((uint8_t*)fanCurBuff1)[5] , ((uint8_t*)fanCurBuff1)[6] , ((uint8_t*)fanCurBuff1)[7]);
#endif
				}	else if(timeCnt / 50 == 2) {
#ifndef FAN_CUR_TABLE
					snprintf((char*)sendBuff , sizeof(sendBuff) - 2 , "PVD标志位：%-1u  ", PWR_Flag_Status_Get(PWR_PVD_OUTPUT_FLAG));
#else
					snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "%5d %5d %5d %5d", ((uint8_t*)fanCurBuff1)[8] , ((uint8_t*)fanCurBuff1)[9] , ((uint8_t*)fanCurBuff1)[10] , ((uint8_t*)fanCurBuff1)[11]);
#endif
				} else if(timeCnt / 50 == 3) {
#ifndef FAN_CUR_TABLE
					snprintf((char*)sendBuff , sizeof(sendBuff) - 2 , "运行计数:%-6u  ", runTime++%(uint32_t)(30*24*3600/4.5) );
#else
					snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "%5d %5d %5d %5d", ((uint8_t*)fanCurBuff1)[12] , ((uint8_t*)fanCurBuff1)[13] , ((uint8_t*)fanCurBuff1)[14] , ((uint8_t*)fanCurBuff1)[15]);
#endif
				} else if(timeCnt / 50 == 4){
#ifndef FAN_CUR_TABLE
					snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "风扇电流AD：%-6u  ", fanCurrentADC.sum);
#else
					snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "%5d %5d %5d %5d", ((uint8_t*)fanCurBuff1)[16] , ((uint8_t*)fanCurBuff1)[17] , ((uint8_t*)fanCurBuff1)[18] , ((uint8_t*)fanCurBuff1)[19]);
#endif
				} else if(timeCnt / 50 == 5){
#ifndef FAN_CUR_TABLE
					snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "器件温度：%-5d  ", ad2temperature(temperatureMOSADC.sum));
#else
					snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "%5d %5d %5d %5d", ((uint8_t*)fanCurBuff1)[20] , ((uint8_t*)fanCurBuff1)[21] , ((uint8_t*)fanCurBuff1)[22] , ((uint8_t*)fanCurBuff1)[23]);
#endif
				} else if(timeCnt / 50 == 6){
#ifndef FAN_CUR_TABLE
					snprintf((char*)sendBuff , sizeof(sendBuff) - 2 , "环境温度：%-5d  ", ad2temperature(temperatureENVADC.sum));
#else
					snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "%5d %5d %5d %5d", ((uint8_t*)fanCurBuff1)[24] , ((uint8_t*)fanCurBuff1)[25] , ((uint8_t*)fanCurBuff1)[26] , ((uint8_t*)fanCurBuff1)[27]);
#endif
				} else if(timeCnt / 50 == 7){
					extern uint32_t fanCurOffLoad;
#ifndef FAN_CUR_TABLE
					snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "风扇空载AD:%-6u  ", fanCurOffLoad);
#else
					snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "%5d %5d %5d %5d	 %7u ", ((uint8_t*)fanCurBuff1)[28] , ((uint8_t*)fanCurBuff1)[29] , ((uint8_t*)fanCurBuff1)[30] , ((uint8_t*)fanCurBuff1)[31] , lostTm);
#endif

				} else if(timeCnt / 50 == 8){
					extern ChargingStepType_T chargeSubprocessNow;
					extern MainStepType_T mainStepNow;
					if(mainStepNow == MAIN_STEP_WAITING_BATPACK) {
						 snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "待机\r\n");
					} else if(mainStepNow == MAIN_STEP_WAITING_TAKE_AWAY) {
						 snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "等移除电池包\r\n");
					} else {
					switch(chargeSubprocessNow) {
					     case    CHARGING_STEP_PRE     :
					    	 snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "预充电\r\n");
					    	 break;
					     case    CHARGING_STEP_CCM     :
					    	 snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "恒流\r\n");
					    	 break;
					     case    CHARGING_STEP_CVM     :
					    	 snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "恒压\r\n");
					    	 break;
					     case    CHARGING_STEP_TRI     :
					    	 snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "涓流\r\n");
					    	 break;
					     case    CHARGING_STEP_FINISHED     :
					    	 snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "充电完成\r\n");
					    	 break;
					     case     CHARGING_STEP_INVALID    :

//					    	 break;
					     default       :
						 snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "存在错误状态\r\n");
						  break;
					}
					}
				} else if(timeCnt / 50 == 20) {
					snprintf((char*)sendBuff ,sizeof(sendBuff) - 2 ,  "第一次上电\r\n");
					//nns_tx_test();
					nns_initial_response_fill();
				}
				else if(timeCnt / 50 == 9) {
					uint32_t temp= PWM_get(PWM_CHANNEL_VOLTAGE);  //(ADC_CHANNEL_BAT_VOLTAGE);//ADC_GetData(4);   //adcGet(ADC_CHANNEL_BAT_VOLTAGE) nns_is_battery_can_charge() get_charging_control_bits() 
					snprintf((char*)sendBuff , sizeof(sendBuff) - 2 , "Vx10：%d Ix10 %d ", adcGet(ADC_CHANNEL_BAT_VOLTAGE)/737,adcGet(ADC_CHANNEL_CHARGING_CURRENT)/355);
				}
				
				uart_emptyTxBuff();
				for(i = 0 ; i < 64 ; i++) {
					uart_fillTXBuff(&sendBuff[i] , 1);
					if(sendBuff[i] == '\0') {
						uart_fillTXBuff(&sendBuff[i] , 1);
						break;
					}
				}
			}

			timeCnt ++;
			if(timeCnt % 50 == 0) {
				if(timeCnt >= 50*10) {
					timeCnt = 0;
				}
			}
			//======================END========================


			chargeState_Running(errFlag);//充电管理

		} else {
	//	static uint8_t fanCurBuffIdxBak = 0;
			nns_Response_ex();  // delay ms and response.
			uart_sendBuffExec();//通讯管理
			nns_deframe_ex();
			//nns_tx_test();
      //can_test();
			can_tx_rx();
		//	can_delay_send();
			
			/*
			if(fanCurBuffIdx != fanCurBuffIdxBak) {
				uint32_t temp;
				fanCurBuffIdxBak  = fanCurBuffIdx;
				temp = fanBlockOutExe((uint8_t *)fanCurBuff , 32);
			}
			*/

		}

	}

}



































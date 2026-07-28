
#include "includeAll.h"
unsigned char fflag=0;
 MainStepType_T mainStepNow = MAIN_STEP_WAITING_BATPACK, mainStepLast = MAIN_STEP_INVALID;
/***
 * 要在主状态控制函数执行前，先执行一次初始化函数
 * 确保主状态控制函数正确执行
 */
void chargeState_Init(void) {
	mainStepNow = MAIN_STEP_WAITING_BATPACK ;
	mainStepLast = MAIN_STEP_INVALID;
}
/***
 *充电器没有插电池包时，处于待机状态
 *插上电池包就进入充电，充满电或者拔掉电池包，都会回到待机状态
 *充满电后，是需要关闭可控硅输出的。不能让电池包一直通电。
 */
MainStepType_T chargeState_Running(uint32_t flags) {
static uint8_t wait600ms = 0;
int n = 16;
nns_charger_state_all_reset();
	while(n-- > 0) {
	/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
	if(mainStepNow == MAIN_STEP_WAITING_BATPACK) {//等待插上电池包（NTC电压0V 表示电池包取走）
		static uint8_t batPackPlugCnt; //电池包插入计数器	
		static uint32_t floatChargingPwm ; //浮充电压PWM值
		uint32_t ratedCurrentPwm; //电流PWM值
		uint32_t at25 = calibrationPWM_get(CALIBRATION_PWM_VOLTAGE_AT_25T_IDX0); //25度时，电压PWM值
		uint32_t at45= calibrationPWM_get(CALIBRATION_PWM_VOLTAGE_AT_45T_IDX1); //45度时，电压PWM值
		uint32_t atN10 = calibrationPWM_get(CALIBRATION_PWM_VOLTAGE_AT_N10T_IDX2); //-10度时，电压PWM值
		if(mainStepNow != mainStepLast) { //如果当前状态和上一次状态不一样，则初始化计数器	
			batPackPlugCnt = 0; //电池包插入计数器
			mainStepLast = mainStepNow;
//			wait600ms = 0;
			{
				floatChargingPwm = calibrationPWM_get(CALIBRATION_PWM_VOLTAGE_FLOAT_IDX4); //浮充电压PWM值
				ratedCurrentPwm = calibrationPWM_get(CALIBRATION_PWM_CURRENT_RATED_IDX3); //电流PWM值	
				PWM_set(PWM_CHANNEL_VOLTAGE , floatChargingPwm );
				PWM_set(PWM_CHANNEL_CURRENT , ratedCurrentPwm / 10);
			} 
			iOPinSet_SCR_off(); //关闭可控硅
			togglePeriod();//TIM3 的“高频/慢频”两档间切换，用于在进入待机态时改变系统整体节拍。
		}
		//if(wait600ms < 27) {
		//	PWM_set(PWM_CHANNEL_VOLTAGE , floatChargingPwm - floatChargingPwm *3 / wait600ms);
		//} else {
		//	PWM_set(PWM_CHANNEL_VOLTAGE , floatChargingPwm );
		//}
		wait600ms++;
		if(wait600ms < 30) {
			return mainStepNow;
		}
		wait600ms--;
		red_led = LED_OFF;
		green_led = LED_TWINKLE_1HZ;//LED_ON;
		//一个pwm值
		//板子没有经过校准，则不能往下执行
		if(flags != 0) {
//			red_led = LED_TWINKLE_2HZ;
//			green_led = LED_OFF;·
		}
fflag=0;
		//如果环境温度范围不对，则不往下
		if( isNtcFault( adcGet(ADC_CHANNEL_TEMP_MOS)) ) {
			green_led= LED_OFF;
			red_led = LED_TWINKLE_2HZ;
			nns_charger_state_set(NNS_CHARGER_STATE_NTC_OPEN);//自检不过,上报充电器异常
fflag|=1;
			return mainStepNow ; //
		} else {
			nns_charger_state_reset(NNS_CHARGER_STATE_NTC_OPEN);//
		}
		if(at45 > at25 || at25 > atN10) {
			red_led = LED_TWINKLE_1HZ;
			green_led = LED_OFF;
		}
		//如果输入电压范围不对，则不往下
		/*
		{
		uint32_t adc = adcGet(ADC_CHANNEL_CHARGING_VOLTAGE);
		uint32_t ac = 80*adc/1000;
			ac+=1500;   // no load compensation.
    if(isOk_ACinput(ac) == 0){
			red_led = LED_TWINKLE_1HZ;
			green_led = LED_OFF;
			return mainStepNow ;// 
		}
		}
		//红灯
		*/
		if(PWR_Flag_Status_Get(PWR_PVD_OUTPUT_FLAG) == SET) {
			nns_charger_state_set(NNS_CHARGER_STATE_5V_FAULT); //自检不过,上报充电器异常
fflag|=2; //置位诊断位
			return mainStepNow;
		}
			nns_charger_state_reset(NNS_CHARGER_STATE_5V_FAULT); 
		//电池包存在
		//没有任何错误,握手成功
		//不是充满状态
		//进行充电  nns_is_battery_can_charge isBatPackTakeAway
		//if(isBatPackNotReady()||nns_is_full()) {
		if(nns_is_battery_in_error()>0){ 
			//red_led =  LED_TWINKLE_1HZ;
		  //green_led = LED_OFF;
			//batPackPlugCnt = 0;
			//return mainStepNow ;//无电池包 则返回  
			mainStepNow = MAIN_STEP_WAITING_TAKE_AWAY;
			continue;
		}
		else if(isBatPackTakeAway()) { //	
			red_led = LED_OFF;
		  green_led = LED_TWINKLE_1HZ;//LED_ON;
			batPackPlugCnt = 0;
fflag|=4;
			return mainStepNow ;//无电池包 则返回  
		}
		else if(nns_is_full()){
		  //red_led = LED_OFF;
		  //green_led = LED_ON;//LED_ON;
			//batPackPlugCnt = 0;
			//nns_charger_state_set(NNS_CHARGER_STATE_FINISHED);
			//return mainStepNow ;//无电池包 则返回  
			mainStepNow = MAIN_STEP_WAITING_TAKE_AWAY;
			continue;
		}
    else if(isBatPackVolOK()){
			batPackPlugCnt++;
			if(batPackPlugCnt < 3) {
				return mainStepNow ;
			} else {
				mainStepNow = MAIN_STEP_CHARGING;//有电池包，则充电
				continue;
			}
		}
fflag|=8;		
	}
	/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

	/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
	if(mainStepNow == MAIN_STEP_CHARGING) {//充电过程控制

//		static uint16_t removeDly = 0;
		static uint8_t batPackRemoveCnt = 0;
		static uint8_t pdTm = 0;
		ChargingStepType_T mode ,chargeingStep = CHARGING_STEP_PRE;
		uint32_t at25 = calibrationPWM_get(CALIBRATION_PWM_VOLTAGE_AT_25T_IDX0);
		uint32_t at45= calibrationPWM_get(CALIBRATION_PWM_VOLTAGE_AT_45T_IDX1);
		uint32_t atN10 = calibrationPWM_get(CALIBRATION_PWM_VOLTAGE_AT_N10T_IDX2);
		if(mainStepNow != mainStepLast) {
			mainStepLast = mainStepNow;
			batPackRemoveCnt = 0;
	//		removeDly = 0;
			pdTm = 0;
			chargeingStep = CHARGING_STEP_PRE;
			chargingStep_Init();//初始化子过程
		}
		if( PWR_Flag_Status_Get(PWR_PVD_OUTPUT_FLAG) == SET) {
			pdTm++;//如果掉到4.8V，并且达到0.5s以上，直接回到MAIN_STEP_WAITING_BATPACK
			if(pdTm > 50){//电压跌落0.5s，认为是AC掉电
				pdTm--;
				mainStepNow = MAIN_STEP_WAITING_BATPACK;
				nns_charger_state_set(NNS_CHARGER_STATE_5V_FAULT);
				continue;
			}
		} else {
			pdTm = 0;
			nns_charger_state_reset(NNS_CHARGER_STATE_5V_FAULT);
		}

		mode = chargeingStep_Running();
		chargeingStep = (ChargingStepType_T)((uint8_t)mode & (uint8_t)(~ERROR_FLAG_BITS));
		if(SCROffStep_needLowPower() == 0) {
			//不是在关断可控硅的动作序列里，可以安全亮灯,可控硅关断动作里，要尽量省电，所以要关灯。
			//如果是涓流，红灯灭，绿灯亮
			//如果是横流和恒压，红灯亮，绿灯灭
				if(chargeingStep == CHARGING_STEP_TRI || chargeingStep == CHARGING_STEP_FINISHED) {//当前在涓流充电阶段|| chargeingStep == CHARGING_STEP_PRE
					green_led = LED_ON;
					red_led = LED_OFF;
				}  else {
					red_led = LED_ON;
					green_led = LED_OFF;
				}

				if(mode & ERROR_FLAG_OVER_TEMP) {
					red_led = LED_TWINKLE_2HZ;
					green_led = LED_OFF;
				} else if(mode & ERROR_FLAG_FAN_ABNORMAL) {
					red_led = LED_TWINKLE_1HZ;
					green_led = LED_OFF;
				}
				else {

				}
		} else {

		}
		if( isNtcFault (adcGet(ADC_CHANNEL_TEMP_MOS)) ) {
			green_led= LED_OFF;
			red_led = LED_TWINKLE_2HZ;
			mainStepNow = MAIN_STEP_SILICON_OFF;
			nns_charger_state_set(NNS_CHARGER_STATE_NTC_OPEN);
			continue;
		} else {
			nns_charger_state_reset(NNS_CHARGER_STATE_NTC_OPEN);
		}
		if(at45 > at25 || at25 > atN10) {
			red_led = LED_TWINKLE_1HZ;
			green_led = LED_OFF;
		}
		if(chargeingStep == CHARGING_STEP_FINISHED) {
			mainStepNow = MAIN_STEP_SILICON_OFF;//电池包充电完成，则到关断可控硅状态
			continue;
		} else {

		}

		//如果电池包没有连接
		//如果已经满了
		//就去断电
		//电池包取走？到关断可控硅状态

		
				if(isBatPackNotOK()) {
				batPackRemoveCnt++;
				if(batPackRemoveCnt > 2) {
					mainStepNow = MAIN_STEP_SILICON_OFF;
					continue;
				}
			} else {
				batPackRemoveCnt = 0;
			}	
		
	}
	/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

	/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
	if(mainStepNow == MAIN_STEP_SILICON_OFF) {
		if(mainStepNow != mainStepLast) {
			mainStepLast = mainStepNow;
			SCROffStepInit();//执行关可控硅的函数初始化
		}
	  //关可控硅程序执行完成？则到等待电池包移除状态
		if(SCROffStepProcess() != SCR_OFF_STEP_FINISHED) {
			return mainStepNow;
		} else {
			mainStepNow = MAIN_STEP_WAITING_TAKE_AWAY;
			continue;
		}
	}
	/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

	/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
	if(mainStepNow == MAIN_STEP_WAITING_TAKE_AWAY) {
		static uint8_t batpackRemoveDelay = 0;
		if(mainStepNow != mainStepLast) {
			batpackRemoveDelay = 0;
			mainStepLast = mainStepNow;
		}
		//电池包移除，则到等待电池包放置状态
		//红灯灭，绿灯亮
		nns_charger_state_set(NNS_CHARGER_STATE_FINISHED);
		green_led = LED_ON;
		red_led = LED_OFF;
		if( isNtcFault (adcGet(ADC_CHANNEL_TEMP_MOS)) ) {
			green_led= LED_OFF;
			red_led = LED_TWINKLE_2HZ;
			nns_charger_state_set(NNS_CHARGER_STATE_NTC_OPEN);
		} else {
			nns_charger_state_reset(NNS_CHARGER_STATE_NTC_OPEN);
		}
		
		if(nns_is_battery_in_error()>0){
			red_led =  LED_TWINKLE_1HZ;
		    green_led = LED_OFF;
		}
		
		
#if 0
		if(nns_is_battery_connected()) {

		} else {
			mainStepNow = MAIN_STEP_WAITING_BATPACK;
			continue;
		}
#endif
		if(isBatPackTakeAway()) {
			batpackRemoveDelay++;
			if(batpackRemoveDelay > 3) {
				//reset_charging_control_bits();
				mainStepNow = MAIN_STEP_WAITING_BATPACK;
				continue;
			}
		} else {
			batpackRemoveDelay = 0;
		}
	}
	/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

	/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
	if(mainStepNow == MAIN_STEP_WARING) {
		if(mainStepNow != mainStepLast) {
			mainStepLast = mainStepNow;
		}
		//关闭输出
		//红灯闪烁
		mainStepNow = MAIN_STEP_WAITING_BATPACK;//
		continue;
	}
	/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

	/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
	if(mainStepNow >= MAIN_STEP_INVALID){
		if(mainStepNow != mainStepLast) {
			mainStepLast = mainStepNow;
		}
		//异常
		mainStepNow = MAIN_STEP_WAITING_BATPACK;
		continue;
	}

//	mainStepLast =  ;

	return mainStepNow;
	}
	return mainStepNow;
}

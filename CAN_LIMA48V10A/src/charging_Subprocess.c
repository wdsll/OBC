
#include "includeAll.h"

/***
 * 对于充电电流电压的控制，
 * 包含了温度补偿
 * 温度保护
 * 风扇异常保护
 */
extern const uint8_t RATED_CURRENT;
uint16_t requested_current=MAX_LIMITED_CURRENT*4;
uint16_t requested_voltage=MAX_LIMITED_VOLTAGE;
static uint32_t triTmLimit = 0; 
static uint32_t ccTmLimit = 0;
static uint32_t cvTmLimit = 0; 
static uint32_t iniTmLimit =0;

static uint32_t limitedPWM=999; 
//static uint8_t over_turn = 0; 
uint8_t crosscvFlag = 0;
 ChargingStepType_T chargeSubprocessNow = CHARGING_STEP_PRE , chargeSubprocessLast = CHARGING_STEP_INVALID ;
 ChargingStepType_T chargingStep_get(void) {
	 return chargeSubprocessNow;
 }
void chargeingStep_Set(ChargingStepType_T newstate) {
	chargeSubprocessNow = newstate;
}
void chargingStep_Init(void) {
	chargeSubprocessNow = CHARGING_STEP_PRE;
	chargeSubprocessLast = CHARGING_STEP_INVALID ; 
	ccTmLimit = 0;
	cvTmLimit = 0;
	triTmLimit = 0;
}


//extern uint8_t Flag_DrateCC1A_AC;
//AC 异常恢复后置 1；AC 欠压进入 1A 限流置 1；AC 恢复后置 0
uint8_t Flag_DrateCC1A_AC=0;
//AC 超出 150V~285V 范围时置 1（触发关可控硅）
uint8_t acErrFlag = 0;
//	AC 异常时递增到 100 才翻转 acErrFlag（延时确认）
static uint8_t prtDelay = 0; 
//C 输入检测状态复位
void init_ACinput(void) {
	Flag_DrateCC1A_AC = 0;
	acErrFlag = 0;
	prtDelay = 0; 
}
//AC 输入电压检测与保护状态机
/**
 * 这是充电器交流输入端的"看门狗"。它接收 ADC 换算后的 AC 电压值 ac，判断电网电压是否正常，驱动两件事：
	1. 如果电网电压异常，则关闭可控硅，并进入宕机状态。
		严重越限 → acErrFlag=1 → 关可控硅 → 保护充电器
		轻度欠压 → Flag_DrateCC1A_AC=1 → 限流 1A → 降额运行
	2. 如果电网电压正常，则打开可控硅，并进入正常状态。
 */
uint8_t isOk_ACinput(uint32_t ac) {
	static uint8_t upDelay = 0; 
	static uint8_t dwDelay = 0; 
BEGIN:	
	if(acErrFlag) {
		if(SCROffStepProcess() != SCR_OFF_STEP_FINISHED) { 
			return 0; // 可控硅还在关断中，不能往下走
		}
		if(ac < (MIN_LOW_VOLTAGE_RECOVERY) || ac > (MAX_HIGH_VOLTAGE_RECOVERY)) {       //165V欠压保护恢复宕机-降额，过压保护宕机至正常
			//恢复确认用了 prtDelay 递减去抖
			if(prtDelay < 100) {
				prtDelay++;
			}
			return 0; //继续等待
		} else {
			if(prtDelay > 0) {
				prtDelay--;
				return 0;
			} else {
//Moe
				Flag_DrateCC1A_AC = 1; //异常恢复后置1
//Moe
				acErrFlag = 0;
				return 1;
			}
		}
	}
	if(ac < MIN_LOW_VOLTAGE || ac > MAX_HIGH_VOLTAGE) {     //150V欠压保护宕机，过压保护宕机设置
		prtDelay++;
		if(prtDelay > 100) {
			acErrFlag = 1;
			SCROffStepInit();//执行关可控硅的函数初始化
			goto BEGIN;
		}
	} else {
//Moe
		if(Flag_DrateCC1A_AC==0)
		{ 
			if(ac<ACVEnter1A)  //AC 欠压进入 1A 限流
			{
				dwDelay++;
				if(dwDelay>50) 
				Flag_DrateCC1A_AC = 1;
			}
			else{dwDelay=0; }
			upDelay=0;
		}
		else
		{
			if(ac>ACVExit1A) //	AC 恢复后置 0
			{
				upDelay++;
				if(upDelay>50) 
				Flag_DrateCC1A_AC = 0;
			}
			else{upDelay=0; }
			dwDelay=0;
		}
//Moe
		if(prtDelay > 0) {
			prtDelay--;
		}
	}
	return 1;
}



/***
 * 充电的执行程序，
 */
ChargingStepType_T chargeingStep_Running(void) { // 充电子进程主状态机入口：按 chargeSubprocessNow 执行对应阶段，返回当前阶段


	static uint16_t preChargingTime = 0; // PRE阶段计时器(每拍+1，约10ms/拍)；static 跨调用保留
	int n = 16; // 单次调用最多处理16次状态转移，防止 while 内无限 continue 卡死
	while(n-- > 0) { // 状态机主循环：允许同一次调用内连续切换阶段
/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/			 // 分隔标注：PRE 阶段开始											
	if(chargeSubprocessNow == CHARGING_STEP_PRE) {    // ===== PRE 预充/自检阶段 =====
	  uint32_t afterCompensatingPWM;  // PRE阶段电压 PWM 临时变量
		uint32_t floatChargingPwm;      // 浮充电压 PWM 临时变量
//		uint32_t ratedCurrenChargingPWM; // (弃用)额定电流PWM
		uint32_t currentAd ; // 充电电流采样值临时变量
		if(chargeSubprocessNow != chargeSubprocessLast) { // 边沿触发：仅"刚进入PRE"这一拍做一次性初始化
			chargeSubprocessLast = chargeSubprocessNow; 	 // 更新"上次状态"，使本段下次不再执行
			crosscvFlag = 0; // 清交叉阶段标志
			floatChargingPwm = calibrationPWM_get(CALIBRATION_PWM_VOLTAGE_FLOAT_IDX4); // 浮充电压PWM值			
			PWM_set(PWM_CHANNEL_VOLTAGE , floatChargingPwm); // 先输出浮充电压(低压)
				

			PWM_set(PWM_CHANNEL_CURRENT , PWM_CURRENT_0o5_RATED);  // 电流PWM设半额定(安全试探电流)
			preChargingTime = 0; // PRE计时清零
			requested_current=RATED_CURRENT*4; // 请求电流恢复为额定(×4缩放)
		}
		ccTmLimit = 0;  //恒流超时判断
		cvTmLimit = 0; //恒压超时判断
		triTmLimit = 0; //涓流充电超时判断
		iniTmLimit = 0; //初始阶段超时

		
		nns_charger_state_set(NNS_CHARGER_STATE_SCR);  // 设置充电器状态为SCR，可控硅正在导通，充电回路正在输出功率。
		nns_charger_state_set(NNS_CHARGER_STATE_CC); // 设置充电器状态为CC



		iOPinSet_FanOn(); //打开风扇
 		iOPinSet_SCR_on();//打开可控硅
		// 有收报文0.8秒按请示电流充电，无报文则等5秒进入盲充
		preChargingTime++ ;
		//读取充电电流
			currentAd = adcGet(ADC_CHANNEL_CHARGING_CURRENT);	
		//等待800ms后，如果电流大于1.2A，则将充电电压调到25T，否则保持不变
			if(preChargingTime > 80 && currentAd > (ADV_CHARGING_CURRENT_ENTER_TRI + ADV_CURRENT_OFFSET)) {
	 	afterCompensatingPWM = calibrationPWM_get(CALIBRATION_PWM_VOLTAGE_AT_25T_IDX0);	
		// 根据请求电压调整PWM值	
		afterCompensatingPWM = afterCompensatingPWM *(requested_voltage - MIN_LIMITED_VOLTAGE)/ (MAX_LIMITED_VOLTAGE -MIN_LIMITED_VOLTAGE);
		PWM_set(PWM_CHANNEL_VOLTAGE , afterCompensatingPWM);				
		}
			
		//等了 >2秒 且 BMS 通信正常（电池已连接 / 盲充允许），等了>5秒进入盲充
		if((preChargingTime > 200 && nns_is_battery_can_charge())|| preChargingTime > 500 ) {

			//currentAd = adcGet(ADC_CHANNEL_CHARGING_CURRENT);
			// currentAd > 涓流进入阈值 + 偏置 → 电池在大量吃电流 → 说明电量低、可快充
			if(currentAd > ADV_CHARGING_CURRENT_ENTER_TRI + ADV_CURRENT_OFFSET) {
//				if(chargeSubprocessNow != CHARGING_STEP_CCM) {
					//恒流快充——电池虚，直接大电流冲
					chargeSubprocessNow = CHARGING_STEP_CCM;
					continue;
//				}
			} else {
				//否则 → 电流很小 → 说明电池可能已接近满电
//				if(chargeSubprocessNow != CHARGING_STEP_TRI) {
					//涓流慢充——电池实，先慢充
					chargeSubprocessNow = CHARGING_STEP_TRI;
					continue;
//				}
			}

		}
		return chargeSubprocessNow;
	} 
/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/ 
	
/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA  恒流(CCM)和恒压(CVM)共享这一段代码，*/
	if(chargeSubprocessNow == CHARGING_STEP_CCM || chargeSubprocessNow == CHARGING_STEP_CVM) {
		uint32_t fancur;
		static uint8_t enterTriDelay = 0 , entercvmDelay = 0 , batpackRemoveDelay = 0 ;
		static uint16_t blockCnt = 0;
		static uint32_t afterCompensatingVoltageAdv ; /**/
		static uint32_t afterCompensatingPWM ;
		static uint32_t afterCalibrationCurrentPWM;
		//static uint32_t percent60PWM;
//		static uint8_t fanWorkDelay = 0;
		static uint32_t ccAndcvTmLimit =0;

		static uint8_t fanFaultFlag = 0 , temperatureAbnormalFlag = 0  ;
		static uint16_t delay_testFanCur = 0;
		static uint8_t  derateFlag = 0;
    static uint32_t otpDly = 0;
//    static uint8_t bms_prot_flag = 0;

		//恒压横流都要处理的部分,
		//计时10小时，恒压横流都要做
		//风扇异常保护 都要做
		//温度异常保护都要做
		if(chargeSubprocessNow == CHARGING_STEP_CCM && chargeSubprocessLast != chargeSubprocessNow) {//第一次进入时要获取所需PWM
			enterTriDelay = 0;
			entercvmDelay = 0;
			afterCompensatingPWM = calibrationPWM_get(CALIBRATION_PWM_VOLTAGE_AT_25T_IDX0);
			afterCompensatingPWM = afterCompensatingPWM *(requested_voltage - MIN_LIMITED_VOLTAGE)/ (MAX_LIMITED_VOLTAGE -MIN_LIMITED_VOLTAGE);
			afterCalibrationCurrentPWM = calibrationPWM_get(CALIBRATION_PWM_CURRENT_RATED_IDX3);
			limitedPWM=afterCalibrationCurrentPWM;
			afterCalibrationCurrentPWM=afterCalibrationCurrentPWM*requested_current/(RATED_CURRENT*4);
			CompensatingChargingVoltage(&afterCompensatingVoltageAdv , &afterCompensatingPWM);//进行一次补偿
			
			PWM_set(PWM_CHANNEL_CURRENT , afterCalibrationCurrentPWM);
			PWM_set(PWM_CHANNEL_VOLTAGE , afterCompensatingPWM);
			ccAndcvTmLimit = 0;
      ccTmLimit = 0;
			cvTmLimit = 0;
			triTmLimit = 0;
			fanFaultFlag = 0;
			delay_testFanCur = 0;
			batpackRemoveDelay = 0;
			blockCnt = 0;
			derateFlag = 0;
			temperatureAbnormalFlag = 0;
//			fanWorkDelay = 0;
//			bms_prot_flag = 0;
//			over_turn = 0;
			chargeSubprocessLast = chargeSubprocessNow;
otpDly = 0;
		}
		
		
		
		/*大电流，高电压充电，高电压做温度补偿*/
		/*正常电流，降额电流，逐步下降的电流*/
			if(chargeSubprocessNow == CHARGING_STEP_CCM) {	//恒流充电，全电压+大电流，电流小到一定程度，转恒压

	//			if(chargeSubprocess0 != chargeSubprocess1) {
	//				chargeSubprocess1 = chargeSubprocess0;
	//			}
				nns_charger_state_set(NNS_CHARGER_STATE_CC);
				ccTmLimit++;
				if(ccTmLimit > CC_TIME_LIMIT) {
					crosscvFlag = 3;
					chargeSubprocessNow = CHARGING_STEP_FINISHED;
					continue;
				}
				
				
				iniTmLimit++;
				if(iniTmLimit >= INIT_TIME_LIMIT) {
         iniTmLimit = INIT_TIME_LIMIT;
				}				
				
				
				
			}

		/*2种电流，都是低电压充电*/
		/*低压全电流充，转到低压涓流*/
			if(chargeSubprocessNow == CHARGING_STEP_CVM) { //恒压充电

				static uint32_t CV_TIME_LIMIT_ADJ = 3600*100;
				if(chargeSubprocessNow != chargeSubprocessLast) {
					cvTmLimit = 0;
					CV_TIME_LIMIT_ADJ = CV_TIME_LIMIT;
					chargeSubprocessLast = chargeSubprocessNow;

				}
				nns_charger_state_set(NNS_CHARGER_STATE_CV);
				if((FW_RECOGNIZE & 0xF000UL*65536UL) == 0x3000UL*65536UL) {
					if(cvTmLimit % (ONE_SECOND*60*10) == 0) {
						int temp = ad2temperature(adcGet(ADC_CHANNEL_TEMP_ENV));
						if(temp > 27) {
							temp = 27;
						} else if(temp < -10) {
							temp = -10;
						}
						if(temp > 25) {
							CV_TIME_LIMIT_ADJ = CV_TIME_LIMIT - (temp - 25) * (3600*ONE_SECOND/20);
						} else {
							CV_TIME_LIMIT_ADJ = CV_TIME_LIMIT + (25 - temp) * (3600*ONE_SECOND/20);
						}
					}
				} else {
					CV_TIME_LIMIT_ADJ = CV_TIME_LIMIT;
				}
				cvTmLimit++ ;
				if(cvTmLimit > CV_TIME_LIMIT_ADJ/*CV_TIME_LIMIT*/) {//恒压不能超过2.5小时
					crosscvFlag = 3;
	//				if(chargeSubprocessNow != CHARGING_STEP_TRI) {
						chargeSubprocessNow = CHARGING_STEP_TRI;
						continue;
	//				}
				}
			}
		 ccAndcvTmLimit++ ;//保护也计时
		if(ccAndcvTmLimit > CC_CV_TIME_LIMIT|| nns_is_full()) {
			crosscvFlag = 3;
//			if(chargeSubprocessNow != CHARGING_STEP_TRI) {
				chargeSubprocessNow = CHARGING_STEP_TRI;
				continue;
//			}
		}
		
		
		
		
/***
 *风扇保护程序
 */
		//如果风扇未开  则清零
		if(ioPinGet_fanState() == PIN_RESET) {
			delay_testFanCur = 0;
			blockCnt = 0;
		}

//BMS_PROT_LOOP:
#if 0
		if(bms_prot_flag >= BMS_PROT_TIMES) {
			if(SCROffStepProcess() != SCR_OFF_STEP_FINISHED) {
				return (ChargingStepType_T)((uint8_t)chargeSubprocessNow | ERROR_FLAG_BMS_PROT);//先关闭可控硅输出，
			}
			if(nns_have_running_errs()) {
				return (ChargingStepType_T)((uint8_t)chargeSubprocessNow | ERROR_FLAG_BMS_PROT);
			} else {
				bms_prot_flag = 0;
//				over_turn = 0;
			}
		}
		if(nns_have_running_errs()) {
			bms_prot_flag ++;
			if(bms_prot_flag >= BMS_PROT_TIMES) {
				SCROffStepInit();
				goto BMS_PROT_LOOP;
			}
		} else {
			bms_prot_flag = 0;
		}
#endif
		
/*过欠压保护*/
		/*
			{
		uint32_t adc = adcGet(ADC_CHANNEL_CHARGING_VOLTAGE);
		uint32_t ac = 80*adc/1000;
		if(isOk_ACinput(ac) == 0){
			return (ChargingStepType_T)((uint8_t)chargeSubprocessNow | ERROR_FLAG_OVER_TEMP);  
		}
		}	
		*/
		
		//如果环境温度过高，则关闭可控硅
		//MOS温度判断，温度过高则关闭可控硅
/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
TEMPERATURE_ABNORMAL_ENTER:
		if(1){//如果发生了风扇堵转的故障,也不需要再进入高温保护了
			uint8_t temperatureAbnormal_MOS  = IS_TEMP_HIGHER_THAN(adcGet(ADC_CHANNEL_TEMP_MOS)  , ADV_CHARGING_PROTECT_ENTER_TEMP_MOS);
			uint8_t temperatureAbnormal_ENV = IS_TEMP_HIGHER_THAN(adcGet(ADC_CHANNEL_TEMP_ENV) , ADV_CHARGING_PROTECT_ENTER_TEMP_ENV);
			uint8_t temperatureRecovery_MOS = IS_TEMP_LOWER_THAN(adcGet(ADC_CHANNEL_TEMP_MOS) , ADV_CHARGING_PROTECT_EXIT_TEMP_MOS);
			uint8_t temperatureRecovery_ENV = IS_TEMP_LOWER_THAN(adcGet(ADC_CHANNEL_TEMP_ENV) , ADV_CHARGING_PROTECT_EXIT_TEMP_ENV);
			uint8_t ntcEnvbroken = isNtcFault(adcGet(ADC_CHANNEL_TEMP_ENV));
			if(temperatureAbnormalFlag) {//发生了高温保护

				if(SCROffStepProcess() != SCR_OFF_STEP_FINISHED) {
					return (ChargingStepType_T)((uint8_t)chargeSubprocessNow | ERROR_FLAG_OVER_TEMP);//先关闭可控硅输出，
				}
				//设置两路pwm
				if( (temperatureRecovery_MOS && (temperatureRecovery_ENV || ntcEnvbroken) ) == 0) {
					if(temperatureRecovery_MOS == 0 && temperatureAbnormalFlag == 1) {
						nns_charger_state_set(NNS_CHARGER_STATE_D102_OTP);
					} else {
						nns_charger_state_set(NNS_CHARGER_STATE_ENV_OTP);
					}
					return (ChargingStepType_T)((uint8_t)chargeSubprocessNow | ERROR_FLAG_OVER_TEMP);
				} else {
					temperatureAbnormalFlag = 0;//温度恢复正常了
//					over_turn = 0;
					nns_charger_state_reset(NNS_CHARGER_STATE_D102_OTP|NNS_CHARGER_STATE_ENV_OTP);			
				}
			}

			if(temperatureAbnormal_MOS || (temperatureAbnormal_ENV && !ntcEnvbroken)) {
				if(temperatureAbnormal_MOS) {
					temperatureAbnormalFlag = 1;
				} else {
					temperatureAbnormalFlag = 2;
				}
				SCROffStepInit();//执行关可控硅的函数初始化
				goto TEMPERATURE_ABNORMAL_ENTER;
			} else {
				temperatureAbnormalFlag = 0;
			}

			if (EN_ACTIVE_OTP)
			{uint8_t temperatureAbnormal_ENV1 = IS_TEMP_HIGHER_THAN(adcGet(ADC_CHANNEL_TEMP_ENV) , ADV_CHARGING_CURRENT_0o5_ENTER_TEMP_ENV);
			
				if(temperatureAbnormal_ENV1 && !ntcEnvbroken) {
					otpDly++;
					if(otpDly > ((uint32_t)(100*5*60))) {
						otpDly = 0;
						temperatureAbnormalFlag = 2;
						SCROffStepInit();//执行关可控硅的函数初始化
						goto TEMPERATURE_ABNORMAL_ENTER;
					}
				} else {					
					otpDly = 0;
				}
			}

		}
/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
//FAN_ERR_DETECT_ENTER:
			if(fanFaultFlag) {	

				//设置两路pwm
				if(FanRepeatTest() != FAN_DET_IS_OK) {//如果风扇故障，则进入打嗝保护程序
//					if(ccAndcvTmLimit / 10 &1) {
//						iOPinSet_SCR_off();
//					} else {
//						iOPinSet_SCR_on();
//					}
					nns_charger_state_set(NNS_CHARGER_STATE_SCR);
					nns_charger_state_set(NNS_CHARGER_STATE_FAN_ERR);
					return (ChargingStepType_T)((uint8_t)chargeSubprocessNow | ERROR_FLAG_FAN_ABNORMAL);//风扇故障则不往下执行
				} else {
					fanFaultFlag = 0;	/*PWM_set(PWM_CHANNEL_VOLTAGE , afterCompensatingPWM);	*/	
					afterCompensatingPWM = calibrationPWM_get(CALIBRATION_PWM_VOLTAGE_AT_25T_IDX0);
					afterCompensatingPWM = afterCompensatingPWM *(requested_voltage - MIN_LIMITED_VOLTAGE)/ (MAX_LIMITED_VOLTAGE -MIN_LIMITED_VOLTAGE);
					afterCalibrationCurrentPWM = calibrationPWM_get(CALIBRATION_PWM_CURRENT_RATED_IDX3);
					afterCalibrationCurrentPWM=afterCalibrationCurrentPWM*requested_current/(RATED_CURRENT*4);
					CompensatingChargingVoltage(&afterCompensatingVoltageAdv , &afterCompensatingPWM);//进行一次补偿
					PWM_set(PWM_CHANNEL_CURRENT , afterCalibrationCurrentPWM);
					PWM_set(PWM_CHANNEL_VOLTAGE , afterCompensatingPWM);		
					
					nns_charger_state_reset(NNS_CHARGER_STATE_FAN_ERR);
				}
			}


		delay_testFanCur++ ;
		if(delay_testFanCur > (uint32_t)(ONE_SECOND*5)) {
			int temp = ad2temperature(adcGet(ADC_CHANNEL_TEMP_ENV));
//			uint32_t addtion =0;
			uint8_t ntcbreak = 0;
			delay_testFanCur = ONE_SECOND * 10;

//			if(temp < 10) {
//				addtion =30000;
//			}
			ntcbreak = isNtcFault(adcGet(ADC_CHANNEL_TEMP_ENV));
//			if(ntcbreak ) {
//				addtion =0;
//			}
			fancur = adcGet(ADC_CHANNEL_FAN_CURRENT); // 获得风扇电流
			if((!ntcbreak) && (temp < 3)) {
				blockCnt = 0;//什么都不做
			} else if(FAN_PROTECT_ENABLE && (fancur < ADV_FAN_MINI_CURRENT || fanFlagGet()/*fancur > ADV_FAN_BLOCKING_CURRENT + addtion*/)) {
					blockCnt++ ;
				/*
				if(blockCnt > 30*ONE_SECOND) {
					fanFaultFlag = 1;
					FanRepeatTest_Init();//初始化故障程序
					blockCnt = 0;
					PWM_set(PWM_CHANNEL_CURRENT , PWM_CURRENT_TRI);//设置涓流充电PWM
					PWM_set(PWM_CHANNEL_VOLTAGE , calibrationPWM_get(CALIBRATION_PWM_VOLTAGE_FLOAT_IDX4));//
					goto FAN_ERR_DETECT_ENTER;
				}
				*/
			} else {
				if(blockCnt > 6) {
					blockCnt -= 6;
				} else {
					blockCnt = 0;
				}
			}
		}
/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

		if(ccAndcvTmLimit % (uint32_t)(ONE_SECOND*1800) == 0) {//半小时调整一次最大充电电压
			CompensatingChargingVoltage(&afterCompensatingVoltageAdv , &afterCompensatingPWM);//进行一次补偿
			PWM_set(PWM_CHANNEL_VOLTAGE , afterCompensatingPWM);
		}

#if 1
		/*
		{
			uint32_t currenPWM = afterCalibrationCurrentPWM;
			//uint32_t voltageAdvNow = adcGet(ADC_CHANNEL_CHARGING_VOLTAGE);//电压接近上限处理
			//降额处理
				uint8_t temperatureTooHi_MOS  = IS_TEMP_HIGHER_THAN(adcGet(ADC_CHANNEL_TEMP_MOS) , ADV_CHARGING_CURRENT_0o5_ENTER_TEMP_MOS);
				uint8_t temperatureTooHi_ENV = IS_TEMP_HIGHER_THAN(adcGet(ADC_CHANNEL_TEMP_ENV) , ADV_CHARGING_CURRENT_0o5_ENTER_TEMP_ENV);
				uint8_t temperatureNormal_MOS = IS_TEMP_LOWER_THAN(adcGet(ADC_CHANNEL_TEMP_MOS) , ADV_CHARGING_CURRENT_0o5_EXIT_TEMP_MOS);
				uint8_t temperatureNormal_ENV = IS_TEMP_LOWER_THAN(adcGet(ADC_CHANNEL_TEMP_ENV)  , ADV_CHARGING_CURRENT_0o5_EXIT_TEMP_ENV);
				uint8_t ntcEnvbroken =  isNtcFault( adcGet(ADC_CHANNEL_TEMP_ENV) ) ; // adcGet(ADC_CHANNEL_TEMP_ENV) > ADV_NTC_OPEN || ad2temperature(adcGet(ADC_CHANNEL_TEMP_ENV)) > 118 ;
				afterCalibrationCurrentPWM = calibrationPWM_get(CALIBRATION_PWM_CURRENT_RATED_IDX3);
			  percent60PWM = afterCalibrationCurrentPWM*PERCENT60/100;
			  afterCalibrationCurrentPWM=afterCalibrationCurrentPWM*requested_current/(RATED_CURRENT*4);
			 
			  currenPWM = afterCalibrationCurrentPWM;
				if( temperatureNormal_MOS && (temperatureNormal_ENV || ntcEnvbroken)) {
					nns_charger_state_reset(NNS_CHARGER_STATE_ENV_DERATING);
					derateFlag = 0;

					if(PWM_get(PWM_CHANNEL_CURRENT) != currenPWM) {
						PWM_set(PWM_CHANNEL_CURRENT , currenPWM);
					}
				}
				if(temperatureTooHi_MOS || (temperatureTooHi_ENV && !ntcEnvbroken)||Flag_DrateCC1A_AC || (iniTmLimit >= INIT_TIME_LIMIT)) {//温度过高，要降额
					nns_charger_state_set(NNS_CHARGER_STATE_ENV_DERATING);
					if(currenPWM > percent60PWM) {   //PWM_CURRENT_0o5_RATED
						derateFlag = 1;
						currenPWM = percent60PWM;
						if(Flag_DrateCC1A_AC) currenPWM = PWM_CURRENT_1A;
						if(PWM_get(PWM_CHANNEL_CURRENT) != currenPWM) {
							PWM_set(PWM_CHANNEL_CURRENT , currenPWM);
						}
					}
				}
		}
*/

		{
	    
			uint32_t currenPWM = afterCalibrationCurrentPWM;
			uint32_t voltageAdvNow = adcGet(ADC_CHANNEL_CHARGING_VOLTAGE);//电压接近上限处理

			/***------------------------------------BEGIN------------------------------------------***/
			/***
			 * 降额与恢复
			 * 环境温度高了就降额，降低充电电流
			 */
			//降额处理
				uint8_t temperatureTooHi_MOS  = IS_TEMP_HIGHER_THAN(adcGet(ADC_CHANNEL_TEMP_MOS) , ADV_CHARGING_CURRENT_0o5_ENTER_TEMP_MOS);
				uint8_t temperatureTooHi_ENV = IS_TEMP_HIGHER_THAN(adcGet(ADC_CHANNEL_TEMP_ENV) , ADV_CHARGING_CURRENT_0o5_ENTER_TEMP_ENV);
				uint8_t temperatureNormal_MOS = IS_TEMP_LOWER_THAN(adcGet(ADC_CHANNEL_TEMP_MOS) , ADV_CHARGING_CURRENT_0o5_EXIT_TEMP_MOS);
				uint8_t temperatureNormal_ENV = IS_TEMP_LOWER_THAN(adcGet(ADC_CHANNEL_TEMP_ENV)  , ADV_CHARGING_CURRENT_0o5_EXIT_TEMP_ENV);
			
				uint8_t ENV081H = IS_TEMP_HIGHER_THAN(adcGet(ADC_CHANNEL_TEMP_ENV) , ADV_CHARGING_CURRENT_081_ENTER_TEMP_ENV);
				uint8_t ENV081L = IS_TEMP_LOWER_THAN(adcGet(ADC_CHANNEL_TEMP_ENV)  , ADV_CHARGING_CURRENT_081_EXIT_TEMP_ENV);
			
				uint8_t ntcEnvbroken =  isNtcFault( adcGet(ADC_CHANNEL_TEMP_ENV) ) ; // adcGet(ADC_CHANNEL_TEMP_ENV) > ADV_NTC_OPEN || ad2temperature(adcGet(ADC_CHANNEL_TEMP_ENV)) > 118 ;


		{
			
		uint32_t  percent60PWM = afterCalibrationCurrentPWM*PERCENT60/100;	
		uint32_t	pwm08=afterCalibrationCurrentPWM*PERCENT80/100;
		uint32_t	beforepwm=PWM_get(PWM_CHANNEL_CURRENT);
		uint32_t	vx10=adcGet(ADC_CHANNEL_BAT_VOLTAGE)/737;
			
			  if(  ENV081L || ntcEnvbroken ) {  
						currenPWM = afterCalibrationCurrentPWM;			
				}
				else if( !ENV081H  ) {
					if (beforepwm <= pwm08 )
					{
						currenPWM = pwm08;
					}
					else {
            currenPWM = afterCalibrationCurrentPWM;
					}		
				}
	
				else if( temperatureNormal_ENV ) {
						currenPWM = pwm08;
				}
				else if( !temperatureTooHi_ENV  ) {
					if(beforepwm <= percent60PWM)		
					{
            currenPWM = percent60PWM;
					}
					else {
            currenPWM = pwm08;
					}
				}
					else {
            currenPWM = percent60PWM;
				}
     
				if(vx10>VPERCENT80) {
					 if (limitedPWM > pwm08 )limitedPWM--;
				}
				if(vx10>VPERCENT60) {
					 if (limitedPWM > percent60PWM )limitedPWM--;
				}
				
				if(currenPWM>limitedPWM) currenPWM=limitedPWM;

				
				if((Flag_DrateCC1A_AC == 1 ) && (currenPWM > PWM_CURRENT_1A ))  // AC over range.
					currenPWM= PWM_CURRENT_1A;				
							
					if(beforepwm != currenPWM) {
							PWM_set(PWM_CHANNEL_CURRENT , currenPWM);
						}
		}
				
		}
		
			/***======================END=========================***/
			/***------------------------------------BEGIN------------------------------------------***/
			{uint32_t voltageMaster = adcGet(ADC_CHANNEL_CHARGING_VOLTAGE);
				uint32_t currentAd = adcGet(ADC_CHANNEL_CHARGING_CURRENT);
				//如果电流小于0.6A，进入涓流充电
				if(currentAd > ADV_CHARGING_CURRENT_ENTER_CONSTANT_VOLTAGE + ADV_CURRENT_OFFSET) {
					crosscvFlag |= 1;
				}
				if(currentAd < ADV_CHARGING_CURRENT_ENTER_CONSTANT_VOLTAGE ) {
					crosscvFlag |= 2;
				}


				if( PWR_Flag_Status_Get(PWR_PVD_OUTPUT_FLAG) == SET) {
					//发生了电压跌落
					//如果是因为拔电池包，电压跌落的时间宽度要求限制在0.5s以内
				} else if(1 || voltageMaster > ADV_AT_LEAST_IF_PACK_FULL) {//涓流 恒压 拔电池包 都会使得输出电压升高，拔电池包会有短暂的下降，然后升高
					{/*进入恒压的判断*/
						uint8_t shoudCv0 = (derateFlag == 0 && currentAd < ADV_CHARGING_CURRENT_ENTER_CONSTANT_VOLTAGE );
						uint8_t shoudCv1 = (derateFlag &&  currentAd < ADV_CHARGING_CURRENT_ENTER_CONSTANT_VOLTAGE_AT_DERATE);
						if(shoudCv0 ||  shoudCv1) {//进入恒压
							entercvmDelay++ ;
							if(entercvmDelay > 100) {
//								if(chargeSubprocessNow != CHARGING_STEP_CVM) {
									chargeSubprocessNow = CHARGING_STEP_CVM;
///////								continue;
//								}
							}
						} else {
							entercvmDelay = 0;
						}
					}

					if(currentAd < 2000){/*拔掉电池包判断 100ma */
						batpackRemoveDelay ++ ;
						if(batpackRemoveDelay > 80) {
//							if(chargeSubprocessNow != CHARGING_STEP_FINISHED) {
								chargeSubprocessNow = CHARGING_STEP_FINISHED;
								reset_charging_control_bits();
								continue;
//							}
						}
					} else {
						batpackRemoveDelay = 0;

						if( currentAd < ADV_CHARGING_CURRENT_ENTER_TRI + ADV_CURRENT_OFFSET) {//电流自然变小，进入涓流
							enterTriDelay++ ;
							if(enterTriDelay > 150) {
//								if(chargeSubprocessNow != CHARGING_STEP_TRI) {
									chargeSubprocessNow = CHARGING_STEP_TRI;
									continue;
//								}
							}
						} else {
							enterTriDelay = 0;
						}
					}
				} else {
					entercvmDelay = 0;
					batpackRemoveDelay = 0;
					enterTriDelay = 0;
				}


/*
				if(isBatPackTakeAway()) {
//					fanWorkDelay = 0;
					iOPinSet_FanOff();
				} else {
							fanWorkDelay++;
						if(1 || fanWorkDelay > 100) {
							fanWorkDelay = 250;
							iOPinSet_FanOn();
						}
				}
				*/
				/*
				if(over_turn < 50)
				{over_turn++;
					if(over_turn  / 10 &1) {
						iOPinSet_SCR_off();
					} else {
						iOPinSet_SCR_on();
					}
				}
				else {
					iOPinSet_SCR_off();
				}
				*/
				  iOPinSet_FanOn();
				  iOPinSet_SCR_on();
				
					nns_charger_state_set(NNS_CHARGER_STATE_SCR);

			}
			/***======================END=========================***/

#endif

		//以上是横流恒压都要处理的部分
		
	}
/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/	


/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
	//涓流充电
	if(chargeSubprocessNow == CHARGING_STEP_TRI) {
//		static uint8_t bms_prot_flag = 0;

		static uint8_t  returnDly = 0;
		static uint8_t currentDetDelay = 0;
		static uint16_t currentRiseDly = 0;
		static uint8_t removeDly1 = 0;
		static uint8_t temperatureAbnormal_MOS_delay = 0;
		uint8_t temperatureAbnormal_MOS  = IS_TEMP_HIGHER_THAN(adcGet(ADC_CHANNEL_TEMP_MOS)  , ADV_CHARGING_PROTECT_ENTER_TEMP_MOS);
		if(chargeSubprocessNow != chargeSubprocessLast) {
			uint32_t floatChargingPwm ;
			chargeSubprocessLast = chargeSubprocessNow;
//			over_turn = 0;
			iOPinSet_FanOff();//关闭风扇
//			bms_prot_flag = 0;
			triTmLimit = 0;
			returnDly = 0;
			currentRiseDly = 0;
			currentDetDelay = 0;
			removeDly1 = 0;
			temperatureAbnormal_MOS_delay = 0;
			floatChargingPwm = calibrationPWM_get(CALIBRATION_PWM_VOLTAGE_FLOAT_IDX4);
//			PWM_set(PWM_CHANNEL_CURRENT , 0);//设置涓流充电PWM
			PWM_set(PWM_CHANNEL_VOLTAGE , floatChargingPwm);//
		}
		nns_charger_state_set(NNS_CHARGER_STATE_TRI);
		iOPinSet_FanOff();//关闭风扇

		triTmLimit++ ;
		if(triTmLimit > TC_TIME_LIMIT) {//涓流不能超过2.5小时
//				if(chargeSubprocessNow != CHARGING_STEP_FINISHED) {
				chargeSubprocessNow = CHARGING_STEP_FINISHED;
				continue;
//				}
		}
#if 0
BMS_PROT_LOOP1:
		if(bms_prot_flag >= BMS_PROT_TIMES) {
			if(SCROffStepProcess() != SCR_OFF_STEP_FINISHED) {
				return (ChargingStepType_T)((uint8_t)chargeSubprocessNow | ERROR_FLAG_BMS_PROT);//先关闭可控硅输出，
			}
			if(nns_have_running_errs()) {
				return (ChargingStepType_T)((uint8_t)chargeSubprocessNow | ERROR_FLAG_BMS_PROT);
			} else {
				bms_prot_flag = 0;
			}
		}
		if(nns_have_running_errs()) {
			bms_prot_flag ++;
			if(bms_prot_flag >= BMS_PROT_TIMES) {
				SCROffStepInit();
				goto BMS_PROT_LOOP1;
			}
		} else {
			bms_prot_flag = 0;
		}
#endif

		if(temperatureAbnormal_MOS) {
			temperatureAbnormal_MOS_delay++;
			if(temperatureAbnormal_MOS_delay > 100) {
				chargeSubprocessNow = CHARGING_STEP_FINISHED;
				continue;
			} else {

			}
		} else {
			temperatureAbnormal_MOS_delay = 0;
		}

		if(crosscvFlag != 3){   // 这段是如果没有进入过恒流恒压充电则等电流比较大后进入恒流模式？

			uint32_t currentAd = adcGet(ADC_CHANNEL_CHARGING_CURRENT);
			if(currentAd > ADV_CHARGING_CURRENT_EXIT_TRI + ADV_CURRENT_OFFSET) {
				returnDly++ ;
				if(returnDly > 100) {
//					if(chargeSubprocessNow != CHARGING_STEP_CCM) {
						chargeSubprocessNow = CHARGING_STEP_CCM;
						continue;
//					}
				}
			} else {
				returnDly = 0;
			}


				if(currentAd < 2000) {
					removeDly1++;
					if(removeDly1 > 200) {
							chargeSubprocessNow = CHARGING_STEP_FINISHED;
							reset_charging_control_bits();
							continue;
					}
				} else {
					removeDly1 = 0;
				}


		} else {   // 这段是等待电流大于105ma后或等待5S 后，检查电流，如果小于100ma就停充？
			uint32_t currentAd = adcGet(ADC_CHANNEL_CHARGING_CURRENT);
			PWM_set(PWM_CHANNEL_CURRENT , PWM_CURRENT_TRI);//设置涓流充电PWM
			currentRiseDly++ ;
			if(currentRiseDly > 60*ONE_SECOND) {
				currentRiseDly--;
				if(currentAd < 2000) {
					removeDly1++;
					if(removeDly1 > 100) {
							chargeSubprocessNow = CHARGING_STEP_FINISHED;
							reset_charging_control_bits();
							continue;
					}
				} else {
					removeDly1 = 0;
				}
			}
			else if(currentAd > 2200) {
				currentDetDelay++;
				if(currentDetDelay > 100) {
					currentRiseDly =61* ONE_SECOND;
				}
			}
		}
		/*
		if(over_turn < 50)
		{over_turn++;
			if(over_turn  / 10 &1) {
				iOPinSet_SCR_off();
			} else {
				iOPinSet_SCR_on();
			}
		}
		else {
			iOPinSet_SCR_off();
		}
		*/
		iOPinSet_SCR_on();
		nns_charger_state_set(NNS_CHARGER_STATE_SCR);

	} 
/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/   	
	//充电完成
	if(chargeSubprocessNow == CHARGING_STEP_FINISHED) {
		if(chargeSubprocessNow != chargeSubprocessLast) {
			chargeSubprocessLast = chargeSubprocessNow;
		}
		iOPinSet_FanOff();//关闭风扇
		return chargeSubprocessNow;//什么都不做，
	}
	if(chargeSubprocessNow >= CHARGING_STEP_INVALID){
		//异常
		chargeSubprocessNow = CHARGING_STEP_FINISHED;
		continue;
	}

	return chargeSubprocessNow;
	}
	return chargeSubprocessNow;
}

/*****关闭可控硅的程序
 * 在执行一次完整的可控硅关断序列之前，先调用一次thyristorOffStepInit，初始化关断序列。
 * 在执行关断序列中，不要执行thyristorOffStepInit
 * 关断过程中需要尽量省电，
 * 外部需要打开灯等负载时，应该查询一下当前是否处于关断状态
 */
static  SCROffStepType_T SCROffStep0 = SCR_OFF_STEP_0, SCROffStep1 = SCR_OFF_STEP_INVALID;

uint32_t SCROffStep_needLowPower(void) {
	if(SCROffStep0 != SCR_OFF_STEP_1 && SCROffStep1 != SCR_OFF_STEP_1) {
		return 0;
	} else {
		return 1;
	}
}

/***
 * 初始化函数，在执行关断函数前，要调用一次，确保关断函数正确执行
 */
void SCROffStepInit(void) {
	SCROffStep0 = SCR_OFF_STEP_0;
	SCROffStep1 = SCR_OFF_STEP_INVALID;
}

/****
 * 可控硅关断函数，分3个步骤
 * 先关闭负载，风扇和灯，再拉掉可控硅触发电平，然后把参考电平拉低，关闭输出。
 * 然后等待100ms，期间利用了电容上的存电
 * 再把拉低参考电平的引脚，设为输入，让电源继续工作
 */
SCROffStepType_T SCROffStepProcess(void) {
	int n = 16;
	uint32_t floatChargingPwm;
	while(n-- > 0) {
	/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
	if(SCROffStep0 == SCR_OFF_STEP_0) {
		if(SCROffStep0 != SCROffStep1) {
			SCROffStep1 = SCROffStep0;

		}
//		red_led = LED_OFF;//关红灯
//		green_led = LED_OFF;//关绿灯
		 iOPinSet_FanOff();//关闭风扇
		//iOPinSet_SCR_off();//拉低可控硅触发
		
		
		PWM_set(PWM_CHANNEL_VOLTAGE , 0);
    PWM_set(PWM_CHANNEL_CURRENT , 0);

		nns_charger_state_reset(NNS_CHARGER_STATE_SCR);
		SCROffStep0 = SCR_OFF_STEP_1;
		continue;
	}
	/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/


	/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
	/**
	 * 延时，确保可控硅能关断
	 */
	if(SCROffStep0 == SCR_OFF_STEP_1) {
		static uint8_t dlyCnt = 0;
		if(SCROffStep0 != SCROffStep1) {
			SCROffStep1 = SCROffStep0;
			dlyCnt = 0;
		}

		if(dlyCnt == 2) {
			//iOPinSet_VRef_OutAndLow();//拉低电压参考输入
		}
		dlyCnt++;//计时100ms
		
		if(dlyCnt >= (uint32_t)(ONE_SECOND*300/1000)) {
			//iOPinSet_VRef_Input();//参考拉低引脚设为输入
			SCROffStep0 = SCR_OFF_STEP_FINISHED;
			iOPinSet_SCR_off();//拉低可控硅触发
			floatChargingPwm = calibrationPWM_get(CALIBRATION_PWM_VOLTAGE_FLOAT_IDX4);
			PWM_set(PWM_CHANNEL_VOLTAGE , floatChargingPwm);		
			continue;
		} else {
			return SCROffStep0;
		}
		
	}
	/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/


	/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
	if(SCROffStep0 == SCR_OFF_STEP_FINISHED) {
		if(SCROffStep0 != SCROffStep1) {
			SCROffStep1 = SCROffStep0;
		}
		//iOPinSet_VRef_Input();//参考拉低引脚设为输入		
	}
	/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/
	return SCROffStep0;
	}//end while
	return SCROffStep0;
}



SCROffStepType_T SCROffStepProcess2(void) {
	int n = 16;
	while(n-- > 0) {
	/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
	if(SCROffStep0 == SCR_OFF_STEP_0) {
		if(SCROffStep0 != SCROffStep1) {
			SCROffStep1 = SCROffStep0;

		}
//		red_led = LED_OFF;//关红灯
//		green_led = LED_OFF;//关绿灯
		iOPinSet_FanOff();//关闭风扇
		iOPinSet_SCR_off();//拉低可控硅触发
		nns_charger_state_reset(NNS_CHARGER_STATE_SCR);
		SCROffStep0 = SCR_OFF_STEP_1;
		continue;
	}
	/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/


	/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
	/**
	 * 延时，确保可控硅能关断
	 */
	if(SCROffStep0 == SCR_OFF_STEP_1) {
		static uint8_t dlyCnt = 0;
		if(SCROffStep0 != SCROffStep1) {
			SCROffStep1 = SCROffStep0;
			dlyCnt = 0;
		}

		if(dlyCnt == 2) {
			//iOPinSet_VRef_OutAndLow();//拉低电压参考输入
		}
		dlyCnt++;//计时100ms
		
		if(dlyCnt >= (uint32_t)(ONE_SECOND*/*SHUT_OFF_DELAY*/70/1000)) {
			//iOPinSet_VRef_Input();//参考拉低引脚设为输入
			SCROffStep0 = SCR_OFF_STEP_FINISHED;
			continue;
		} else {
			return SCROffStep0;
		}
		
	}
	/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/


	/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
	if(SCROffStep0 == SCR_OFF_STEP_FINISHED) {
		if(SCROffStep0 != SCROffStep1) {
			SCROffStep1 = SCROffStep0;
		}
		//iOPinSet_VRef_Input();//参考拉低引脚设为输入		
	}
	/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/
	return SCROffStep0;
	}//end while
	return SCROffStep0;
}


void  CompensatingChargingVoltage(uint32_t *compensatingVoltageAdv , uint32_t *compensatingPWM) {

	int temp = ad2temperature(adcGet(ADC_CHANNEL_TEMP_ENV));
	int distance = 0;
	uint32_t  hitemp = calibrationPWM_get(CALIBRATION_PWM_VOLTAGE_AT_45T_IDX1);
	uint32_t lotemp = calibrationPWM_get(CALIBRATION_PWM_VOLTAGE_AT_N10T_IDX2);
	uint32_t nortemp = calibrationPWM_get(CALIBRATION_PWM_VOLTAGE_AT_25T_IDX0);

	if(temp <= -10) {//低出下界
		temp = -10;
	} else if(temp >= 45) {//高出上界
		temp = 45;
	}
	if( isNtcFault( adcGet( ADC_CHANNEL_TEMP_ENV ) ) ) {
		temp = 25;
	}
	if(temp == 25) {
		//给出全电压
		*compensatingVoltageAdv  = ADV_BAT_VOLTAGE_AT_25T;
		* compensatingPWM = nortemp;
		//给出全电压对应的PWM   是经过校准的
	} else if(temp < 25) {//低温段
		distance = 25 - temp;
		//低温电压ad值每度加220
		*compensatingVoltageAdv = distance * 220 + ADV_BAT_VOLTAGE_AT_25T;//给出全电压
		if(lotemp > nortemp) {
			*compensatingPWM = ( lotemp - nortemp)*(25 - temp)/(25 - (-10)) + nortemp;
		}else {
			* compensatingPWM = nortemp;
		}
		//给出全电压对应的PWM是计算的
	} else {//高温段
		distance = temp - 25;
		//高温电压ad值每度减440
		*compensatingVoltageAdv = ADV_BAT_VOLTAGE_AT_25T - 440 * distance;//给出全电压
		if(nortemp > hitemp) {
			*compensatingPWM = nortemp - (nortemp - hitemp)*(temp - 25) / (45 - 25) ;
		}else {
			* compensatingPWM = nortemp;
		}
		//给出全电压对应的PWM  是计算的
	}


}

//belowing communication protocal needed.....

uint8_t nns_get_charge_step()
{
	if(chargeSubprocessNow==CHARGING_STEP_CCM) return 0;
	if(chargeSubprocessNow==CHARGING_STEP_CVM) return 1;
	if(chargeSubprocessNow==CHARGING_STEP_TRI) return 2;
	return 3;
}

uint32_t nns_get_step_time_min()
{
	return (ccTmLimit+cvTmLimit+triTmLimit)/(ONE_SECOND*60);
}


extern const uint32_t ALLOW_CHARGE_BAT_VOLTAGE;
/*
uint8_t isBatPackNotOK(void) {
	uint8_t res=0;
	if (adcGet(ADC_CHANNEL_BAT_VOLTAGE) < ALLOW_CHARGE_BAT_VOLTAGE) res=1;     //60v: 39V*0.5; 72V:47V
	if(nns_is_battery_in_error()>0) res=1;
	return res;
}
uint8_t isBatPackTakeAway(void) {
	uint8_t res=0;
	if ((adcGet(ADC_CHANNEL_BAT_VOLTAGE) < ALLOW_CHARGE_BAT_VOLTAGE) && nns_is_battery_not_connected())
		{ res=1;     //60v: 39V*0.5; 72V:47V
      reset_charging_control_bits();
		}
	return res;
}
uint8_t isBatPackVolOK(void) {
	uint8_t res=0;
	if (adcGet(ADC_CHANNEL_BAT_VOLTAGE) > ALLOW_CHARGE_BAT_VOLTAGE) 
		{ res=1;     //60v: 39V*0.5; 72V:47V
		}
	return res;
}
*/
uint8_t isBatPackNotOK(void) {
	return 0;
}
uint8_t isBatPackTakeAway(void) {
	return 0;
}
uint8_t isBatPackVolOK(void) {
	return 1;
}

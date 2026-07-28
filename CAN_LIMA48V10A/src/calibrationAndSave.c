/*
 * calibration.c
 *
 *  Created on: 2022年12月8日
 *      Author: Jeffery
 */
#include "includeAll.h"
/*给一组默认值，方便调试，也可以让大多数板子正常工作*/
#define CALIBRATION_PARAMETER_QUANTITY      6
 uint32_t calibrationPWM[CALIBRATION_PARAMETER_QUANTITY] = {0};
 uint32_t calibrationPWMnew[CALIBRATION_PARAMETER_QUANTITY];
static uint32_t calibrationPWMnewId = 100;
static uint8_t calibrationNeedDoflag = 0;
static uint32_t saveFlag = 0;
static uint8_t dly1S = 0;
static void calibrationSave(uint32_t dat[] , uint32_t length , uint32_t key) ;
static uint8_t calibrationRead(uint32_t dat[] , uint32_t length) ;

uint8_t calibrationInit(void) {
	uint8_t errFlag = 0;
	saveFlag = 0;
	dly1S = 0;
	calibrationPWM[CALIBRATION_PWM_VOLTAGE_AT_25T_IDX0] = PWM_VOLTAGE_NORMAL;
	calibrationPWM[CALIBRATION_PWM_VOLTAGE_AT_45T_IDX1] = PWM_VOLTAGE_HI_TEM;
	calibrationPWM[CALIBRATION_PWM_VOLTAGE_AT_N10T_IDX2] = PWM_VOLTAGE_LO_TEM;
	calibrationPWM[CALIBRATION_PWM_CURRENT_RATED_IDX3] = PWM_CURRENT_RATED;
	calibrationPWM[CALIBRATION_PWM_VOLTAGE_FLOAT_IDX4] = PWM_VOLTAGE_FLOAT;

	errFlag = calibrationRead(calibrationPWM , CALIBRATION_PARAMETER_QUANTITY);

	return errFlag;
}

void calibrationSaveApply(uint32_t key) {
	saveFlag = key;
}

/***
 * channel:        CALIBRATION_PWM_VOLTAGE_AT_25T_IDX0
 * 				CALIBRATION_PWM_VOLTAGE_AT_45T_IDX1
 * 				CALIBRATION_PWM_VOLTAGE_AT_N10T_IDX2
 * 				CALIBRATION_PWM_CURRENT_RATED_IDX3
 * 				 CALIBRATION_PWM_VOLTAGE_FLOAT_IDX4
 *				 CALIBRATION_RATED_CURRENT_AD_IDX5
 *newVal:        不大于999的无符号整数
 */
void calibrationPWM_set(uint32_t channel  , uint32_t newVal) {
	if(channel <= CALIBRATION_RATED_CURRENT_AD_IDX5) {
		calibrationPWMnew[channel] = newVal;
		calibrationPWMnewId = channel;
		calibrationNeedDoflag = 1;
	}
}


/***
 * channel:        	CALIBRATION_PWM_VOLTAGE_AT_25T_IDX0
 * 									CALIBRATION_PWM_VOLTAGE_AT_45T_IDX1
 * 									CALIBRATION_PWM_VOLTAGE_AT_N10T_IDX2
 * 									CALIBRATION_PWM_CURRENT_RATED_IDX3
 * 									CALIBRATION_PWM_VOLTAGE_FLOAT_IDX4
 *					 				CALIBRATION_RATED_CURRENT_AD_IDX5
 */
uint32_t calibrationPWM_get(uint32_t channel) {
	if(channel <= CALIBRATION_RATED_CURRENT_AD_IDX5) {
		return calibrationPWM[channel];
	} else {
		return 0;
	}
}
/***
 * 校准计算
 *计算出当前温度
 *
 *
 */

/***
 *把设置的pwm输出到端口，
 *把数据保存进flash
 */
static uint8_t disposable =0;
void calibrationRunning(void) {
	//是否首次进入
	if(disposable == 0) {
		uint32_t floatChargingPwm ; //浮充电压校准
		uint32_t ratedCurrentPwm; //3/4A电流校准
		static uint8_t over_turn = 0; //翻转次数
		floatChargingPwm = calibrationPWM_get(CALIBRATION_PWM_VOLTAGE_FLOAT_IDX4); 
		ratedCurrentPwm = calibrationPWM_get(CALIBRATION_PWM_CURRENT_RATED_IDX3);
		PWM_set(PWM_CHANNEL_VOLTAGE , floatChargingPwm);//
		PWM_set(PWM_CHANNEL_CURRENT , ratedCurrentPwm );
		iOPinSet_FanOn(); //打开风扇
		over_turn++;
		/*
		if(over_turn / 10 & 1) {
			iOPinSet_SCR_on();
		} else {
			iOPinSet_SCR_off();
		}
		*/
		iOPinSet_SCR_on(); //打开可控硅输出的
		
		disposable = 1; //已经进入过了
	}
	if(calibrationNeedDoflag ) { //需要校准		
		calibrationNeedDoflag = 0; //清除标志位
		switch(calibrationPWMnewId) { //根据校准的通道，设置对应的pwm值
		case 	CALIBRATION_PWM_VOLTAGE_AT_25T_IDX0   :  //代表低温（25℃）下的充电电压 PWM 值
			if(calibrationPWMnew[calibrationPWMnewId] > PWM_MAX) { //防止pwm值过大
				calibrationPWMnew[calibrationPWMnewId] = PWM_MAX;// 超范围保护
			}
			PWM_set(PWM_CHANNEL_VOLTAGE , calibrationPWMnew[calibrationPWMnewId]); //设置pwm值
			dly1S = 100; //延时100ms
			break;
		case 	CALIBRATION_PWM_VOLTAGE_AT_45T_IDX1   : //代表低温（45℃）下的充电电压 PWM 值
			if(calibrationPWMnew[calibrationPWMnewId]> PWM_MAX) { 
				calibrationPWMnew[calibrationPWMnewId] = PWM_MAX;// 超范围保护
			}
			PWM_set(PWM_CHANNEL_VOLTAGE , calibrationPWMnew[calibrationPWMnewId]);
			dly1S = 100;
			break;
		case 	CALIBRATION_PWM_VOLTAGE_AT_N10T_IDX2  : //代表低温（-10℃）下的充电电压 PWM 值
			if(calibrationPWMnew[calibrationPWMnewId]> PWM_MAX) {
				calibrationPWMnew[calibrationPWMnewId] = PWM_MAX;// 超范围保护
			}
			PWM_set(PWM_CHANNEL_VOLTAGE , calibrationPWMnew[calibrationPWMnewId]);
			dly1S = 100;
			break;
		case 	CALIBRATION_PWM_CURRENT_RATED_IDX3    ://3/4A电流校准
			if(calibrationPWMnew[calibrationPWMnewId]> PWM_MAX) {
				calibrationPWMnew[calibrationPWMnewId] = PWM_MAX;// 超范围保护
			}
			PWM_set(PWM_CHANNEL_CURRENT , calibrationPWMnew[calibrationPWMnewId]);//刚设置的是电流PWM，现在要把这个电流PWM进行输出，
			dly1S = 0;
			break;
		case CALIBRATION_PWM_VOLTAGE_FLOAT_IDX4:  //浮充电压校准
			if(calibrationPWMnew[calibrationPWMnewId]> PWM_MAX) {
				calibrationPWMnew[calibrationPWMnewId] = PWM_MAX;// 超范围保护
			}
			PWM_set(PWM_CHANNEL_VOLTAGE , calibrationPWMnew[calibrationPWMnewId]);
			dly1S = 100;
			break;
		default:
			break;
		}
	}
	if(calibrationPWMnewId == CALIBRATION_PWM_CURRENT_RATED_IDX3) { //	3/4A电流校准
		/**读取3次电流值，把可信值保存起来**/
		static uint32_t ratedCurrenAdv[3];
		static uint32_t max , min ;
		uint32_t diff;
		if(dly1S == 100) {
			ratedCurrenAdv[0] = adcGet(ADC_CHANNEL_CHARGING_CURRENT);
			max = ratedCurrenAdv[0] ;
			min = ratedCurrenAdv[0] ;
		} else if(dly1S == 150) {
			ratedCurrenAdv[1] = adcGet(ADC_CHANNEL_CHARGING_CURRENT);
			if(ratedCurrenAdv[1]  > max) {
				max = ratedCurrenAdv[1] ;
			} else if(ratedCurrenAdv[1]  < min) {
				min = ratedCurrenAdv[1] ;
			} else {

			}
		} else if(dly1S == 200) {
			ratedCurrenAdv[2] = adcGet(ADC_CHANNEL_CHARGING_CURRENT);
			if(ratedCurrenAdv[2]  > max) {
				max = ratedCurrenAdv[2] ;
			} else if(ratedCurrenAdv[2]  < min) {
				min = ratedCurrenAdv[2] ;
			} else {

			}
			diff = max - min;//
			if(diff < min * 3 / 128) {
				calibrationPWMnew[CALIBRATION_RATED_CURRENT_AD_IDX5] = (ratedCurrenAdv[0] + ratedCurrenAdv[1] + ratedCurrenAdv[2])/3;
			} else {
				dly1S = 99;
			}
		}
	}
//是一段电压校准值的单调性自检，用于在校准模式下用红灯提示“三个温度点的电压标定值是否合理”
	if(calibrationPWMnew[CALIBRATION_PWM_VOLTAGE_AT_45T_IDX1] > calibrationPWMnew[CALIBRATION_PWM_VOLTAGE_AT_25T_IDX0]
	|| calibrationPWMnew[CALIBRATION_PWM_VOLTAGE_AT_25T_IDX0] > calibrationPWMnew[CALIBRATION_PWM_VOLTAGE_AT_N10T_IDX2]) {
		red_led = LED_ON;
	}
	else {
		red_led = LED_OFF;
	}
	dly1S++;
	if(dly1S >= 200 ) {
		dly1S = 250;
//		green_led = LED_ON;
	} else {
		green_led = LED_OFF;
	}

	if(saveFlag ) {		
		green_led = LED_OFF;
		calibrationSave(calibrationPWMnew , CALIBRATION_PARAMETER_QUANTITY , saveFlag);
		saveFlag = 0;
	}
}

/***
 * 校准值的保存
 */
static void calibrationSave(uint32_t dat[] , uint32_t length , uint32_t key) {
	WriteFlashFormat_T save;
	int i;
	for(i = 0 ; i < length ; i++) {
		save.dat[i*2] = dat[i];
		save.dat[i*2+1] = ~dat[i];
	}
	save.addr = LAST_ADDR_BLOCK;

	flashWriteIn(save.dat , length * 2 , key);
}



/***
 * 校准值的读取
 */
static uint8_t calibrationRead(uint32_t dat[] , uint32_t length) {
	uint32_t  readOutDat;
	uint32_t nreadOutDat;
	int i ;
	uint8_t errFlag = 0;
	for(i = 0 ; i < length ; i++) {
		readOutDat = flash_read(LAST_ADDR_BLOCK + i  *2 * 4);
		nreadOutDat = flash_read(LAST_ADDR_BLOCK + i *2  * 4+ 4);
		if(readOutDat == ~nreadOutDat) {
			dat[i] = readOutDat;
		} else {
			errFlag = 1;
		}
	}
	return errFlag;
}

/*
 * definePWM.h
 *
 *  Created on: 2022年12月8日
 *      Author: Jeffery
 */

#ifndef FW_DEFINEPWM_H_
#define FW_DEFINEPWM_H_

#define PWM_CURRENT_0o5_RATED					keyParam[4] /*506*/

#define PWM_CURRENT_TRI                                0

#define PWM_MAX     999

/***
 * 初始化时装入这些值，
 */
#define PWM_VOLTAGE_NORMAL                   keyParam[19]  /*800*/  /*25℃对应73.5V*/
#define PWM_VOLTAGE_LO_TEM                    keyParam[21]  /* 920 */ /*对应低温-10℃时，最高输出电压75.075V*/
#define PWM_VOLTAGE_HI_TEM                  keyParam[20] /*650 */  /*对应高温45℃时的最低电压输出71.7V*/
#define PWM_CURRENT_RATED                    keyParam[22] /*950*/   /*对应额定电流输出时的占空比*/
#define PWM_VOLTAGE_FLOAT					keyParam[23]		/*520 */  /**浮充电压对应的pwm   69V**/

#define CALIBRATION_PWM_VOLTAGE_AT_25T_IDX0      0
#define CALIBRATION_PWM_VOLTAGE_AT_45T_IDX1      1
#define CALIBRATION_PWM_VOLTAGE_AT_N10T_IDX2    2
#define CALIBRATION_PWM_CURRENT_RATED_IDX3        3
#define  CALIBRATION_PWM_VOLTAGE_FLOAT_IDX4        4
#define  CALIBRATION_RATED_CURRENT_AD_IDX5          5
#endif /* FW_DEFINEPWM_H_ */

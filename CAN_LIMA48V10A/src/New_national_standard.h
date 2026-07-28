/*
 * New_national_standard.h
 *
 *  Created on: 2023年8月25日
 *      Author: Jeffery
 */

#ifndef NEW_NATIONAL_STANDARD_H_
#define NEW_NATIONAL_STANDARD_H_

#include "includeAll.h"

#define NNS_BMS_VOLTAGE_ERR 		1
#define NNS_BMS_CAPACITY_RATING_ERR  	2
#define NNS_BMS_HANDSHAKE_ERR			4
#define NNS_BMS_BATTERY_TYPE_ERR		8
#define NNS_BMS_FORBID_ERR				0x10
#define NNS_BMS_FULL					0x40
#define NNS_BMS_BATTERY_CONNECTED  		0x80


#define NNS_CHARGER_STATE_CC   1
#define NNS_CHARGER_STATE_CV  		2
#define NNS_CHARGER_STATE_TRI	 	4
#define NNS_CHARGER_STATE_FINISHED  8
#define NNS_CHARGER_STATE_D102_OTP	0x10
#define NNS_CHARGER_STATE_ENV_OTP		0x20
#define NNS_CHARGER_STATE_ENV_DERATING			0x40
#define NNS_CHARGER_STATE_NTC_OPEN  0x80
#define NNS_CHARGER_STATE_NTC_SHORT	0x100
#define NNS_CHARGER_STATE_FAN_ERR			0x200
#define NNS_CHARGER_STATE_SCR					0x400
#define NNS_CHARGER_STATE_5V_FAULT				0x800
typedef uint8_t enable1_t ;
typedef uint8_t beTrue_t;


beTrue_t nns_have_prot(void);
beTrue_t nns_is_full(void);
void nns_Response_ex(void);
void nns_deframe_ex(void) ;
void nns_charger_state_all_reset(void);
void nns_charger_state_set(uint32_t new_event);
uint32_t voGet(uint32_t vo_ad);
uint32_t icGet(uint32_t ic_ad);
#endif /* 新国标通信版48V1O8A_NEW_NATIONAL_STANDARD_H_ */

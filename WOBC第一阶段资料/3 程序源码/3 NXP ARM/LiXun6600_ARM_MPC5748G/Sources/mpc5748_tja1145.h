/*
 * s32k142_tja1145.h
 *
 *  Created on: 2019��7��11��
 *      Author: Administrator
 */

#ifndef MPC5748_TJA1145_H_
#define MPC5748_TJA1145_H_

#define TJA1145_INIT     0
#define TJA1145_SLEEP    1
#define TJA1145_STANDBY  2
#define TJA1145_NORMAL   3

extern uint8_t State_TJA1145_MODE;

void Delay(__IO uint32_t nCount);
void TJA1145_SetModel(uint8_t model);
void TJA1145_Init(void);

#endif /* MPC5748_TJA1145_H_ */

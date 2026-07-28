/*
 * MPC5748_gpio.h
 *
 *  Created on: 2019��4��3��
 *      Author: haishengliu
 */

#ifndef MPC5748_GPIO_H_
#define MPC5748_GPIO_H_
#include <main.h>

#define SET_PIN_SELF_LOCK_H()   (PINS_DRV_SetPins(PTI,1<<5))
#define SET_PIN_SELF_LOCK_L()   (PINS_DRV_ClearPins(PTI,1<<5))

#define SET_PIN_S2_CTRL_H()     (PINS_DRV_SetPins(PTB,1<<11))
#define SET_PIN_S2_CTRL_L()     (PINS_DRV_ClearPins(PTB,1<<11))

#define SET_PIN_LED0_TOGLE()     (PINS_DRV_TogglePins(PTF,1<<8))
#define SET_PIN_LED1_TOGLE()     (PINS_DRV_TogglePins(PTF,1<<9))
#define SET_PIN_LED2_TOGLE()     (PINS_DRV_TogglePins(PTF,1<<10))
#define SET_PIN_LED3_TOGLE()     (PINS_DRV_TogglePins(PTF,1<<11))

#define GET_PIN_VCU_OBC_EN()         ((PINS_DRV_ReadPins(PTC) & (1<<14))>>14)

void Gpio_Init(void);

#endif /* MPC5748_GPIO_H_ */

/*
 * MPC5748_gpio.c
 *
 *  Created on: 2019��4��3��
 *      Author: haishengliu
 */
#include <main.h>


void Gpio_Init(void)
{
    /****************** Initial level ******************************/
	SET_PIN_SELF_LOCK_H();
	SET_PIN_S2_CTRL_L();
	SET_PIN_LED0_TOGLE();
	SET_PIN_LED1_TOGLE();
	SET_PIN_LED2_TOGLE();
	SET_PIN_LED3_TOGLE();
}


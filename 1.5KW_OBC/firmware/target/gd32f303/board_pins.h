#ifndef BOARD_PINS_H
#define BOARD_PINS_H

#include "gd32f30x.h"

#define PIN_CV_PWM_PORT          GPIOA
#define PIN_CV_PWM              GPIO_PIN_8
#define PIN_OUTPUT_RELAY_PORT    GPIOA
#define PIN_OUTPUT_RELAY        GPIO_PIN_9

#define PIN_CURRENT_PWM_PORT     GPIOB
#define PIN_CURRENT_PWM          GPIO_PIN_13
#define PIN_LLC_FAULT_PORT       GPIOB
#define PIN_LLC_FAULT            GPIO_PIN_12
#define PIN_FAULT_CLEAR_PORT     GPIOB
#define PIN_FAULT_CLEAR          GPIO_PIN_14
#define PIN_LLC_ENABLE_PORT      GPIOB
#define PIN_LLC_ENABLE           GPIO_PIN_15

#define PIN_RED_LED_PORT         GPIOC
#define PIN_RED_LED              GPIO_PIN_8
#define PIN_GREEN_LED_PORT       GPIOC
#define PIN_GREEN_LED            GPIO_PIN_9
#define PIN_PFC_RELAY_PORT       GPIOC
#define PIN_PFC_RELAY            GPIO_PIN_10
#define PIN_FAN_PORT             GPIOC
#define PIN_FAN                  GPIO_PIN_12

#endif

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/*
 * Production power-stage control is intentionally locked out in the initial
 * firmware. Do not change this value until the low-voltage checklist and
 * schematic net-name conflict are closed by review.
 */
#define APP_ALLOW_POWER_ENABLE 0

#define APP_SELF_TEST_DELAY_MS       100U
#define APP_FAULT_CLEAR_PULSE_MS       2U
#define APP_FAULT_CLEAR_SETTLE_MS      5U

#define APP_PWM_DUTY_MIN_PERMILLE      0U
#define APP_PWM_DUTY_MAX_PERMILLE   1000U

/* Protocol values are intentionally not activated until the interface is agreed. */
#define APP_CAN_ENABLE                 0
#define APP_CAN_BITRATE          1000000U

#endif

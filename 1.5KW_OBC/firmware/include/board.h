#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    BOARD_ADC_AC_VOLTAGE = 0,
    BOARD_ADC_FAN_CURRENT,
    BOARD_ADC_BUS_VOLTAGE,
    BOARD_ADC_CASE_TEMPERATURE,
    BOARD_ADC_OUTPUT_VOLTAGE,
    BOARD_ADC_OUTPUT_CURRENT,
    BOARD_ADC_PFC_MOS_TEMPERATURE,
    BOARD_ADC_BUS_ADJUST_NODE,
    BOARD_ADC_LLC_MOS_TEMPERATURE,
    BOARD_ADC_TRANSFORMER_TEMPERATURE,
    BOARD_ADC_BATTERY_VOLTAGE,
    BOARD_ADC_CHANNEL_COUNT
} board_adc_channel_t;

/* Must be called before any other board initialization. */
void board_early_safe_init(void);
void board_init(void);
void board_force_safe_state(void);

void board_set_pfc_relay(bool on);
void board_set_llc_enable(bool on);
void board_set_output_relay(bool on);
void board_set_fan(bool on);
void board_set_red_led(bool on);
void board_set_green_led(bool on);
void board_set_cv_pwm_permille(uint16_t duty_permille);
void board_set_current_pwm_permille(uint16_t duty_permille);

/* asserted=true drives HARD_FAULT_CLR low. */
void board_set_fault_clear(bool asserted);
bool board_llc_fault_is_active(void);

uint16_t board_adc_read_raw(board_adc_channel_t channel);
uint32_t board_millis(void);
void board_idle(void);

#endif

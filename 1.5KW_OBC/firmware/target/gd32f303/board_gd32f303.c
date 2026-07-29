#include "board.h"

#include "app_config.h"
#include "board_pins.h"
#include "gd32f30x.h"

static volatile uint32_t s_millis;

static void write_pin(uint32_t port, uint32_t pin, bool high)
{
    if (high) {
        gpio_bit_set(port, pin);
    } else {
        gpio_bit_reset(port, pin);
    }
}

static void configure_safe_output(uint32_t port, uint32_t pin, bool high)
{
    write_pin(port, pin, high);
    gpio_init(port, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, pin);
}

void board_early_safe_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);

    configure_safe_output(PIN_CV_PWM_PORT, PIN_CV_PWM, false);
    configure_safe_output(PIN_CURRENT_PWM_PORT, PIN_CURRENT_PWM, false);
    configure_safe_output(PIN_OUTPUT_RELAY_PORT, PIN_OUTPUT_RELAY, false);
    configure_safe_output(PIN_PFC_RELAY_PORT, PIN_PFC_RELAY, false);
    configure_safe_output(PIN_LLC_ENABLE_PORT, PIN_LLC_ENABLE, false);
    configure_safe_output(PIN_FAN_PORT, PIN_FAN, false);
    configure_safe_output(PIN_RED_LED_PORT, PIN_RED_LED, false);
    configure_safe_output(PIN_GREEN_LED_PORT, PIN_GREEN_LED, false);
    configure_safe_output(PIN_FAULT_CLEAR_PORT, PIN_FAULT_CLEAR, true);

    gpio_init(PIN_LLC_FAULT_PORT, GPIO_MODE_IPD, GPIO_OSPEED_2MHZ,
              PIN_LLC_FAULT);
}

static void adc_init_all_channels(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_ADC0);
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);

    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ,
              GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 |
              GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ,
              GPIO_PIN_0 | GPIO_PIN_1);
    gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ,
              GPIO_PIN_4 | GPIO_PIN_5);

    adc_deinit(ADC0);
    adc_mode_config(ADC_MODE_FREE);
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    adc_special_function_config(ADC0, ADC_SCAN_MODE, DISABLE);
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
    adc_enable(ADC0);

    for (volatile uint32_t delay = 0U; delay < 10000U; ++delay) {
        __NOP();
    }
    adc_calibration_enable(ADC0);
}

void board_init(void)
{
    SystemCoreClockUpdate();
    board_force_safe_state();
    adc_init_all_channels();
    (void)SysTick_Config(SystemCoreClock / 1000U);
}

void board_force_safe_state(void)
{
    board_set_pfc_relay(false);
    board_set_llc_enable(false);
    board_set_output_relay(false);
    board_set_fan(false);
    board_set_red_led(false);
    board_set_green_led(false);
    board_set_cv_pwm_permille(0U);
    board_set_current_pwm_permille(0U);
}

void board_set_pfc_relay(bool on)
{
    write_pin(PIN_PFC_RELAY_PORT, PIN_PFC_RELAY, on);
}

void board_set_llc_enable(bool on)
{
    write_pin(PIN_LLC_ENABLE_PORT, PIN_LLC_ENABLE, on);
}

void board_set_output_relay(bool on)
{
    write_pin(PIN_OUTPUT_RELAY_PORT, PIN_OUTPUT_RELAY, on);
}

void board_set_fan(bool on)
{
    write_pin(PIN_FAN_PORT, PIN_FAN, on);
}

void board_set_red_led(bool on)
{
    write_pin(PIN_RED_LED_PORT, PIN_RED_LED, on);
}

void board_set_green_led(bool on)
{
    write_pin(PIN_GREEN_LED_PORT, PIN_GREEN_LED, on);
}

void board_set_cv_pwm_permille(uint16_t duty_permille)
{
    /*
     * Initial firmware deliberately implements 0% as a static low GPIO.
     * Timer setup and the measured CV transfer function belong in a reviewed
     * follow-up change.
     */
    (void)duty_permille;
    write_pin(PIN_CV_PWM_PORT, PIN_CV_PWM, false);
}

void board_set_current_pwm_permille(uint16_t duty_permille)
{
    (void)duty_permille;
    write_pin(PIN_CURRENT_PWM_PORT, PIN_CURRENT_PWM, false);
}

void board_set_fault_clear(bool asserted)
{
    write_pin(PIN_FAULT_CLEAR_PORT, PIN_FAULT_CLEAR, !asserted);
}

bool board_llc_fault_is_active(void)
{
    /* Schematic latch output is treated as active high; verify on LV bench. */
    return gpio_input_bit_get(PIN_LLC_FAULT_PORT, PIN_LLC_FAULT) != RESET;
}

uint16_t board_adc_read_raw(board_adc_channel_t channel)
{
    static const uint8_t adc_channel[BOARD_ADC_CHANNEL_COUNT] = {
        ADC_CHANNEL_1, ADC_CHANNEL_2, ADC_CHANNEL_3, ADC_CHANNEL_4,
        ADC_CHANNEL_5, ADC_CHANNEL_6, ADC_CHANNEL_7, ADC_CHANNEL_8,
        ADC_CHANNEL_9, ADC_CHANNEL_14, ADC_CHANNEL_15
    };

    if ((unsigned)channel >= (unsigned)BOARD_ADC_CHANNEL_COUNT) {
        return 0U;
    }

    adc_regular_channel_config(ADC0, 0U, adc_channel[channel],
                               ADC_SAMPLETIME_55POINT5);
    adc_flag_clear(ADC0, ADC_FLAG_EOC);
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
    while (adc_flag_get(ADC0, ADC_FLAG_EOC) == RESET) {
    }
    return (uint16_t)adc_regular_data_read(ADC0);
}

uint32_t board_millis(void)
{
    return s_millis;
}

void board_idle(void)
{
    __WFI();
}

void SysTick_Handler(void)
{
    s_millis++;
}

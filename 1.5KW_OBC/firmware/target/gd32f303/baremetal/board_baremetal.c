#include "board.h"

#include "gd32f303_minimal.h"

#include <stdbool.h>
#include <stdint.h>

#define GPIO_CFG_ANALOG_INPUT   0x0UL
#define GPIO_CFG_INPUT_PULL     0x8UL
#define GPIO_CFG_OUTPUT_PP_2MHZ 0x2UL
#define ADC_TIMEOUT             100000UL

static volatile uint32_t s_millis;

static void gpio_config(gd32_gpio_t *port, uint8_t pin, uint32_t config)
{
    volatile uint32_t *ctl = (pin < 8U) ? &port->CTL0 : &port->CTL1;
    const uint32_t shift = ((uint32_t)pin & 7UL) * 4UL;
    uint32_t value = *ctl;

    value &= ~(0xFUL << shift);
    value |= (config & 0xFUL) << shift;
    *ctl = value;
}

static void gpio_write(gd32_gpio_t *port, uint8_t pin, bool high)
{
    if (high) {
        port->BOP = GPIO_PIN(pin);
    } else {
        port->BC = GPIO_PIN(pin);
    }
}

static void safe_output(gd32_gpio_t *port, uint8_t pin, bool high)
{
    gpio_write(port, pin, high);
    gpio_config(port, pin, GPIO_CFG_OUTPUT_PP_2MHZ);
}

static void configure_adc_pin(gd32_gpio_t *port, uint8_t pin)
{
    gpio_config(port, pin, GPIO_CFG_ANALOG_INPUT);
}

static bool wait_register_clear(volatile uint32_t *reg, uint32_t mask)
{
    uint32_t timeout = ADC_TIMEOUT;

    while (((*reg & mask) != 0UL) && (timeout > 0UL)) {
        timeout--;
    }
    return timeout > 0UL;
}

void board_early_safe_init(void)
{
    RCU_APB2EN |= RCU_APB2EN_AFEN | RCU_APB2EN_PAEN |
                  RCU_APB2EN_PBEN | RCU_APB2EN_PCEN;

    safe_output(GPIOA, 8U, false);
    safe_output(GPIOA, 9U, false);
    safe_output(GPIOB, 13U, false);
    safe_output(GPIOB, 14U, true);
    safe_output(GPIOB, 15U, false);
    safe_output(GPIOC, 8U, false);
    safe_output(GPIOC, 9U, false);
    safe_output(GPIOC, 10U, false);
    safe_output(GPIOC, 12U, false);

    GPIOB->BC = GPIO_PIN(12U);
    gpio_config(GPIOB, 12U, GPIO_CFG_INPUT_PULL);
}

static void adc_init(void)
{
    RCU_APB2EN |= RCU_APB2EN_ADC0EN;
    RCU_CFG0 = (RCU_CFG0 & ~RCU_CFG0_ADCPSC_MASK) | RCU_CFG0_ADCPSC_DIV8;

    configure_adc_pin(GPIOA, 1U);
    configure_adc_pin(GPIOA, 2U);
    configure_adc_pin(GPIOA, 3U);
    configure_adc_pin(GPIOA, 4U);
    configure_adc_pin(GPIOA, 5U);
    configure_adc_pin(GPIOA, 6U);
    configure_adc_pin(GPIOA, 7U);
    configure_adc_pin(GPIOB, 0U);
    configure_adc_pin(GPIOB, 1U);
    configure_adc_pin(GPIOC, 4U);
    configure_adc_pin(GPIOC, 5U);

    ADC0->CTL0 = 0UL;
    ADC0->CTL1 = ADC_CTL1_ETSRC_SWRCST | ADC_CTL1_ETERC;
    ADC0->RSQ0 = 0UL;
    ADC0->CTL1 |= ADC_CTL1_ADCON;

    for (volatile uint32_t delay = 0UL; delay < 1000UL; ++delay) {
        __asm volatile ("nop");
    }

    ADC0->CTL1 |= ADC_CTL1_RSTCLB;
    (void)wait_register_clear(&ADC0->CTL1, ADC_CTL1_RSTCLB);
    ADC0->CTL1 |= ADC_CTL1_CLB;
    (void)wait_register_clear(&ADC0->CTL1, ADC_CTL1_CLB);
}

void board_init(void)
{
    SystemCoreClockUpdate();
    board_force_safe_state();
    adc_init();

    SYSTICK_LOAD = (SystemCoreClock / 1000UL) - 1UL;
    SYSTICK_VAL = 0UL;
    SYSTICK_CTRL = SYSTICK_CTRL_CLKSOURCE |
                   SYSTICK_CTRL_TICKINT |
                   SYSTICK_CTRL_ENABLE;
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
    gpio_write(GPIOC, 10U, on);
}

void board_set_llc_enable(bool on)
{
    gpio_write(GPIOB, 15U, on);
}

void board_set_output_relay(bool on)
{
    gpio_write(GPIOA, 9U, on);
}

void board_set_fan(bool on)
{
    gpio_write(GPIOC, 12U, on);
}

void board_set_red_led(bool on)
{
    gpio_write(GPIOC, 8U, on);
}

void board_set_green_led(bool on)
{
    gpio_write(GPIOC, 9U, on);
}

void board_set_cv_pwm_permille(uint16_t duty_permille)
{
    (void)duty_permille;
    gpio_write(GPIOA, 8U, false);
}

void board_set_current_pwm_permille(uint16_t duty_permille)
{
    (void)duty_permille;
    gpio_write(GPIOB, 13U, false);
}

void board_set_fault_clear(bool asserted)
{
    gpio_write(GPIOB, 14U, !asserted);
}

bool board_llc_fault_is_active(void)
{
    return (GPIOB->ISTAT & GPIO_PIN(12U)) != 0UL;
}

uint16_t board_adc_read_raw(board_adc_channel_t channel)
{
    static const uint8_t adc_channel[BOARD_ADC_CHANNEL_COUNT] = {
        1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 14U, 15U
    };
    uint32_t timeout = ADC_TIMEOUT;
    uint8_t selected;
    uint32_t shift;

    if ((unsigned)channel >= (unsigned)BOARD_ADC_CHANNEL_COUNT) {
        return 0U;
    }

    selected = adc_channel[channel];
    if (selected <= 9U) {
        shift = (uint32_t)selected * 3UL;
        ADC0->SAMPT1 = (ADC0->SAMPT1 & ~(7UL << shift)) | (5UL << shift);
    } else {
        shift = ((uint32_t)selected - 10UL) * 3UL;
        ADC0->SAMPT0 = (ADC0->SAMPT0 & ~(7UL << shift)) | (5UL << shift);
    }

    ADC0->RSQ2 = selected;
    ADC0->STAT &= ~ADC_STAT_EOC;
    ADC0->CTL1 |= ADC_CTL1_SWRCST;

    while (((ADC0->STAT & ADC_STAT_EOC) == 0UL) && (timeout > 0UL)) {
        timeout--;
    }

    if (timeout == 0UL) {
        return 0U;
    }
    return (uint16_t)(ADC0->RDATA & 0xFFFFUL);
}

uint32_t board_millis(void)
{
    return s_millis;
}

void board_idle(void)
{
    __asm volatile ("wfi");
}

void SysTick_Handler(void)
{
    s_millis++;
}

#include "app.h"

#include "board.h"
#include "can_service.h"
#include "safety_manager.h"

static uint16_t s_adc_raw[BOARD_ADC_CHANNEL_COUNT];

void app_init(void)
{
    safety_manager_init();
    (void)can_service_init();
}

void app_tick_1ms(void)
{
    static uint8_t adc_index;

    safety_manager_tick_1ms();

    /* Read one channel per millisecond to keep this bring-up loop bounded. */
    s_adc_raw[adc_index] = board_adc_read_raw((board_adc_channel_t)adc_index);
    adc_index++;
    if (adc_index >= (uint8_t)BOARD_ADC_CHANNEL_COUNT) {
        adc_index = 0U;
    }

    can_service_poll();
}

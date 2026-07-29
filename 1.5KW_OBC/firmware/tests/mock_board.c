#include "mock_board.h"

#include "board.h"

mock_board_state_t g_mock_board;

void mock_board_reset(void)
{
    g_mock_board = (mock_board_state_t){0};
}

void board_early_safe_init(void) {}
void board_init(void) {}

void board_force_safe_state(void)
{
    g_mock_board.pfc_relay = false;
    g_mock_board.llc_enable = false;
    g_mock_board.output_relay = false;
    g_mock_board.fan = false;
    g_mock_board.safe_state_calls++;
}

void board_set_pfc_relay(bool on) { g_mock_board.pfc_relay = on; }
void board_set_llc_enable(bool on) { g_mock_board.llc_enable = on; }
void board_set_output_relay(bool on) { g_mock_board.output_relay = on; }
void board_set_fan(bool on) { g_mock_board.fan = on; }
void board_set_red_led(bool on) { (void)on; }
void board_set_green_led(bool on) { (void)on; }
void board_set_cv_pwm_permille(uint16_t duty) { (void)duty; }
void board_set_current_pwm_permille(uint16_t duty) { (void)duty; }

void board_set_fault_clear(bool asserted)
{
    g_mock_board.fault_clear_asserted = asserted;
}

bool board_llc_fault_is_active(void)
{
    return g_mock_board.fault_active;
}

uint16_t board_adc_read_raw(board_adc_channel_t channel)
{
    (void)channel;
    return 0U;
}

uint32_t board_millis(void) { return 0U; }
void board_idle(void) {}

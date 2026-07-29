#include <assert.h>
#include <stdio.h>

#include "app_config.h"
#include "mock_board.h"
#include "safety_manager.h"

static void tick_many(unsigned count)
{
    while (count-- > 0U) {
        safety_manager_tick_1ms();
    }
}

static void test_boot_stays_safe(void)
{
    mock_board_reset();
    safety_manager_init();

    assert(safety_manager_get_status().state == SAFETY_STATE_SELF_TEST);
    tick_many(APP_SELF_TEST_DELAY_MS);
    assert(safety_manager_get_status().state == SAFETY_STATE_STANDBY);
    assert(!g_mock_board.pfc_relay);
    assert(!g_mock_board.llc_enable);
    assert(!g_mock_board.output_relay);
}

static void test_power_enable_is_rejected(void)
{
    mock_board_reset();
    safety_manager_init();
    tick_many(APP_SELF_TEST_DELAY_MS);

    assert(!safety_manager_request_power_enable());
    assert(safety_manager_get_status().power_enable_rejected);
    assert(!g_mock_board.pfc_relay);
    assert(!g_mock_board.llc_enable);
    assert(!g_mock_board.output_relay);
}

static void test_fault_latches_and_clears(void)
{
    mock_board_reset();
    safety_manager_init();
    tick_many(APP_SELF_TEST_DELAY_MS);

    g_mock_board.fault_active = true;
    safety_manager_tick_1ms();
    assert(safety_manager_get_status().state == SAFETY_STATE_FAULT_LATCHED);
    assert(safety_manager_get_status().fault_seen);

    g_mock_board.fault_active = false;
    safety_manager_request_fault_clear();
    safety_manager_tick_1ms();
    assert(g_mock_board.fault_clear_asserted);
    assert(safety_manager_get_status().state ==
           SAFETY_STATE_FAULT_CLEAR_PULSE);

    tick_many(APP_FAULT_CLEAR_PULSE_MS);
    assert(!g_mock_board.fault_clear_asserted);
    assert(safety_manager_get_status().state ==
           SAFETY_STATE_FAULT_CLEAR_SETTLE);

    tick_many(APP_FAULT_CLEAR_SETTLE_MS);
    assert(safety_manager_get_status().state == SAFETY_STATE_SELF_TEST);
    assert(!safety_manager_get_status().fault_seen);
}

int main(void)
{
    test_boot_stays_safe();
    test_power_enable_is_rejected();
    test_fault_latches_and_clears();
    puts("safety_manager tests passed");
    return 0;
}

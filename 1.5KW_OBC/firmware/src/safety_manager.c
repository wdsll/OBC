#include "safety_manager.h"

#include "app_config.h"
#include "board.h"

static safety_status_t s_status;
static bool s_clear_requested;

static void enter_state(safety_state_t next)
{
    s_status.state = next;
    s_status.state_time_ms = 0U;
}

static void latch_fault(void)
{
    board_force_safe_state();
    board_set_fault_clear(false);
    s_status.fault_seen = true;
    s_clear_requested = false;
    enter_state(SAFETY_STATE_FAULT_LATCHED);
}

void safety_manager_init(void)
{
    board_force_safe_state();
    board_set_fault_clear(false);

    s_status.state = SAFETY_STATE_BOOT_SAFE;
    s_status.state_time_ms = 0U;
    s_status.fault_seen = false;
    s_status.power_enable_rejected = false;
    s_clear_requested = false;

    if (board_llc_fault_is_active()) {
        latch_fault();
    } else {
        enter_state(SAFETY_STATE_SELF_TEST);
    }
}

void safety_manager_tick_1ms(void)
{
    if (s_status.state_time_ms < UINT32_MAX) {
        s_status.state_time_ms++;
    }

    if ((s_status.state != SAFETY_STATE_FAULT_CLEAR_PULSE) &&
        board_llc_fault_is_active()) {
        latch_fault();
        return;
    }

    switch (s_status.state) {
    case SAFETY_STATE_BOOT_SAFE:
        board_force_safe_state();
        enter_state(SAFETY_STATE_SELF_TEST);
        break;

    case SAFETY_STATE_SELF_TEST:
        board_force_safe_state();
        if (s_status.state_time_ms >= APP_SELF_TEST_DELAY_MS) {
            enter_state(SAFETY_STATE_STANDBY);
        }
        break;

    case SAFETY_STATE_STANDBY:
        board_force_safe_state();
        break;

    case SAFETY_STATE_FAULT_LATCHED:
        board_force_safe_state();
        if (s_clear_requested) {
            s_clear_requested = false;
            board_set_fault_clear(true);
            enter_state(SAFETY_STATE_FAULT_CLEAR_PULSE);
        }
        break;

    case SAFETY_STATE_FAULT_CLEAR_PULSE:
        board_force_safe_state();
        if (s_status.state_time_ms >= APP_FAULT_CLEAR_PULSE_MS) {
            board_set_fault_clear(false);
            enter_state(SAFETY_STATE_FAULT_CLEAR_SETTLE);
        }
        break;

    case SAFETY_STATE_FAULT_CLEAR_SETTLE:
        board_force_safe_state();
        if (s_status.state_time_ms >= APP_FAULT_CLEAR_SETTLE_MS) {
            if (board_llc_fault_is_active()) {
                latch_fault();
            } else {
                s_status.fault_seen = false;
                enter_state(SAFETY_STATE_SELF_TEST);
            }
        }
        break;

    default:
        latch_fault();
        break;
    }
}

void safety_manager_request_fault_clear(void)
{
    if (s_status.state == SAFETY_STATE_FAULT_LATCHED) {
        s_clear_requested = true;
    }
}

bool safety_manager_request_power_enable(void)
{
#if APP_ALLOW_POWER_ENABLE
    /*
     * The initial release deliberately has no start sequence. A later reviewed
     * change must add voltage plausibility, relay timing and LLC enable logic.
     */
    s_status.power_enable_rejected = true;
    return false;
#else
    s_status.power_enable_rejected = true;
    board_force_safe_state();
    return false;
#endif
}

safety_status_t safety_manager_get_status(void)
{
    return s_status;
}

#ifndef SAFETY_MANAGER_H
#define SAFETY_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    SAFETY_STATE_BOOT_SAFE = 0,
    SAFETY_STATE_SELF_TEST,
    SAFETY_STATE_STANDBY,
    SAFETY_STATE_FAULT_LATCHED,
    SAFETY_STATE_FAULT_CLEAR_PULSE,
    SAFETY_STATE_FAULT_CLEAR_SETTLE
} safety_state_t;

typedef struct {
    safety_state_t state;
    uint32_t state_time_ms;
    bool fault_seen;
    bool power_enable_rejected;
} safety_status_t;

void safety_manager_init(void);
void safety_manager_tick_1ms(void);
void safety_manager_request_fault_clear(void);
bool safety_manager_request_power_enable(void);
safety_status_t safety_manager_get_status(void);

#endif

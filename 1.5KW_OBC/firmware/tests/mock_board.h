#ifndef MOCK_BOARD_H
#define MOCK_BOARD_H

#include <stdbool.h>

typedef struct {
    bool pfc_relay;
    bool llc_enable;
    bool output_relay;
    bool fan;
    bool fault_clear_asserted;
    bool fault_active;
    unsigned safe_state_calls;
} mock_board_state_t;

extern mock_board_state_t g_mock_board;
void mock_board_reset(void);

#endif

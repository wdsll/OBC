#include "app.h"
#include "board.h"

int main(void)
{
    uint32_t last_tick;

    board_early_safe_init();
    board_init();
    app_init();

    last_tick = board_millis();
    for (;;) {
        const uint32_t now = board_millis();
        while (last_tick != now) {
            last_tick++;
            app_tick_1ms();
        }
        board_idle();
    }
}

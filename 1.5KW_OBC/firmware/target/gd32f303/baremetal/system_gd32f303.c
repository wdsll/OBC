#include "gd32f303_minimal.h"

#include <stdint.h>

#define IRC8M_CLOCK_HZ 8000000UL
#define HXTAL_CLOCK_HZ 8000000UL
#define HXTAL_TIMEOUT   1000000UL

uint32_t SystemCoreClock = IRC8M_CLOCK_HZ;

extern const void *const g_vector_table[];

void SystemInit(void)
{
    uint32_t timeout = HXTAL_TIMEOUT;

    SCB_VTOR = (uint32_t)(uintptr_t)g_vector_table;
    RCU_CTL |= RCU_CTL_HXTALEN;

    while (((RCU_CTL & RCU_CTL_HXTALSTB) == 0UL) && (timeout > 0UL)) {
        timeout--;
    }

    if ((RCU_CTL & RCU_CTL_HXTALSTB) != 0UL) {
        RCU_CFG0 = (RCU_CFG0 & ~RCU_CFG0_SCS_MASK) | RCU_CFG0_SCS_HXTAL;
        timeout = HXTAL_TIMEOUT;
        while (((RCU_CFG0 & RCU_CFG0_SCSS_MASK) != RCU_CFG0_SCSS_HXTAL) &&
               (timeout > 0UL)) {
            timeout--;
        }
        if ((RCU_CFG0 & RCU_CFG0_SCSS_MASK) == RCU_CFG0_SCSS_HXTAL) {
            SystemCoreClock = HXTAL_CLOCK_HZ;
        }
    }
}

void SystemCoreClockUpdate(void)
{
    if ((RCU_CFG0 & RCU_CFG0_SCSS_MASK) == RCU_CFG0_SCSS_HXTAL) {
        SystemCoreClock = HXTAL_CLOCK_HZ;
    } else {
        SystemCoreClock = IRC8M_CLOCK_HZ;
    }
}

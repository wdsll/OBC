#include <stdint.h>

extern int main(void);
extern void SystemInit(void);
extern void SysTick_Handler(void);

void Reset_Handler(void);
void Default_Handler(void);

void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void) __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));

#if !defined(__ARMCC_VERSION)
extern uint32_t _estack;
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
#define INITIAL_STACK_POINTER ((const void *)&_estack)
#else
extern void __main(void);
extern uint32_t Image$$ARM_LIB_STACK$$ZI$$Limit;
#define INITIAL_STACK_POINTER \
    ((const void *)&Image$$ARM_LIB_STACK$$ZI$$Limit)
#endif

#define D ((const void *)Default_Handler)

__attribute__((used, section("RESET")))
const void *const g_vector_table[] = {
    INITIAL_STACK_POINTER,
    (const void *)Reset_Handler,
    (const void *)NMI_Handler,
    (const void *)HardFault_Handler,
    (const void *)MemManage_Handler,
    (const void *)BusFault_Handler,
    (const void *)UsageFault_Handler,
    0,
    0,
    0,
    0,
    (const void *)SVC_Handler,
    (const void *)DebugMon_Handler,
    0,
    (const void *)PendSV_Handler,
    (const void *)SysTick_Handler,
    D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D,
    D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D,
    D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D,
    D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D,
    D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D
};

void Reset_Handler(void)
{
#if defined(__ARMCC_VERSION)
    SystemInit();
    __main();
#else
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;

    while (dst < &_edata) {
        *dst++ = *src++;
    }

    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0U;
    }

    SystemInit();
    (void)main();
#endif

    for (;;) {
    }
}

void Default_Handler(void)
{
    for (;;) {
    }
}

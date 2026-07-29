#ifndef GD32F303_MINIMAL_H
#define GD32F303_MINIMAL_H

#include <stdint.h>

#define REG32(address) (*(volatile uint32_t *)(address))

#define RCU_BASE                0x40021000UL
#define RCU_CTL                 REG32(RCU_BASE + 0x00UL)
#define RCU_CFG0                REG32(RCU_BASE + 0x04UL)
#define RCU_APB2EN              REG32(RCU_BASE + 0x18UL)

#define RCU_CTL_HXTALEN         (1UL << 16)
#define RCU_CTL_HXTALSTB        (1UL << 17)
#define RCU_CFG0_SCS_MASK       (3UL << 0)
#define RCU_CFG0_SCS_HXTAL      (1UL << 0)
#define RCU_CFG0_SCSS_MASK      (3UL << 2)
#define RCU_CFG0_SCSS_HXTAL     (1UL << 2)
#define RCU_CFG0_ADCPSC_MASK    (3UL << 14)
#define RCU_CFG0_ADCPSC_DIV8    (3UL << 14)

#define RCU_APB2EN_AFEN         (1UL << 0)
#define RCU_APB2EN_PAEN         (1UL << 2)
#define RCU_APB2EN_PBEN         (1UL << 3)
#define RCU_APB2EN_PCEN         (1UL << 4)
#define RCU_APB2EN_ADC0EN       (1UL << 9)

typedef struct {
    volatile uint32_t CTL0;
    volatile uint32_t CTL1;
    volatile uint32_t ISTAT;
    volatile uint32_t OCTL;
    volatile uint32_t BOP;
    volatile uint32_t BC;
    volatile uint32_t LOCK;
} gd32_gpio_t;

#define GPIOA                   ((gd32_gpio_t *)0x40010800UL)
#define GPIOB                   ((gd32_gpio_t *)0x40010C00UL)
#define GPIOC                   ((gd32_gpio_t *)0x40011000UL)

#define GPIO_PIN(n)             (1UL << (n))

typedef struct {
    volatile uint32_t STAT;
    volatile uint32_t CTL0;
    volatile uint32_t CTL1;
    volatile uint32_t SAMPT0;
    volatile uint32_t SAMPT1;
    volatile uint32_t IOFF0;
    volatile uint32_t IOFF1;
    volatile uint32_t IOFF2;
    volatile uint32_t IOFF3;
    volatile uint32_t WDHT;
    volatile uint32_t WDLT;
    volatile uint32_t RSQ0;
    volatile uint32_t RSQ1;
    volatile uint32_t RSQ2;
    volatile uint32_t ISQ;
    volatile uint32_t IDATA0;
    volatile uint32_t IDATA1;
    volatile uint32_t IDATA2;
    volatile uint32_t IDATA3;
    volatile uint32_t RDATA;
} gd32_adc_t;

#define ADC0                    ((gd32_adc_t *)0x40012400UL)
#define ADC_STAT_EOC            (1UL << 1)
#define ADC_CTL1_ADCON          (1UL << 0)
#define ADC_CTL1_CLB            (1UL << 2)
#define ADC_CTL1_RSTCLB         (1UL << 3)
#define ADC_CTL1_ETSRC_MASK     (7UL << 17)
#define ADC_CTL1_ETSRC_SWRCST   (7UL << 17)
#define ADC_CTL1_ETERC          (1UL << 20)
#define ADC_CTL1_SWRCST         (1UL << 22)

#define SYSTICK_CTRL            REG32(0xE000E010UL)
#define SYSTICK_LOAD            REG32(0xE000E014UL)
#define SYSTICK_VAL             REG32(0xE000E018UL)
#define SYSTICK_CTRL_ENABLE     (1UL << 0)
#define SYSTICK_CTRL_TICKINT    (1UL << 1)
#define SYSTICK_CTRL_CLKSOURCE  (1UL << 2)

#define SCB_VTOR                REG32(0xE000ED08UL)

extern uint32_t SystemCoreClock;
void SystemInit(void);
void SystemCoreClockUpdate(void);

#endif

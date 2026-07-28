
#ifndef __BSP_H
#define __BSP_H

#define PWM_CHANNEL_CURRENT     0     /**PWM设置函数，通道选择参数，电流通道**/
#define PWM_CHANNEL_VOLTAGE     1     /**PWM设置函数，通道选择参数，电压通道**/
#define LAST_ADDR_BLOCK  0x0800F000    /*数据保存在最后一块section，*/


#define SLOW_FRE    4764
#define HIGH_FRE       4836
#define USE_BAT_DET
#define USE_CHARGING_CURRENT
//#define PA5_DEBUG
#define RED_LED                 0
#define GREEN_LED                 1

typedef enum {LED_OFF , LED_TWINKLE_2HZ , LED_TWINKLE_1HZ ,  LED_ON } LEDState_T;
extern uint8_t red_led , green_led;
void led_ctr_exec(void);
void LED_Set_State(uint8_t whichLed , LEDState_T newState);
void lcdtest(void);
void num2led(unsigned char n,unsigned char k1,unsigned char k2);
/***
 *
 */
void initAll(void);
//void RCC_Init(void);
void swd_delay_init(void);
/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
void PWM_Init(void) ;
void PWM_set(uint32_t channel , uint32_t pwmval) ;
uint32_t PWM_get(uint32_t channel );
void setTim3Period(uint32_t newPeriod);
void togglePeriod(void) ;
/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
void adc_channel_select(uint32_t channel) ;
uint32_t adc_overFlag(void) ;
void adc_Start(void);
uint32_t adc_get(void);
void adc_overFlagClear(void) ;
/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
void uart_send(char dat) ;
uint32_t uart_get_TxOverFlag(void);
void uart_clear_txOverFlag(void);

/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
#define MYKEY 0x567abc0F
void flashWriteIn(uint32_t dat[] , uint32_t length , uint32_t key);

uint32_t flash_getEraseOverFlag(void);


uint32_t flash_read(uint32_t addr) ;
/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/

void iOPinSet_VRef_Input(void) ;
void iOPinSet_VRef_OutAndLow(void) ;
void iOPinSet_FanOff(void) ;
void iOPinSet_FanOn(void);
uint8_t ioPinGet_fanState(void) ;
void iOPinSet_SCR_off(void) ;
void iOPinSet_SCR_on(void);

void ioPinSet_RedLedOn(void) ;
void ioPinSet_RedLedOff(void) ;
void ioPinSet_GreenLedOn(void);
void ioPinSet_GreenLedOff(void) ;

uint8_t isBatPackTakeAway(void);
uint8_t isBatPackNotOK(void);
uint8_t isBatPackVolOK(void);
/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/
void IWDG_Config(IWDG_CONFIG_PRESCALER IWDG_prescaler, uint16_t reload_value);
void IWDG_Feed(void);


void setPdStopRst(void);
void setPVD(void) ;
void setLvr(void);

void CAN_CONFIG(void);
void can_delay_send(void);
void can_test(void);
uint16_t ADC_GetData(uint8_t ADC_Channel);
#endif

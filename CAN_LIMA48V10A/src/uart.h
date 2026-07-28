#ifndef __ANNEX_H
#define __ANNEX_H


typedef enum{TX_WAITING , TX_QUERY , TX_INVALID}TX_step_T;

typedef struct {
	uint32_t addr;
	uint32_t dat[16];
}WriteFlashFormat_T;


void uart_sendBuffExec(void);
void uart_fillTXBuff(uint8_t * sendDat , uint32_t length);
void uart_deframeRunning(void);
void uart_emptyTxBuff(void) ;
void uart_seriousInto(uint8_t x) ;
void uart2_send(char dat) ;
uint32_t uart2_txOverFlag_get(void);
void nns_rx_data_put(uint8_t newData) ;
void nns_initial_response_fill(void);
uint8_t nns_get_charge_step(void);
uint32_t nns_get_step_time_min(void);
void nns_charger_state_reset(uint32_t event);

/***
 * 0----选择充电电流检测通道
 * 1----选择充电电压检测通道
 * 2----MOS管温度检测通道
 * 3----环境温度检测通道
 * 4----风扇电流检测通道
 */
#define ADC_CHANNEL_QTY 6
#define ADC_CHANNEL_CHARGING_CURRENT    0
#define ADC_CHANNEL_CHARGING_VOLTAGE    1
#define ADC_CHANNEL_TEMP_MOS          2
#define ADC_CHANNEL_TEMP_ENV         3
#define ADC_CHANNEL_FAN_CURRENT   4
#define ADC_CHANNEL_BAT_VOLTAGE   5

uint32_t adcGet(uint32_t  channelSel );
void adcRunning(void);
beTrue_t nns_is_battery_not_connected(void);
beTrue_t nns_is_battery_can_charge(void);
beTrue_t nns_is_battery_in_error(void);
uint8_t get_charging_control_bits(void);
uint8_t get_blind_charge(void);

void reset_charging_control_bits(void);
int ad2temperature(uint32_t adv);
uint8_t isNtcFault(uint32_t adv) ;

void enterCalibration(void) ;
void calibrationPWM_set(uint32_t channel  , uint32_t newVal) ;
void calibrationSaveApply(uint32_t key);

void can_tx_rx(void);

#endif

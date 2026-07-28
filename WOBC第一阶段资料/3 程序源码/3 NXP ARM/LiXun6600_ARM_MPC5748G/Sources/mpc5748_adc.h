/*
 * MPC5748_adc.h
 *
 *  Created on: 2019��4��3��
 *      Author: haishengliu
 */

#ifndef MPC5748_ADC_H_
#define MPC5748_ADC_H_
#include <main.h>

#define ADC_KL30_CHN    6
#define ADC_CCDET_CHN   2
#define ADC_CPDET_CHN   5

#define AD_CH_NUM             (3)
#define AD_FILTER_NUM		      (8)

typedef struct {
	// Secondary MCU ADC Analog
	uint16_t Vkl30_mcu;
	uint16_t Vcc_mcu;
	uint16_t Rcc_mcu;
	uint16_t Vcp_mcu;
	// Primary MCU ADC Analog
	uint16_t Vin_prmy;//Primary
	uint16_t Iin_prmy;
	uint16_t Vbus_prmy;
	uint16_t V18v_prmy;//Primary
	uint16_t V12v_prmy;
	uint16_t Boost_temp_prmy;
}filter_t;
typedef struct {
	uint16_t adcResult[AD_CH_NUM][AD_FILTER_NUM];
	uint16_t adcSum[AD_CH_NUM];
	uint8_t  adcFilterIndex[AD_CH_NUM];
	uint16_t adcResult_SR;
	uint16_t adcResult_Case;
	filter_t analog;
}adc_t;

extern adc_t AdcData;

void Adc_Iint(void);
void ADC_ConvertHandle(void);
#endif /* MPC5748_ADC_H_ */



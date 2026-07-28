/*
 * s32k116_adc.c
 *
 *  Created on: 2019��4��3��
 *      Author: haishengliu
 */
/* Including needed modules to compile this module/procedure */
#include <main.h>

adc_t AdcData;
uint16_t adcRawValue[3];
/*FUNCTION**********************************************************************
 * Function Name : Adc_Iint
 * Description   : Adc_Iint
 END***************************************************************************/
void Adc_Iint(void)
{
  /* Initialize ADC */
  ADC_DRV_Reset(INST_ADCONV1);
  ADC_DRV_DoCalibration(INST_ADCONV1);
  ADC_DRV_ConfigConverter(INST_ADCONV1,&adConv1_ConvCfg0);
  ADC_DRV_EnableChannel(INST_ADCONV1, ADC_CONV_CHAIN_NORMAL, ADC_CCDET_CHN);
  ADC_DRV_EnableChannel(INST_ADCONV1, ADC_CONV_CHAIN_NORMAL, ADC_CPDET_CHN);
  ADC_DRV_EnableChannel(INST_ADCONV1, ADC_CONV_CHAIN_NORMAL, ADC_KL30_CHN);
}

/*FUNCTION**********************************************************************
 * Function Name : Adc_ReadValue_ATDDR0
 * Description   : Read ADC Value
 END***************************************************************************/
void Adc_ReadValue_ATDDR0(void)
{
    ADC_DRV_StartConversion(INST_ADCONV1, ADC_CONV_CHAIN_NORMAL);
    while(!(ADC_DRV_GetStatusFlags(INST_ADCONV1) & ADC_FLAG_NORMAL_ENDCHAIN));
    uint32_t len = ADC_DRV_GetConvResultsToArray(INST_ADCONV1, ADC_CONV_CHAIN_NORMAL, &adcRawValue, 3u);
    ADC_DRV_ClearStatusFlags(INST_ADCONV1, ADC_FLAG_NORMAL_ENDCHAIN);
}
/*FUNCTION**********************************************************************
 * Function Name : ADC_FaultCheck
 * Description   : Read ADC Value and fault handle
 END***************************************************************************/
void ADC_ConvertHandle(void)
{
	uint8_t Chn = 0;

	Adc_ReadValue_ATDDR0();
    /************************* KL30_ADC *****************************************/
	Chn = 0;
    AdcData.adcSum[Chn] -= AdcData.adcResult[Chn][AdcData.adcFilterIndex[Chn]]; //
    AdcData.adcResult[Chn][AdcData.adcFilterIndex[Chn]] = adcRawValue[2];//
    AdcData.adcSum[Chn] += AdcData.adcResult[Chn][AdcData.adcFilterIndex[Chn]];//

	if( ++AdcData.adcFilterIndex[Chn] >= AD_FILTER_NUM )//
	{
		 AdcData.adcFilterIndex[Chn] = 0;
	}
	AdcData.analog.Vkl30_mcu =  (uint16_t)(AdcData.adcSum[Chn]/AD_FILTER_NUM*7.8983);   //0.001V
    /************************* CC_DET_ADC *****************************************/
	Chn = 1;
    AdcData.adcSum[Chn] -= AdcData.adcResult[Chn][AdcData.adcFilterIndex[Chn]]; //
    AdcData.adcResult[Chn][AdcData.adcFilterIndex[Chn]] = adcRawValue[0];//
    AdcData.adcSum[Chn] += AdcData.adcResult[Chn][AdcData.adcFilterIndex[Chn]];//

	if( ++AdcData.adcFilterIndex[Chn] >= AD_FILTER_NUM )//
	{
		 AdcData.adcFilterIndex[Chn] = 0;
	}
	AdcData.analog.Vcc_mcu =  (uint16_t)(AdcData.adcSum[Chn]/AD_FILTER_NUM);   //ADC
	AdcData.analog.Rcc_mcu =  (uint32_t)AdcData.analog.Vcc_mcu*1000/(4095 - AdcData.analog.Vcc_mcu);//Re
    /************************* CP_DET_ADC *****************************************/
	Chn = 2;
    AdcData.adcSum[Chn] -= AdcData.adcResult[Chn][AdcData.adcFilterIndex[Chn]]; //
    AdcData.adcResult[Chn][AdcData.adcFilterIndex[Chn]] = adcRawValue[1];//
    AdcData.adcSum[Chn] += AdcData.adcResult[Chn][AdcData.adcFilterIndex[Chn]];//

	if( ++AdcData.adcFilterIndex[Chn] >= AD_FILTER_NUM )//
	{
		 AdcData.adcFilterIndex[Chn] = 0;
	}
	AdcData.analog.Vcp_mcu =  (uint16_t)(AdcData.adcSum[Chn]/AD_FILTER_NUM);   //ADC

}




















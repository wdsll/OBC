#ifndef DEVICEINIT_28069_H
#define DEVICEINIT_28069_H
#include "DSP28x_Project.h"

#define true   1
#define false  0

#define  SET_PIN_RELAY_EN_H   (GpioDataRegs.GPASET.bit.GPIO24 = 1)
#define  SET_PIN_RELAY_EN_L   (GpioDataRegs.GPASET.bit.GPIO24 = 0)

#define  SET_PIN_TEST_H   (GpioDataRegs.GPASET.bit.GPIO17 = 1)
#define  SET_PIN_TEST_L   (GpioDataRegs.GPASET.bit.GPIO17 = 0)

typedef struct
{
    Uint8   OBC_Tx_Resp;

	Uint8   OBC_RX_LLCONOFF;
	Uint16  OBC_RX_LLCSETVOLT;
	Uint16  OBC_RX_LLCSETCURR;

	Uint16 OBC_TX_PFC_PROTECT;
	Uint16 OBC_TX_PFC_GETVAC;
	Uint16 OBC_TX_PFC_GETIAC;
	Uint16 OBC_TX_ACFreq;
	Uint16 OBC_TX_PROTECT;
	Uint16 OBC_TX_GETVOLT;
	Uint16 OBC_TX_GETCURR;
	Uint8  OBC_TX_PFCT;
	Uint8  OBC_TX_TemperatureAVG;
}OBC_INFO_Data_HV;


extern OBC_INFO_Data_HV OBCinfoData_HV;
extern char cpuTimer_1msFalg;
extern char cpuTimer_20usFlag;

void cpu_timer1_Handler(void); 
void ePWM1_CMPB_INT_Handler(void);
void epwm1_tz3_ISR_Handler(void);
void vEpwm_SetParameter(Uint16 Period, Uint16 Duty,Uint16 TDelay);
Uint16 Get_Epwm1_2_PER(void);
void vEpwm_PinConfig_Enable(void);
void vEpwm_PinConfig_Disable(void);
void vEpwm1Config(void);
void vEpwm2Config(void);
void vEpwm3Config(void);
void vEpwm4Config(void);
void CAN_Init(void);
void CAN_TransmitMsg_Handle(void);
void CAN_ReceiveMsg_Handle(void);
void InitSciBb(void);
void InitCpuTimer0(void);
void GpioInit(void);
void ADC_Init(void);
#endif  // end of DSP28x_PROJECT_H definition








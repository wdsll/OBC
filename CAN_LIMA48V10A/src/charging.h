
#ifndef __CHARGING_H
#define __CHARGING_H

#define ONE_SECOND        100


#define ERROR_FLAG_BITS             0xF0
#define ERROR_FLAG_OVER_TEMP         0x80
#define ERROR_FLAG_FAN_ABNORMAL   0x40
#define ERROR_FLAG_BMS_PROT			0x20
#define BMS_PROT_TIMES   3
typedef enum{MAIN_STEP_WAITING_BATPACK ,  MAIN_STEP_CHARGING , MAIN_STEP_SILICON_OFF , 
							MAIN_STEP_WAITING_TAKE_AWAY , MAIN_STEP_WARING , MAIN_STEP_INVALID}MainStepType_T ;
 
 typedef enum{CHARGING_STEP_PRE , CHARGING_STEP_CCM , CHARGING_STEP_CVM , CHARGING_STEP_TRI ,
	 	 	 	 	 	 	 	 CHARGING_STEP_FINISHED , CHARGING_STEP_INVALID} ChargingStepType_T;
 
 typedef enum{FAN_DET_SCR_OFF , FAN_DET_STOP_1S , FAN_DET_ON_500MS , FAN_DET_IS_OK , FAN_DET_INVALID}FanDet_T;
 
 typedef enum{SCR_OFF_STEP_0 , SCR_OFF_STEP_1 , SCR_OFF_STEP_FINISHED , SCR_OFF_STEP_INVALID}SCROffStepType_T;

 MainStepType_T chargeState_Running(uint32_t flags) ;
 ChargingStepType_T chargeingStep_Running(void);
 void chargeingStep_Set(ChargingStepType_T newstate);
 void chargingStep_Init(void);
 void init_ACinput(void) ;
 uint8_t isOk_ACinput(uint32_t ac);
 void chargeState_Init(void);
 
 void FanRepeatTest_Init(void);
 FanDet_T FanRepeatTest(void);
 int fanBlockJudge(uint8_t fanAd[] , uint8_t size);
 void fanBlockFlageSet(uint8_t newFlag) ;
 uint8_t fanFlagGet(void);
 uint32_t fanBlockOutExe(uint8_t fanAd[] , uint8_t size) ;
 void fanDatPut(uint8_t * pdes , uint8_t newDat) ;
 
 SCROffStepType_T SCROffStepProcess(void);
 void SCROffStepInit(void);
 uint32_t SCROffStep_needLowPower(void);
 
typedef struct {
	uint32_t pwm;
	uint32_t voltageAd;
}CompensatingType_T;

void  CompensatingChargingVoltage(uint32_t *compensatingVoltageAdv , uint32_t *compensatingPWM);
void calibrationRunning(void) ;
uint32_t calibrationPWM_get(uint32_t channel);
ChargingStepType_T chargingStep_get(void);
 #endif

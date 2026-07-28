


#include "includeAll.h"
//风扇被打开
//风扇被关闭
//风扇自动保护
//get风扇状态(关闭  工作  堵转  开路)
static FanDet_T fanDet0 = FAN_DET_SCR_OFF ,fanDet1 = FAN_DET_INVALID;
uint32_t fanCurOffLoad = 0;
static uint32_t maxTimeStayHere = 0;
void FanRepeatTest_Init(void) {
	fanDet0 = FAN_DET_SCR_OFF;
	fanDet1 = FAN_DET_INVALID;
maxTimeStayHere = 0;
}
FanDet_T FanRepeatTest(void) {
	static uint8_t normalCnt = 0;
	static uint8_t hltoggle = 0;
	static uint16_t timeCnt = 0;
	
	int n = 16;

	while(n-- > 0) {
		if(n == 15) {
			maxTimeStayHere++;
		}
        if(fanDet0 == FAN_DET_SCR_OFF) {
            if(fanDet1 != fanDet0) {
                fanDet1 = fanDet0 ;
                SCROffStepInit();
				//
            }
            //if(SCROffStepProcess() != SCR_OFF_STEP_FINISHED) {
            //    return fanDet0;
            //}
            fanDet0 = FAN_DET_STOP_1S;
            continue;
        }

        if(fanDet0 == FAN_DET_STOP_1S) {			
            if(fanDet0 != fanDet1) {
                fanDet1 = fanDet0;
                timeCnt = 0;
            }
            iOPinSet_FanOff();//关风扇
            timeCnt < 60*100 ? timeCnt++ : timeCnt;
            if(timeCnt < 200) {
                return fanDet0;
            }
            
            fanDet0 = FAN_DET_ON_500MS;
            continue;
        }
	
        if(fanDet0 == FAN_DET_ON_500MS) {
            if(fanDet0 != fanDet1) {
                fanDet1 = fanDet0;
                timeCnt = 0;
                normalCnt = 0;
            }
            //iOPinSet_SCR_on();

            timeCnt < 600*100 ? timeCnt++ : timeCnt;
            iOPinSet_FanOn();//开风扇
            if(timeCnt < 3*100) {
                return fanDet0;
            }

            {
                uint32_t fancur;
                int temp = ad2temperature(adcGet(ADC_CHANNEL_TEMP_ENV));
//                uint32_t addtion =0;
                    fancur = adcGet(ADC_CHANNEL_FAN_CURRENT); // 获得风扇电流//判断电流
//                if(temp < 10) {
//                    addtion =30000;
//                }
//                if( isNtcFault( adcGet( ADC_CHANNEL_TEMP_ENV ) ) ){
//                    addtion =0;
//                }
                fanCurOffLoad = fancur;
				if(maxTimeStayHere >= (100*3600)) {
					
				}
                else if(fancur < ADV_FAN_MINI_CURRENT || fanFlagGet()/*fancur > ADV_FAN_BLOCKING_CURRENT_PROTECTED + addtion*//*ADV_FAN_BLOCKING_CURRENT*/) {
                    normalCnt = 0;
                    fanDet0 = FAN_DET_SCR_OFF;  //仍然是异常的
                    if(hltoggle) {
                        setTim3Period(HIGH_FRE);
                    } else {
                        setTim3Period(SLOW_FRE);
                    }
                    hltoggle = !hltoggle;
                    continue;
                }

                normalCnt < 250 ? normalCnt++ : normalCnt;
                if(normalCnt < 100) {
                    return fanDet0;
                }

                fanDet0 = FAN_DET_IS_OK;
                continue;
            }

        }
	
        if(fanDet0 == FAN_DET_IS_OK) {
            if(fanDet0 != fanDet1) {
                fanDet1 = fanDet0;
            }
            return fanDet0;
        }
        
        if(fanDet0 >= FAN_DET_INVALID) {//异常值
            if(fanDet0 != fanDet1) {
                fanDet1 = fanDet0;
            }
            fanDet0 = FAN_DET_STOP_1S;
            continue;
        }
	
        return fanDet0;
	}
	return fanDet0;
}


enum{x0_39 , x40_42,x43_45, x46_48 , x49_51 , x52_54 ,x55_57 , x58_60 , x61_63  , x64_66 , x67_69 , x70_72 , x73_75 , x76_78 , x79_81 ,
x82_84 , x85_87, x88_90 , x91_93 , x94_96 , x97_99 , x100_102 , x103_105 , x106_108 , x109_111 , x112_114 , x115_117 , x118_120 , x121_123 ,
x124_126 , x127_129 , x130_255};
 uint8_t fullFlag = 0;

void fanDatPut(uint8_t * pdes , uint8_t newDat) {
	if(fullFlag == 0) {
		if(newDat < 40) {
			if(pdes[x0_39] < 255 )
				pdes[x0_39] ++;
			else
				fullFlag = 1;
		}
		else if(newDat > 129 ) {
			if(pdes[x130_255] < 255 )
				pdes[x130_255] ++;
			else
				fullFlag = 1;
		} else {
			if(pdes[(newDat - 40) / 3 +1] < 255 )
				pdes[(newDat - 40) / 3 +1]++;
			else
				fullFlag = 1;
		}
	}
}
static uint8_t fanBlockFlag = 0;
void fanBlockFlageSet(uint8_t newFlag) {
	fanBlockFlag = newFlag;
}
uint8_t fanFlagGet(void) {
	return fanBlockFlag;
}

uint32_t fanBlockOutExe(uint8_t fanAd[] , uint8_t size) {
	static uint16_t fanBlockTime = 0;

	if(fanBlockJudge(fanAd , size ) != 0) { //认为不转
		if(fanBlockTime < 60000) {fanBlockTime++;}
		if(fanBlockTime > 6000) {togglePeriod();fanBlockTime = 0;}

		fanBlockFlageSet(1); //持续1s电流无波动, 设置堵转标记,  可能堵转了
	} else {//认为在转
		if(fanBlockTime > 0)
			fanBlockTime --;
		fanBlockFlageSet(0);
	}

	return fanBlockTime;
}
int fanBlockJudge(uint8_t fanAd[] , uint8_t size) {
	static uint8_t fanBlock = 0;
	static uint16_t maxPeriod = 0;
	static uint8_t valley[6] = {1,1,1,1,0,0};
	static uint8_t runningCouter = 0;
	uint8_t i;
	uint8_t zero = 0;

	maxPeriod++;
	if(maxPeriod > 1800) {
		fullFlag = 2;
	}

		if( adcGet(ADC_CHANNEL_FAN_CURRENT) > FAN_BLOCKING_MIN_CURREENT/*16000*//*FAN_WORK_MIN_CURREENT*//*20000*/) {
			extern uint32_t fanCurBuff1[8];
			uint16_t sum = 0;
			if(fullFlag) {uint8_t  rightBand = 200;

sum = valley[0] + valley[1] + valley[2] + valley[3] + valley[4] + valley[5] + fanAd[x0_39];
valley[5] = valley[4] ; 
valley[4] = valley[3];
valley[3] = valley[2];
valley[2] = valley[1];
valley[1] = valley[0];
valley[0] = fanAd[x0_39];
runningCouter++;
if(runningCouter > 10) {
	runningCouter--;
}

				for(i = 0 ; i < size ; i++) {
					if(fanAd[i] == 255) {
						rightBand = 12;
					}
					if(rightBand-- == 0) {
						break;
					}
					if(fanAd[i] == 0) {
						zero++;
					}
				}
				{
					extern uint32_t lostTm;
					lostTm = zero;
				}
				if(fanAd[x0_39] < FAN_VALLEY_COUNT/*6*/ && zero > FAN_ZERO_COUNT/*9*/  && (runningCouter >= 10?sum < FAN_VALLEY_COUNT : 1)) {
					fanBlock = 1;//满足以上条件认为堵转发生
				} else {
					fanBlock= 0;
				}
				if(fullFlag == 2) {
					fanBlock= 0;
				}
				fanCurBuff1[0] = ((uint32_t*)fanAd)[0];
				fanCurBuff1[1] = ((uint32_t*)fanAd)[1];
				fanCurBuff1[2] = ((uint32_t*)fanAd)[2];
				fanCurBuff1[3] = ((uint32_t*)fanAd)[3];
				fanCurBuff1[4] = ((uint32_t*)fanAd)[4];
				fanCurBuff1[5] = ((uint32_t*)fanAd)[5];
				fanCurBuff1[6] = ((uint32_t*)fanAd)[6];
				fanCurBuff1[7] = ((uint32_t*)fanAd)[7];
((uint8_t*)fanCurBuff1)[1] = sum;
				((uint32_t*)fanAd)[0] = 0;
				((uint32_t*)fanAd)[1] = 0;
				((uint32_t*)fanAd)[2] = 0;
				((uint32_t*)fanAd)[3] = 0;
				((uint32_t*)fanAd)[4] = 0;
				((uint32_t*)fanAd)[5] = 0;
				((uint32_t*)fanAd)[6] = 0;
				((uint32_t*)fanAd)[7] = 0;
				fullFlag = 0;
				maxPeriod = 0;
			}
			//维持上次判断
		} else {extern uint32_t fanCurBuff1[8];
			fanCurBuff1[0] = ((uint32_t*)fanAd)[0];
			fanCurBuff1[1] = ((uint32_t*)fanAd)[1];
			fanCurBuff1[2] = ((uint32_t*)fanAd)[2];
			fanCurBuff1[3] = ((uint32_t*)fanAd)[3];
			fanCurBuff1[4] = ((uint32_t*)fanAd)[4];
			fanCurBuff1[5] = ((uint32_t*)fanAd)[5];
			fanCurBuff1[6] = ((uint32_t*)fanAd)[6];
			fanCurBuff1[7] = ((uint32_t*)fanAd)[7];
valley[5] = 0;
valley[4] = 0;
valley[3] = 0;
valley[2] = 0;
valley[1] = 0;
valley[0] = 0;
runningCouter = 0;
			((uint32_t*)fanAd)[0] = 0;
			((uint32_t*)fanAd)[1] = 0;
			((uint32_t*)fanAd)[2] = 0;
			((uint32_t*)fanAd)[3] = 0;
			((uint32_t*)fanAd)[4] = 0;
			((uint32_t*)fanAd)[5] = 0;
			((uint32_t*)fanAd)[6] = 0;
			((uint32_t*)fanAd)[7] = 0;
			fullFlag = 0;
			maxPeriod = 0;
			fanBlock= 0;
		}

		return fanBlock;
//正常运转应该是数据更分散
	//堵转的话会更多集中在7条范围内

}

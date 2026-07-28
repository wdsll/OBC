/*
 * J1772.h
 *
 *  Created on: May 13, 2021
 *      Author: Administrator1
 */

#ifndef J1772_H_
#define J1772_H_


//参考国标GB18487<->j1772
#define jSTATUS_A (0)	//未连接AE
#define jSTATUS_B1 (5)	//准备就绪
#define jSTATUS_B2 (2)	//能量传递
#define jSTATUS_C (1) //能量传递 C D状态
#define jSTATUS_D (6) //能量传递 C D状态
#define jSTATUS_F (4)	//错误状态

typedef struct {
    uint16_t usValueRaw;
    uint16_t usValuePre;
    uint16_t usIsChange;
    uint16_t usAcLimtPer;
}j1772_cc_data_t;
typedef struct {
    uint16_t usFreqRaw;
    uint16_t usDutyRaw;
    uint16_t usValueRaw;
    uint16_t usIsChange;
}j1772_cp_data_t;

typedef struct {
    int16_t sState;
    int16_t sCable;
    int16_t sChgAmp;//0.1A/bit
    int16_t usAcLimtPer;
    int16_t sInMaxI;
    int16_t sIoMaxI;
    union{
    	uint8_t usAll;
    	struct{
    		uint8_t S2E :1;//1 Close 0 Open
    		uint8_t ELOCK :1;//1 Close 0 Open
    		uint8_t Rsv :6;//1 Close 0 Open
    	}bits;
    }xCtl;
}j1772_data_t;

extern j1772_cc_data_t xCc;
extern j1772_cp_data_t xCp;
extern j1772_data_t xJ1772Dt;
extern void vJ1772Init(void);
extern void vJ1772Handle(void); //10ms?

#endif /* J1772_H_ */

#include "includeAll.h"

/***
 * 0xA5 	CMD_TYPE 	DAT1		DAT0		0x5A
 * CMD_TYPE 进入校验模式(0xA1) , 设置低温电压PWM(0xA2) , 设置常温电压PWM(0xA3) , 设置高温电压PWM(0xA4)
 * 设置电流PWM(0xA8)	, 保存数据(0xA9) ,
 * DAT1:数据高8位
 * DAT0：数据低8位
 *0xA5 		0xA1		0x5F		0xF5		0x5A			进入校验，连发2次
 *0xA5 		0xA9 	0x5F		0xF5		0x5A			保存校验数据
 * uart通讯，无crc校验
 * 数据格式：
 *进入校准命令
 *进入电压校准命令
 *进入电流校准命令
 *保存命令
 *当前值提高命令
 *当前值减少命令
 *设置增减步距
 */
/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
static uint8_t rxBuff[32];
static uint8_t rxIdx = 0;
static uint32_t freeTime = 0;
static uint8_t thisFrameHead = 0;
void uart_seriousInto(uint8_t x) {
	freeTime = 0;
	if(rxIdx < 30) {
	  rxBuff[rxIdx++] = x;
	}
}
uint32_t bcd2dec(uint8_t a , uint8_t b) {
	uint32_t x;
	uint32_t y[4];
	y[3] = (a >> 4) & 0x0F ;
	if(y[3] > 9) {
		y[3] = 9;
	}
	y[2] = a & 0x0F;
	if(y[2] > 9) {
		y[2] = 9;
	}

	y[1] = (b >> 4) & 0x0F;
	if(y[1] > 9) {
		y[1] = 9;
	}

	y[0] = b & 0x0F;
	if(y[0] > 9) {
		y[0] = 9;
	}
	x = y[3] * 1000 + y[2] * 100 +	y[1] * 10 + y[0] ;
	return x;
}
void uart_deframeRunning(void) {
		uint32_t key = MYKEY & 0xFFFFFFF0;
		//按雅迪协议先解析,如果符合雅迪的协议
		key |= 1;
				do{
					freeTime++;
					if(rxIdx   >= 5 + thisFrameHead) {
						break;
					}


					if(freeTime >= 3) {
						rxIdx = 0;//没有接收到完整的帧
						thisFrameHead = 0;
						freeTime = 0;
					}
					return;
				}while(0);
		key |= 2;
		ENTER_JUDEG:

	//解析数据

		if(1) {uint8_t cmdType , datH , datL , ending ,head ;

			head = rxBuff[thisFrameHead++];
			cmdType = rxBuff[thisFrameHead++];

			datH =  rxBuff[thisFrameHead++];

			datL = rxBuff[thisFrameHead++];

			ending = rxBuff[thisFrameHead++];

			if(ending != 0x5A || head != 0xA5) {//一帧数据错误
				if(rxIdx >= 5 + thisFrameHead) {
					goto ENTER_JUDEG;
				}else {
					return;
				}
			}
			key |= 4;
			switch (cmdType) {
			case 0xA1://进入校验模式(0xA1)
				if(datH == 0x5F && datL == 0xF5) {
					enterCalibration();//收到进入校准模式命令时
				}
				break;
			case 0xA2://设置低温电压PWM(0xA2)
				calibrationPWM_set(CALIBRATION_PWM_VOLTAGE_AT_N10T_IDX2 , bcd2dec(datH , datL));
				break;
			case 0xA3://设置常温电压PWM(0xA3)
				calibrationPWM_set(CALIBRATION_PWM_VOLTAGE_AT_25T_IDX0 , bcd2dec(datH , datL));
				break;
			case  0xA4://设置高温电压PWM(0xA4)
				calibrationPWM_set(CALIBRATION_PWM_VOLTAGE_AT_45T_IDX1 , bcd2dec(datH , datL));
				break;
			case 0xA6://设置浮充电压PWM
				calibrationPWM_set(CALIBRATION_PWM_VOLTAGE_FLOAT_IDX4 , bcd2dec(datH , datL));

				break;
			case  0xA8://设置电流PWM(0xA8)
				calibrationPWM_set(CALIBRATION_PWM_CURRENT_RATED_IDX3 , bcd2dec(datH , datL));
				break;
			case  0xA9://保存数据(0xA9)
key |= 8;
				calibrationSaveApply(key);//保存校准数据
				break;
			default:
				break;
			}

			if(1) {uint8_t head = 0xA5;//把收到的数据发回去，让发送端自行判断数据是否出错
			uart_emptyTxBuff();
				uart_fillTXBuff(&head , 1);
				uart_fillTXBuff(&cmdType , 1);
				uart_fillTXBuff(&datH , 1);
				uart_fillTXBuff(&datL , 1);
				uart_fillTXBuff(&ending , 1);//FW_RECOGNIZE

				uart_fillTXBuff(&head , 1);
				uart_fillTXBuff(((uint8_t *)&FW_RECOGNIZE) , 1);
				uart_fillTXBuff(((uint8_t *)&FW_RECOGNIZE)+1 , 1);
				uart_fillTXBuff(((uint8_t *)&FW_RECOGNIZE)+2 , 1);
				uart_fillTXBuff(((uint8_t *)&FW_RECOGNIZE)+3 , 1);
				uart_fillTXBuff(&ending , 1);
			}
			if(ending != 0x5A || head != 0xA5) {//一帧数据错误
				if(rxIdx >= 5 + thisFrameHead) {
					goto ENTER_JUDEG;
				}
			}
		}



		//发送校准数据
	//

}
/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
/***
 * uart发送
 * uart_fillTXBuff发送数据的函数
 * uart_sendBuffExec将数据从缓冲发送出去
 */
static char txBuff[64];
static int txIdx = 0;
static uint8_t stepOutIdx = 0;
static TX_step_T txStep0 = TX_WAITING ;
//波特率用9600
void uart_emptyTxBuff(void) {
	txIdx = 0;
	stepOutIdx = 0;
}
void uart_fillTXBuff(uint8_t * sendDat , uint32_t length) {
	int i;
	for(i = 0 ; i < length ; i++) {
		if(txIdx < 60) {
			txBuff[txIdx] = sendDat[i];
			txIdx++;
		}
	}

}
void uart_sendBuffExec(void) {

	if(txStep0 == TX_WAITING) {
		if(stepOutIdx == txIdx) {
			txIdx = 0;
			stepOutIdx = 0;
			return;//退出，无数据要发送
		}
		uart_clear_txOverFlag();//清除发送完成标记
		uart_send(txBuff[stepOutIdx]);//发送数据
		stepOutIdx++;
		txStep0 = TX_QUERY;//到下一步去
	}
	
	if(txStep0 == TX_QUERY) {
		if(uart_get_TxOverFlag() == 0) {//等待数据发送完成标记
			return;
		}
		txStep0 = TX_WAITING;//回到第一步
	}
	
	if(txStep0 >= TX_INVALID) {
		//发生错误
	}
}

/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/



/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

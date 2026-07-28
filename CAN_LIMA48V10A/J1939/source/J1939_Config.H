/*********************************************************************
 *
 *            J1939 Main Source Code
 *
 *********************************************************************
 *
 *	本程序是由XieTongXueFlyMe对现有的J1939协议文档分析，和对前辈的贡献总结,
 * 写出的一套开源的J1939驱动。
 *	本协议特点：
 *		1.易移植（不针对特定的CAN硬件，只要满足CAN2.0B即可）
 *		2.轻量级（可适应低端的MCU）
 *		3.支持多任务调用接口（可用于嵌入式系统）
 *		4.双模式（轮询或者中断，逻辑更加简单明了）
 *		5.不掉帧（数据采用收发列队缓存）
 *
 *  源代码下载：
 *		https://github.com/XeiTongXueFlyMe/J1939
 *  源代码临时手册Web站点：
 *		https://xeitongxueflyme.github.io/j1939doc.github.io/
 *
 * Version     Date        Description
 * -------------------------------------------------------------------
 * v1.0.0     2017/06/04    首个版本 Version 1 测试版发布
 * v1.0.1     2017/08/04    完善功能
 * v1.1.0     2017/11/22    Version 1 稳定发布版
 * v2.0.1     2017/11/24    Version 2 测试版发布
 * v2.0.2     2018/01/03    解决V2.0.1 遗留问题
 * v2.1.0     2018/01/20    Version 2 稳定发布版
 * Author               Date         changes
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *XeiTongXueFlyMe       7/06/04      首个版本
 *XeiTongXueFlyMe       7/08/04      增加对TP的支持
 *XeiTongXueFlyMe       7/11/24      增加对多路CAN硬件的收发，和报文处理
 *XeiTongXueFlyMe       7/11/29      增加请求和响应API
 *XeiTongXueFlyMe       7/12/07      重做TP接受API函数
 *XeiTongXueFlyMe       7/12/08      增加软件滤波器
 *XeiTongXueFlyMe       8/01/03      重做接受发送API，简化协议栈初始化调用逻辑
 **********************************************************************/

#ifndef __J1939_Config_H 
#define __J1939_Config_H 
#include "includeAll.h"
#include "J1939.H"
extern CAN_NODE   Can_Node;   //CAN硬件选择


/***************************J1939 地址配置*****************************/
//设备默认的地址（地址命名是有规定的，参考J1939的附录B 地址和标识符的分配）
#define J1939_STARTING_ADDRESS_1 0x01
#define J1939_STARTING_ADDRESS_2 244
#define J1939_STARTING_ADDRESS_3 247
#define J1939_STARTING_ADDRESS_4 0


/******************************J1939功能配置***************************/
#define J1939_RX_QUEUE_SIZE 3
//当mcu来不及处理消息，接收消息列队是否允许被新的消息覆盖
#define J1939_OVERWRITE_RX_QUEUE J1939_FALSE
#define J1939_TX_QUEUE_SIZE 3
//当mcu来不及处理消息，发送消息列队是否允许被新的消息覆盖
#define J1939_OVERWRITE_TX_QUEUE J1939_FALSE
//是否使用轮询模式（否则使用中断模式）
#define J1939_POLL_ECAN J1939_TRUE
//是否启用软件滤波器
#define J1939SoftwareFilterEn J1939_TRUE
/******************************J1939移植配置函数************************/

#define Port_CAN_Transmit(MsgPtr) J1939_CAN_Transmit(MsgPtr)
#define Port_CAN_Receive(MsgPtr) J1939_CAN_Receive(MsgPtr)
#define Port_SetAddressFilter(Address) J1939_SetAddressFilter(Address)
/*不使用中断模式，不对下面的函数进行移植*/
#if J1939_POLL_ECAN == J1939_FALSE
	#define Port_RXinterruptEnable() J1939_RXinterruptEnable() 
	#define Port_RXinterruptDisable() J1939_RXinterruptDisable() 
	#define Port_TXinterruptEnable() J1939_TXinterruptEnable() 
	#define Port_TXinterruptDisable() J1939_TXinterruptDisable() 
	#define Port_TXinterruptOk() J1939_TXinterruptOk()
	#define Port_CAN_identifier_clc()  CAN_identifier_clc()
#endif 

/***************************************************************************************/

/*
*输入：
*输出： 
*说明：基于SAE J1939协议，我们需要CAN控制器提供至少3个滤波器给J1939协议代码。三个滤波器分别配置如下： 
		1. 设置滤波器0，只接受广播信息（PF = 240 -255）。 
		2. 设置设置滤波器1，2只接受全局地址（J1939_GLOBAL_ADDRESS） 
        3. 随着程序的运行，将改变滤波器2，来适应程序逻辑。
	J1939_SetAddressFilter() 是用来设置滤波器2的， 函数主要设置PS位（目标地址），其目的是，让控制器
	只接受发送给本设备的消息。
*警告： 滤波器0，1是在CAN驱动里配置，如果对硬件滤波配置不是很熟练，可以使能软件滤波器，#define J1939SoftwareFilterEn 
*则可跳过本函数的移植和CAN硬件滤波器的配置，为了J1939协议栈性能最优化，建议只是用硬件滤波。
*/
void J1939_SetAddressFilter(unsigned char Ps_Address)
{
	switch (Can_Node)
	{
		case  Select_CAN_NODE_1:
		{
			break;
		}
		case  Select_CAN_NODE_2:
		{
			break;
		}
		case  Select_CAN_NODE_3:
		{
			break;
		}
		case  Select_CAN_NODE_4:
		{
			break;
		}
		default  :
		{
			break;
		}
	}
}

/*
*输入：  *MsgPtr ，协议要发送的消息，
*输出： 
*说明：      将数据 从MsgPtr结构体赋值到CAN驱动自带的结构体中
		先将传入函数的MsgPtr中的数据写到CAN的结构体，再调用CAN驱动的发送函数
		默认支持4路CAN硬件的收发。如少于4路，只需配置相应的Can_Node开关代码区，
		其他（Select_CAN_NODE）保持不变。就直接返回（break）。
*/

extern CanTxMessage CAN_TxMessage;
void J1939_CAN_Transmit(J1939_MESSAGE *MsgPtr)
{
	  CAN_TxMessage.StdId   = 0x0000;       
    CAN_TxMessage.ExtId   = MsgPtr->Array[0]<<24 + MsgPtr->Array[1]<<16 + MsgPtr->Array[2]<<8 + MsgPtr->Array[3]; //  PF PGN ; PS F4 ; SA 56
    CAN_TxMessage.IDE     = CAN_EXTENDED_ID;         /* CAN_ID_STD / CAN_ID_EXT */
    CAN_TxMessage.RTR     = CAN_RTRQ_DATA;           /* CAN_RTR_DATA / CAN_RTR_REMOTE */
    CAN_TxMessage.DLC     = MsgPtr->Mxe.DataLength;   /* 0 to 8 */
    CAN_TxMessage.Data[0] = MsgPtr->Mxe.Data[0];
    CAN_TxMessage.Data[1] = MsgPtr->Mxe.Data[1];
    CAN_TxMessage.Data[2] = MsgPtr->Mxe.Data[2];
    CAN_TxMessage.Data[3] = MsgPtr->Mxe.Data[3];
    CAN_TxMessage.Data[4] = MsgPtr->Mxe.Data[4];
    CAN_TxMessage.Data[5] = MsgPtr->Mxe.Data[5];
    CAN_TxMessage.Data[6] = MsgPtr->Mxe.Data[6];
    CAN_TxMessage.Data[7] = MsgPtr->Mxe.Data[7];
	  CAN_Transmit_Message_initializes(CAN,&CAN_TxMessage);
}
/*
*输入：     *MsgPtr 数据要存入的内存的指针
*输出：      1 | 0
*说明：      读取CAN驱动的数据，如果没有数据，返回0
		将CAN中的数据取出，存入J1939_MESSAGE结构体中
		默认支持4路CAN硬件的收发。如少于4路，只需配置相应的Can_Node开关代码区，
		其他（Select_CAN_NODE）保持不变。就直接返回（return 0）
*/
extern CanRxMessage CAN_RxMessage;
extern volatile unsigned char can_rx_flag;
int J1939_CAN_Receive(J1939_MESSAGE *MsgPtr)
{

    uint32_t _id=0;
    if ( can_rx_flag)  //XMC_CAN_MO_STATUS_NEW_DATA
    {
			can_rx_flag=0;
      //将29位标志位（can_identifier）写入J1939的结构中
      _id = CAN_RxMessage.ExtId;
      MsgPtr->Array[0] = _id>>(8*3);
      MsgPtr->Array[1] = _id>>(8*2);
      MsgPtr->Array[2] = _id>>(8*1);
      MsgPtr->Array[3] = _id>>(8*0);
      //读取数据长度
      MsgPtr->Mxe.DataLength = CAN_RxMessage.DLC;
      if (MsgPtr->Mxe.DataLength > 8)
          MsgPtr->Mxe.DataLength = 8;
      //读取数据
      MsgPtr->Mxe.Data[0] = CAN_RxMessage.Data[0];
      MsgPtr->Mxe.Data[1] = CAN_RxMessage.Data[1];
      MsgPtr->Mxe.Data[2] = CAN_RxMessage.Data[2];
      MsgPtr->Mxe.Data[3] = CAN_RxMessage.Data[3];
      MsgPtr->Mxe.Data[4] = CAN_RxMessage.Data[4];
      MsgPtr->Mxe.Data[5] = CAN_RxMessage.Data[5];
      MsgPtr->Mxe.Data[6] = CAN_RxMessage.Data[6];
      MsgPtr->Mxe.Data[7] = CAN_RxMessage.Data[7];
      return 1;
    }else
    {
        return 0;//没有消息
    }
}




/*不使用中断模式，不对下面的函数进行移植*/
#if J1939_POLL_ECAN == J1939_FALSE
/*
*输入：
*输出：
*说明：使能接受中断
*/
	void J1939_RXinterruptEnable()
	{
		;
	}
/*
*输入：
*输出：
*说明：失能接受中断
*/
	void J1939_RXinterruptDisable()
	{
		;
	}
/*
*输入：
*输出：
*说明：使能发送中断
*/
	void J1939_TXinterruptEnable()
	{
		;
	}
/*
*输入：
*输出：
*说明：失能发送中断
*/
	void J1939_TXinterruptDisable()
	{
		;
	}
/*
*输入：
*输出：
*说明：触发发送中断标致位，当协议栈在中断模式下，要发送消息，将调用此函数
	CAN驱动函数，就将直接把消息发送出去，不需要协议在调用任何can驱动函数
*/
	void J1939_TXinterruptOk()
	{
		;
	}
/*
*输入：
*输出：
*说明：清除CAN驱动相关的中断产生标识位，包括（发送中断标志位，接受中断标
	志位，can总线错误标识位）
*/
	void CAN_identifier_clc()
	{
		;
	}
#endif

#endif

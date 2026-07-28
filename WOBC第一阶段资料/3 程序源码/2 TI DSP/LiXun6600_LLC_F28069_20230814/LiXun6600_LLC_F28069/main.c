//###########################################################################
//
// FILE:    Example_2806xGpioSetup.c
//
// TITLE:   GPIO Setup Example
//
//! \addtogroup f2806x_example_list
//! <h1>GPIO Setup (gpio_setup)</h1>
//!
//! This example Configures the 2806x GPIO into two different configurations
//! This code is verbose to illustrate how the GPIO could be setup.In a real
//! application, lines of code can be combined for improved code size and
//! efficiency.
//!
//! This example only sets-up the GPIO.. nothing is actually done with
//! the pins after setup.
//!
//! In general:
//!   - All pullup resistors are enabled.  For ePWMs this may not be desired.
//!   - Input qual for communication ports (eCAN, SPI, SCI, I2C) is asynchronous
//!   - Input qual for Trip pins (TZ) is asynchronous
//!   - Input qual for eCAP and eQEP signals is synch to SYSCLKOUT
//!   - Input qual for some I/O's and interrupts may have a sampling window
//
//
//###########################################################################
// $TI Release:  $
// $Release Date:  $
// $Copyright:
// Copyright (C) 2009-2023 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the
//   distribution.
//
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//###########################################################################

//
// Included Files
//
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "DeviceInit_28069.h" 
#include <math.h>
#include <string.h>

#pragma CODE_SECTION(main_app,"ramfuncs")
#pragma CODE_SECTION(InitSysCtrl,"ramfuncs")

typedef unsigned char UINT08;
typedef unsigned short UINT16;
typedef unsigned long UINT32;
typedef signed short SINT16;
typedef signed long SINT32;

//Version number
#define FirmWareREV       0X01
#define HardwareREV       0X01
//Full Scale,Real Value while AD = 4095
#define VoutFullScale     6473//6541                                                  //157.8V Vout real value while AD = 4095
#define IoutFullScale     755//710//710                                                   //64.5A Iout real value while AD = 3722
#define VbatOffsetAD      2048                                                  //6501V Vbat real value while AD = 4095, -6501V Vbat real value while AD = 0,
#define VbatFullScale     6501
//Limit Set Max Real value
#define PoutMaxHighLine   6600
#define PoutMaxLowLine    3300
#define IoutSetMax        170
#define IoutSetMin        20                                                   
#define VoutSetMax        4800
#define VoutSetMin        3200
//Protect real value
#define VbatUVPSlow       3100
#define VbatUVPRecover    3150
#define VoutUVPSlow       3100
#define VoutUVPFast       2400
#define VoutOVPSlow       4900
#define VoutOVPFast       5100
#define VoutOVPRecover    4850
#define IoutOCPSlow       200
#define IoutOCPFast       250

#define TemperatureOTP    (82 + 40)                                             //80 degree
#define TemperatureOTR    (75 + 40)                                             //70 degree
#define TemperatureAlarmLowLine  (70 + 40)
#define TemperatureAlarmHighLine (75 + 40)
#define PoutDeratingStep  600                                                   //Pout derate 100W/degree while Temperature > TemperatureAlarm

//LLC OFF delay time
#define RETRYTIME         3000                                                   //500ms
//PWM related define
#define PWMPeriodMax      750//   120k  //720                                                  // 125kHz
#define PWMPeriodMin      360                                                  // 250kHz
#define PWMDeadtime       18                                                   // deadtime =200ns
//LLC Portect Code    
#define PC_PFCOFF         0x0001
#define PC_VOUTUVP        0x0002
#define PC_VOUTOVP        0x0004
#define PC_IOUTOCP        0x0008
#define PC_PFCUART        0x0010
#define PC_LLCOTP         0x0020
#define PC_FANFULT        0x0040
#define PC_LLCOFF         0x0080
#define PC_POLARERR       0x0100
#define PC_VBATUVP        0//0x0200
#define PC_IPRICMP        0x0400
#define PC_ARMUART        0x0800
#define PC_SOFTRESET      0x1000
#define PC_LLCERR         0x8000
//Variable send to ARM
static UINT16 PROTECT; //PSFB Protect Code
static UINT16 GETVOLT; //Report Output Voltage, 0.1V/bit
static UINT16 GETCURR; //Report Output Current, 0.1A/bit
static UINT08 Temperature; //Report Temperature, offset 40D

//Variable received from PFC, then send to ARM
static UINT16 PFC_PROTECT;
static UINT16 PFC_GETVAC;
static UINT16 PFC_GETIAC;
static UINT16 PFC_ACFreq;
static UINT08 PFCT;
static UINT08 UART1RXSUCCESS = 0;
//Variable received from ARM
static UINT08 LLCONOFF;
static UINT16 SETVOLT;
static UINT16 SETCURR;

//Variable send to PFC
static UINT08 PFCONOFF;




//IO                                      //
#define PFC_READY     (GpioDataRegs.GPADAT.bit.GPIO17) //read input pin

#define RELAY_EN()        (GpioDataRegs.GPASET.bit.GPIO12 = 1)
#define RELAY_DISEN()      (GpioDataRegs.GPACLEAR.bit.GPIO12 = 1)
#define Coil_RELAY_PickUp_EN()        {GpioDataRegs.GPASET.bit.GPIO20 = 1;GpioDataRegs.GPASET.bit.GPIO21 = 1;}
#define Coil_RELAY_PickUp_DISEN()      {GpioDataRegs.GPACLEAR.bit.GPIO20 = 1;GpioDataRegs.GPACLEAR.bit.GPIO21 = 1;}

#define Coil_RELAY_Hold_EN()        {GpioDataRegs.GPASET.bit.GPIO18 = 1;}
#define Coil_RELAY_Hold_DISEN()        {GpioDataRegs.GPACLEAR.bit.GPIO18 = 1;}

#define DEBUG_PIN_high()  (GpioDataRegs.GPBSET.bit.GPIO57 = 1)
#define DEBUG_PIN_low()  (GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1)

//ADC Buffer define
static UINT16 ADCBUF[6];
#define VOUT_ADCBUF       (ADCBUF[0])
#define IOUT_ADCBUF       (ADCBUF[2])
#define IOUTDIS_ADCBUF    (ADCBUF[1])
#define VBUS_ADCBUF       (ADCBUF[3])
#define VBAT_ADCBUF       (ADCBUF[4])
#define TEMP_ADCBUF       (ADCBUF[5])

//Temperature Table
static UINT16 TEMPTABLE[170] = {
159,168,179,190,201,213,226,239,252,267,
282,297,313,330,348,366,385,405,425,446,
468,490,514,538,563,588,615,642,670,698,
727,758,788,820,852,885,918,952,987,1022,
1058,1094,1131,1169,1207,1245,1283,1322,1362,1401,
1441,1481,1522,1562,1603,1644,1684,1725,1766,1806,
1847,1887,1928,1968,2008,2048,2087,2126,2165,2203,
2241,2279,2317,2353,2390,2426,2461,2496,2531,2565,
2599,2632,2664,2696,2727,2758,2788,2818,2847,2876,
2904,2932,2958,2985,3011,3036,3061,3085,3109,3132,
3155,3177,3198,3220,3240,3261,3280,3300,3318,3337,
3355,3372,3389,3406,3422,3438,3453,3468,3483,3497,
3511,3525,3538,3551,3564,3576,3588,3600,3611,3622,
3633,3643,3654,3664,3674,3683,3692,3701,3710,3719,
3727,3735,3743,3751,3759,3766,3773,3780,3787,3794,
3800,3806,3813,3819,3824,3830,3836,3841,3846,3852,
3857,3862,3866,3871,3876,3880,3884,3889,3893,3897
};


static UINT16 ONTIMECNT, OFTIMECNT;

static UINT16 GETBAT,GETCURRDIS,GETVBUS;
static UINT16 VOUT_ADCBUF_AVG,IOUT_ADCBUF_AVG,IOUTDIS_ADCBUF_AVG,VBUS_ADCBUF_AVG,VBAT_ADCBUF_AVG,TEMP_ADCBUF_AVG;


static UINT32 SETPOWER, LMTPOWER;
static UINT32 SETVOLTCURT, LMTVOLT;
static UINT32 SETCURRCURT, LMTCURR;

static SINT32 CURTPeriod;
static SINT32 DELTCV, CURTERRORCV, LASTERRORCV;
static SINT32 DELTCC, CURTERRORCC, LASTERRORCC;
static SINT32 DebugBUF;

static UINT08 UART1TXBUF[4], UART1RXBUF[15];
static UINT08 UART1TXCNT, UART1RXCNT, UART1TXFG, UART1RXFG;
static UINT16 UART1TXTIMECNT, UART1RXOTIMEPCNT, UART1RXIDLECNT;

static UINT32 KVout_RealToAD, KVout_ADToReal, KIout_RealToAD, KIout_ADToReal, KVbat_ADToReal, KVbat_RealToAD;
static UINT32 VoutSetMax_AD, VoutSetMin_AD, IoutSetMax_AD, IoutSetMin_AD;
static UINT16 VoutUVPFast_AD, VoutOVPFast_AD, IoutOCPFast_AD;

static UINT08 RelayOnOffFlag;
static UINT16 KVbat_ADToVout_AD;
static UINT08 PWMUpdateFlag = 0;
static UINT08 PWMEnableFlag = 1;
static UINT08 BurstModeFlag = 0;

static UINT32 TD;
static UINT08 sciCheckSum = 0;
static UINT08 TSERCHL, TSERCHR, TSERCHM;

void main_app(void);

void ParameterInit(void)
{
    UINT32 tmp,a,b,c;
    tmp = 4095;
    KVout_RealToAD = (tmp << 10) / VoutFullScale;
    a = VoutFullScale;
    KVout_ADToReal = (a << 10) / 4095;
    KIout_RealToAD = (tmp << 10) / IoutFullScale;
    b = IoutFullScale;
    KIout_ADToReal = (b << 10) / 4095;
    c = VbatFullScale;
    KVbat_ADToReal = (c << 10) / 2047;
    KVbat_RealToAD = (tmp << 10) / VbatFullScale;

    VoutSetMax_AD = (VoutSetMax * KVout_RealToAD) >> 10;
    VoutSetMin_AD = (VoutSetMin * KVout_RealToAD) >> 10;
    IoutSetMax_AD = (IoutSetMax * KIout_RealToAD) >> 10;
    IoutSetMin_AD = (IoutSetMin * KIout_RealToAD) >> 10;

    VoutOVPFast_AD = (VoutOVPFast * KVout_RealToAD) >> 10;
    VoutUVPFast_AD = (VoutUVPFast * KVout_RealToAD) >> 10;
    IoutOCPFast_AD = (IoutOCPFast * KIout_RealToAD) >> 10;

    KVbat_ADToVout_AD = (KVbat_ADToReal * KVbat_RealToAD) >> 10;
    PROTECT = 0;
    PROTECT |= PC_LLCERR;
    PROTECT |= PC_LLCOFF;

    ONTIMECNT = 0;
    OFTIMECNT = 0;
    CURTPeriod = PWMPeriodMin;
    TSERCHL = 0;
    TSERCHR = 169;
    Temperature = 25 + 40;

    SETVOLT = 3200;
    SETCURR = 20;

    PFC_PROTECT = 0;
    PFC_GETVAC = 0;
    PFC_GETIAC = 0;
    PFC_ACFreq = 0;
    PFCT = 0;
    Coil_RELAY_PickUp_EN();
    Coil_RELAY_Hold_EN();
}

void Scib_ReceiveData(void)
{
    if (ScibRegs.SCIFFRX.bit.RXFFST == 1) {
        UART1RXBUF[UART1RXCNT] = ScibRegs.SCIRXBUF.all;
        if (UART1RXCNT < 12) {
            UART1RXCNT++;
        }
        UART1RXIDLECNT = 5;
    }   
}

void Scib_TxRxHandle(void)//period 1ms
{
    UART1TXTIMECNT++;
    if (UART1TXTIMECNT > 50) {
        UART1TXTIMECNT = 0;
        UART1TXBUF[0] = 0xFF;
        UART1TXBUF[1] = 0x55;
        UART1TXBUF[2] = PFCONOFF;
        UART1TXBUF[3] = 0x54 + UART1TXBUF[2];
        UART1TXFG = 1;
    }

    if (UART1RXOTIMEPCNT > 0) {
        UART1RXOTIMEPCNT--;
    } else {
        PROTECT |= PC_PFCUART;
    }

    if (UART1RXIDLECNT > 0) {
        UART1RXIDLECNT--;
        UART1RXFG = 1;
    } else {
        if (UART1RXFG == 1) {
            UART1RXFG = 0;
            if ((UART1RXCNT == 12) && (UART1RXBUF[0] == 0xFF) && (UART1RXBUF[1] == 0x55))
            {
                sciCheckSum = (UART1RXBUF[0] + UART1RXBUF[1] + UART1RXBUF[2] + UART1RXBUF[3] + UART1RXBUF[4] + UART1RXBUF[5] + UART1RXBUF[6] + UART1RXBUF[7] +
                           UART1RXBUF[8] + UART1RXBUF[9] + UART1RXBUF[10]);
                sciCheckSum &= 0x00FF;
                if (sciCheckSum == UART1RXBUF[11])
                {
                    UART1RXOTIMEPCNT = 500;
                    PFC_PROTECT = ((UINT16)UART1RXBUF[2] << 8) + (UINT16)UART1RXBUF[3];
                    PFC_GETVAC = ((UINT16)UART1RXBUF[4] << 8) + (UINT16)UART1RXBUF[5];
                    PFC_GETIAC = ((UINT16)UART1RXBUF[6] << 8) + (UINT16)UART1RXBUF[7];
                    PFC_ACFreq = ((UINT16)UART1RXBUF[8] << 8) + (UINT16)UART1RXBUF[9];
                    PFCT = UART1RXBUF[10];
                    UART1RXSUCCESS = (UART1RXSUCCESS < 250)?(UART1RXSUCCESS + 1):(0);
                }
            }
            UART1RXCNT = 0;
            UART1RXBUF[0] = 0;
            UART1RXBUF[1] = 0;
        }
    }
}

void Temperature_Handle(void)
{
    ///////////////////////////////////////////////////////Temperature transform

    static UINT16 TSERCHND;
    if (TSERCHL < TSERCHR) {
        TSERCHM = (TSERCHL + TSERCHR) >> 1;
        if (TSERCHM > 169) {
            TSERCHM = 169;
        } else if (TSERCHM < 1) {
            TSERCHM = 0;
        }
        if (TSERCHND < TEMPTABLE[TSERCHM]) {
            TSERCHR = TSERCHM - 1;
        } else {
            TSERCHL = TSERCHM + 1;
        }
    } else {
        Temperature = TSERCHM;
        TSERCHL = 0;
        TSERCHR = 169;
        TSERCHND = TEMP_ADCBUF_AVG;
    }    
}

void Period_1ms_TaskHandle(void)//period 1ms
{

    static UINT16 Counter_Coil_RLY=0;
    if(Counter_Coil_RLY<205){
        Counter_Coil_RLY++;
    }
    if(Counter_Coil_RLY == 200){

        Coil_RELAY_PickUp_DISEN();
    }
    static UINT08 Counter_20ms;
    Counter_20ms++;
    if(Counter_20ms > 19){
        Counter_20ms = 0;
        GETVOLT = (UINT32)(VOUT_ADCBUF_AVG * KVout_ADToReal) >> 10;
        GETCURR = (UINT32)(IOUT_ADCBUF_AVG * KIout_ADToReal) >> 10;
        GETCURRDIS = (UINT32)(IOUTDIS_ADCBUF_AVG * KIout_ADToReal) >> 10;
        GETBAT = (UINT32)(VBAT_ADCBUF_AVG * KVbat_ADToReal) >> 10;

    /////////////////////////////////////////////LLC Max Output Power VS AC voltage and Temperature
        static UINT16 PoutMaxTmp,PoutMaxValue,CurrCalibration;

        if(PFC_GETVAC < 1600){
            PoutMaxTmp = (UINT16) (PFC_GETVAC * 2.8);
            PoutMaxValue = PoutMaxLowLine;
            CurrCalibration = 4;
        }else if(PFC_GETVAC > 1700){
            PoutMaxTmp = (UINT16) (PFC_GETVAC * 3.04);
            PoutMaxValue = PoutMaxHighLine;
            CurrCalibration = 0;
        }

        if (PoutMaxTmp > PoutMaxValue) {
            PoutMaxTmp = PoutMaxValue;
        }

        SETPOWER = PoutMaxTmp;

        static UINT08 TemperatureAlarmTemp;
        TemperatureAlarmTemp = TemperatureAlarmHighLine;
        if (Temperature >= TemperatureAlarmTemp) {
            LMTPOWER = ((Temperature - TemperatureAlarmTemp) * PoutDeratingStep);
            if (LMTPOWER > PoutMaxTmp - 400) {
                LMTPOWER = PoutMaxTmp - 400;
            }
            LMTPOWER = PoutMaxTmp - LMTPOWER;
        } else {
            LMTPOWER = PoutMaxTmp;
        }
        if (SETPOWER > LMTPOWER) {
            SETPOWER = LMTPOWER;
        }

        LMTCURR = ((SETPOWER * 100) / (GETVOLT + 1)) - CurrCalibration;

        if (LMTCURR > SETCURR) {
            LMTCURR = SETCURR;
        }else if(LMTCURR < IoutSetMin){
            LMTCURR = IoutSetMin;
        }
        LMTCURR = ((LMTCURR * KIout_RealToAD) >> 10); //77
    }

    ///////////////////////////////////////////////////////////////////Slow PROTECT
    //////////////////////////////////////////////////////LLC ON OFF detect
    if (LLCONOFF == 0) {
        PROTECT |= PC_LLCOFF;
    }

    //////////////////////////////////////////////////////Over Temperature Protect
    if (Temperature >= TemperatureOTP) {
        PROTECT |= PC_LLCOTP;
    }
    ////////////////////////////////////////////////////////////////////PC_POLARERR
    if (VBAT_ADCBUF < 1800) {
        PROTECT |= PC_POLARERR;
    }
    ///////////////////////////////////////////////////////VOUT slow OVP
    static UINT08 TmpCNT1;
    if (GETVOLT > VoutOVPSlow) {
        if (TmpCNT1 < 100) {
            TmpCNT1++;
        } else {
            PROTECT |= PC_VOUTOVP;
        }
    } else {
        TmpCNT1 = 0;
    }
    ///////////////////////////////////////////////////////VOUT slow UVP
    static UINT08 TmpCNT2;
    if ((GETVOLT < VoutUVPSlow)&&(ONTIMECNT >= 499)&&(SETVOLTCURT == LMTVOLT)) {
        if (TmpCNT2 < 200) {
            TmpCNT2++;
        } else {
            PROTECT |= PC_VOUTUVP;
        }
    } else {
        TmpCNT2 = 0;
    }
    ///////////////////////////////////////////////////////IOUT slow OCP 
    static UINT08 TmpCNT3;
    if (GETCURR > IoutOCPSlow) {
        if (TmpCNT3 < 200) {
            TmpCNT3++;
        } else {
            PROTECT |= PC_IOUTOCP;
        }
    } else {
        TmpCNT3 = 0;
    }
////////////////////////////////////////////////////////soft reset
    if (PROTECT & PC_SOFTRESET) {
        vEpwm_PinConfig_Disable();
        vEpwm_SetParameter(PWMPeriodMin,PWMPeriodMin>>1,0);
        asm("RESET");
    }


    /////////////////////////////////////////////////LLC ON OFF Control
    if (PROTECT) {
        ONTIMECNT = 0;
        if (OFTIMECNT < RETRYTIME) {
            OFTIMECNT++;
        }
        if (OFTIMECNT == 1) {
            /*vEpwm_PinConfig_Disable();
            CURTPeriod = PWMPeriodMin;
            TD = 1;
            PWMUpdateFlag = 1;*/
            PROTECT |= PC_LLCERR;
            GpioDataRegs.GPASET.bit.GPIO16 = 1;
        }
        if (OFTIMECNT == 2) {
            RELAY_DISEN();
        }
        if (OFTIMECNT == 35) {
            RelayOnOffFlag = 0;
        }
        if (OFTIMECNT >= RETRYTIME) {
            /////////////////////////////////////////////////////////////////LLC FAULT Recover                                    
            if (PFC_READY == 1) {
                PROTECT &= ~PC_PFCOFF;
            }
            PROTECT &= ~PC_VOUTUVP;
            if (GETVOLT < VoutOVPRecover) {
                PROTECT &= ~PC_VOUTOVP;
            }
            PROTECT &= ~PC_IOUTOCP;
            if (UART1RXOTIMEPCNT > 0) {
                PROTECT &= ~PC_PFCUART;
            }
            if (Temperature <= TemperatureOTR) {
                PROTECT &= ~PC_LLCOTP;
            }
            PROTECT &= ~PC_FANFULT;
            if (LLCONOFF == 1) {
                PROTECT &= ~PC_LLCOFF;
                PFCONOFF = 1;
            } else {
                PFCONOFF = 0;
            }
            if (VBAT_ADCBUF > 1900) {
                PROTECT &= ~PC_POLARERR;
            }
            PROTECT &= ~PC_IPRICMP;
            EALLOW;
            EPwm1Regs.TZCLR.bit.OST = 1;
            EPwm2Regs.TZCLR.bit.OST = 1;
            EPwm3Regs.TZCLR.bit.OST = 1;
            EPwm4Regs.TZCLR.bit.OST = 1;
            EDIS;
            PROTECT &= ~PC_LLCERR;
        }
    } else {

        OFTIMECNT = 0;
        if (ONTIMECNT < 500) {
            ONTIMECNT++;
        }

        ////////////////////////////////////////////////////////////LLC Voltage and Current Softstart
        if (ONTIMECNT >= 499) {
            if (SETVOLTCURT < LMTVOLT - 15) {
                SETVOLTCURT = SETVOLTCURT + 15;
            } else if (SETVOLTCURT > LMTVOLT + 15) {
                SETVOLTCURT = SETVOLTCURT - 15;
            } else {
                SETVOLTCURT = LMTVOLT;
                if (SETCURRCURT < LMTCURR - 1) {
                    SETCURRCURT = SETCURRCURT + 1;
                } else if (SETCURRCURT > LMTCURR + 1) {
                    SETCURRCURT = SETCURRCURT - 1;
                } else {
                    SETCURRCURT = LMTCURR;
                }
            }
        }
    }

    ///////////////////////////////////////////////LLC Voltage Set Value Limit
    if (SETVOLT > VoutSetMax) {
        SETVOLT = VoutSetMax;
    } else if (SETVOLT < VoutSetMin) {
        SETVOLT = VoutSetMin;
    }
    LMTVOLT = (SETVOLT * KVout_RealToAD) >> 10;
    /////////////////////////////////////////////LLC Current Set Value Limit
    if (SETCURR > IoutSetMax) {
        SETCURR = IoutSetMax;
    }
    if (SETCURR < IoutSetMin) {
        SETCURR = IoutSetMin;
    }


}


void CAN_TransmitMsg_Handle(void)//Response transmit
{
     if(OBCinfoData_HV.OBC_Tx_Resp) ;
     else return;
     OBCinfoData_HV.OBC_Tx_Resp = false;

    ECanaMboxes.MBOX0.MSGCTRL.bit.DLC = 8;
    // Write to the mailbox RAM field of MBOX0 - 3
    OBCinfoData_HV.OBC_TX_PFC_PROTECT = PFC_PROTECT;//PFC protect
    OBCinfoData_HV.OBC_TX_PFC_GETVAC = PFC_GETVAC;//0.1V/Div
    OBCinfoData_HV.OBC_TX_PFC_GETIAC = PFC_GETIAC;//0.1A/Div
    OBCinfoData_HV.OBC_TX_ACFreq = PFC_ACFreq;//0.1Hz/Div
    ECanaMboxes.MBOX0.MDL.byte.BYTE0 = (Uint8)(OBCinfoData_HV.OBC_TX_PFC_PROTECT >> 8);
    ECanaMboxes.MBOX0.MDL.byte.BYTE1 = (Uint8)(OBCinfoData_HV.OBC_TX_PFC_PROTECT);
    ECanaMboxes.MBOX0.MDL.byte.BYTE2 = (Uint8)(OBCinfoData_HV.OBC_TX_PFC_GETVAC >> 8);
    ECanaMboxes.MBOX0.MDL.byte.BYTE3 = (Uint8)(OBCinfoData_HV.OBC_TX_PFC_GETVAC);
    ECanaMboxes.MBOX0.MDH.byte.BYTE4 = (Uint8)(OBCinfoData_HV.OBC_TX_PFC_GETIAC >> 8);
    ECanaMboxes.MBOX0.MDH.byte.BYTE5 = (Uint8)(OBCinfoData_HV.OBC_TX_PFC_GETIAC);
    ECanaMboxes.MBOX0.MDH.byte.BYTE6 = (Uint8)(OBCinfoData_HV.OBC_TX_ACFreq >> 8);
    ECanaMboxes.MBOX0.MDH.byte.BYTE7 = (Uint8)(OBCinfoData_HV.OBC_TX_ACFreq);
    // Set TRS for all transmit mailboxes
    ECanaRegs.CANTRS.all |= 0x00000001;//Transmit MBOX0-0x301

    ECanaMboxes.MBOX1.MSGCTRL.bit.DLC = 8;
    // Write to the mailbox RAM field of MBOX0 - 3
    OBCinfoData_HV.OBC_TX_PROTECT = PROTECT;//LLC Protect
    OBCinfoData_HV.OBC_TX_GETVOLT = GETVOLT;//0.1V/Div
    OBCinfoData_HV.OBC_TX_GETCURR = GETCURR;//0.1A/Div
    OBCinfoData_HV.OBC_TX_PFCT = PFCT;//1C/Div,offset -40
    OBCinfoData_HV.OBC_TX_TemperatureAVG = Temperature;//1C/Div,offset -40  SETCURRCURT
    ECanaMboxes.MBOX1.MDL.byte.BYTE0 = (Uint8)(OBCinfoData_HV.OBC_TX_PROTECT >> 8);
    ECanaMboxes.MBOX1.MDL.byte.BYTE1 = (Uint8)(OBCinfoData_HV.OBC_TX_PROTECT);
    ECanaMboxes.MBOX1.MDL.byte.BYTE2 = (Uint8)(OBCinfoData_HV.OBC_TX_GETVOLT >> 8);
    ECanaMboxes.MBOX1.MDL.byte.BYTE3 = (Uint8)(OBCinfoData_HV.OBC_TX_GETVOLT);
    ECanaMboxes.MBOX1.MDH.byte.BYTE4 = (Uint8)(OBCinfoData_HV.OBC_TX_GETCURR >> 8);
    ECanaMboxes.MBOX1.MDH.byte.BYTE5 = (Uint8)(OBCinfoData_HV.OBC_TX_GETCURR);
    ECanaMboxes.MBOX1.MDH.byte.BYTE6 = (Uint8)(OBCinfoData_HV.OBC_TX_PFCT);
    ECanaMboxes.MBOX1.MDH.byte.BYTE7 = (Uint8)(OBCinfoData_HV.OBC_TX_TemperatureAVG);
    // Set TRS for all transmit mailboxes
    ECanaRegs.CANTRS.all |= 0x00000002;//Transmit MBOX1-0x302
}

// mailbox_read - This function reads out the contents of the indicated
// by the Mailbox number (MBXnbr). MSGID of a rcv MBX is transmitted as the 
// MDL data.
void CAN_ReceiveMsg_Handle(void)
{
    if(ECanaRegs.CANRMP.all) ;
    else return;

    if(ECanaRegs.CANRMP.bit.RMP2)//0x303
    {
        if(ECanaMboxes.MBOX2.MSGID.all == ((Uint32)0x303<<18))
        {
            OBCinfoData_HV.OBC_Tx_Resp = true;
            OBCinfoData_HV.OBC_RX_LLCONOFF = ECanaMboxes.MBOX2.MDL.byte.BYTE0 & 0x01;
            OBCinfoData_HV.OBC_RX_LLCSETVOLT = ((Uint16)ECanaMboxes.MBOX2.MDL.byte.BYTE1 << 8) | (Uint16)ECanaMboxes.MBOX2.MDL.byte.BYTE2;
            OBCinfoData_HV.OBC_RX_LLCSETCURR = ((Uint16)ECanaMboxes.MBOX2.MDL.byte.BYTE3 << 8) | (Uint16)ECanaMboxes.MBOX2.MDH.byte.BYTE4;
            LLCONOFF = OBCinfoData_HV.OBC_RX_LLCONOFF;
            SETVOLT = OBCinfoData_HV.OBC_RX_LLCSETVOLT;
            SETCURR = OBCinfoData_HV.OBC_RX_LLCSETCURR;
        }
    }
    /* Clear all RMPn bits */
    ECanaRegs.CANRMP.all = 0xFFFFFFFF;
}

void main(void)
{
    // Initialize System Control:
    InitSysCtrl();//System clock = 90M
    // Disable CPU interrupts
    DINT;
    // Initialize PIE control registers to their default state.
    InitPieCtrl();
    // Disable CPU interrupts and clear all CPU
    //interrupt flags
    IER = 0x0000;
    IFR = 0x0000;
    // Initialize the PIE vector table with pointers to the shell Interrupt
    InitPieVectTable();

    memcpy((Uint16 *)&RamfuncsRunStart,(Uint16 *)&RamfuncsLoadStart,(unsigned long)&RamfuncsLoadSize);
    InitFlash();


    GpioInit();

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;
    vEpwm1Config();
    vEpwm2Config();
    vEpwm3Config();
    vEpwm4Config();
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
    vEpwm_PinConfig_Disable();
    ADC_Init();
    CAN_Init();//500kbps
    InitSciBb();//9600bps
    InitCpuTimer0();//1ms and 12us



    // Enable global Interrupts and higher priority real-time debug events
    EINT;   // Enable Global interrupt INTM
    ERTM;   // Enable Global realtime interrupt DBGM

    ParameterInit();

    main_app();
}

void scib_msgTx(void)//must be no-blocking
{
    if ((ScibRegs.SCIFFTX.bit.TXFFST == 0) && (UART1TXFG == 1)) {
        ScibRegs.SCITXBUF = UART1TXBUF[UART1TXCNT++];
        if (UART1TXCNT >= 4) {
            UART1TXCNT = 0;
            UART1TXFG = 0;
        }
    }
}

void main_app(void)
{
    while(1)
    {
        scib_msgTx();
        CAN_ReceiveMsg_Handle();
        CAN_TransmitMsg_Handle();
        Scib_ReceiveData();
        Temperature_Handle();
        if(cpuTimer_1msFalg)//period 1ms
        {
            cpuTimer_1msFalg = 0;
            Scib_TxRxHandle();
            Period_1ms_TaskHandle();
        }
        if(cpuTimer_20usFlag){// 20us Task
            cpu_timer1_Handler();
            cpuTimer_20usFlag = 0;
        }
    }
}
// cpu_timer0_isr - 
void cpu_timer1_Handler(void)//period 20us
{
/*static UINT08 flag;
flag = !flag;
if(flag){
DEBUG_PIN_high();
}else{
DEBUG_PIN_low();
}*/
//    DEBUG_PIN_high();
    ADCBUF[0] = AdcResult.ADCRESULT0;
    ADCBUF[1] = AdcResult.ADCRESULT1;
    ADCBUF[2] = AdcResult.ADCRESULT2;
    ADCBUF[3] = AdcResult.ADCRESULT3;
    ADCBUF[4] = AdcResult.ADCRESULT4;
    ADCBUF[5] = AdcResult.ADCRESULT5;

    static UINT08 counter_1ms;
    counter_1ms++;
    if(counter_1ms > 50){
        counter_1ms = 0;
        cpuTimer_1msFalg = 1;
    }

    static UINT16 AvgCounter;
    static UINT32 VOUT_ADCBUF_SUM,IOUT_ADCBUF_SUM,IOUTDIS_ADCBUF_SUM;
    static UINT32 VBUS_ADCBUF_SUM,VBAT_ADCBUF_SUM,TEMP_ADCBUF_SUM;
    AvgCounter ++;
    VOUT_ADCBUF_SUM += VOUT_ADCBUF;
    IOUT_ADCBUF_SUM += IOUT_ADCBUF;
    IOUTDIS_ADCBUF_SUM += IOUTDIS_ADCBUF;
    VBUS_ADCBUF_SUM += VBUS_ADCBUF;
    if(VBAT_ADCBUF > VbatOffsetAD){
    VBAT_ADCBUF_SUM += (VBAT_ADCBUF - VbatOffsetAD);
    }else{
        VBAT_ADCBUF_SUM += 0;
    }
    TEMP_ADCBUF_SUM += (4095 - TEMP_ADCBUF);

if(AvgCounter >= 1024){
    VOUT_ADCBUF_AVG = VOUT_ADCBUF_SUM >> 10;
    IOUT_ADCBUF_AVG = IOUT_ADCBUF_SUM >> 10;
    IOUTDIS_ADCBUF_AVG = IOUTDIS_ADCBUF_SUM >> 10;
    VBUS_ADCBUF_AVG = VBUS_ADCBUF_SUM >>10;
    VBAT_ADCBUF_AVG = VBAT_ADCBUF_SUM >> 10;
    TEMP_ADCBUF_AVG = TEMP_ADCBUF_SUM >> 10;

    VOUT_ADCBUF_SUM = 0;
    IOUT_ADCBUF_SUM = 0;
    IOUTDIS_ADCBUF_SUM = 0;
    VBUS_ADCBUF_SUM = 0;
    VBAT_ADCBUF_SUM = 0;
    TEMP_ADCBUF_SUM = 0;
    AvgCounter = 0;
}

if (PROTECT) {
    if(PWMEnableFlag){
        vEpwm_PinConfig_Disable();
        CURTPeriod = PWMPeriodMin;
        TD = 1;
        PWMUpdateFlag = 1;
        PWMEnableFlag = 0;
    }
   } else {
       if (RelayOnOffFlag == 0) {
           UINT16 tmp1, tmp2;
           if (VBAT_ADCBUF > VbatOffsetAD) {
               tmp2 = VBAT_ADCBUF - VbatOffsetAD;
           } else {
               tmp2 = 0;
           }
           tmp1 = (tmp2 * KVbat_ADToVout_AD) >> 10;
           if (VOUT_ADCBUF > tmp1) {
               RELAY_EN();
               RelayOnOffFlag = 1;
           }
       }

       if (ONTIMECNT == 499 && (!PWMEnableFlag)) {
           SETVOLTCURT = VOUT_ADCBUF;
           if (SETVOLTCURT > VoutSetMax_AD) {
               SETVOLTCURT = VoutSetMax_AD;
           }
           SETCURRCURT = IoutSetMin_AD;
/*           EALLOW;
           SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
           EPwm1Regs.TBCTR = 0;       // Clear counter
           EPwm2Regs.TBCTR = 0;       // Clear counter
           EPwm3Regs.TBCTR = 0;       // Clear counter
           EPwm4Regs.TBCTR = 0;       // Clear counter
           SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
           EDIS;*/
           vEpwm_PinConfig_Enable();
           PWMEnableFlag = 1;
       }
       if (ONTIMECNT >= 499) {
           static UINT16 VoutDff;
           VoutDff = IOUT_ADCBUF_AVG >> 6;
           if(VoutDff > 20){
               VoutDff = 20;
           }else if(VoutDff < 1){
               VoutDff = 1;
           }

           CURTERRORCV = (SETVOLTCURT - VOUT_ADCBUF) + VoutDff;
           CURTERRORCC = (SETCURRCURT - IOUT_ADCBUF) + 16;

           if(CURTERRORCV< -45){
               DELTCV = (((CURTERRORCV - LASTERRORCV) * (SINT32)1200) + (CURTERRORCV * (SINT32)1200)) >> 10; //4 4
           }else{
               DELTCV = (((CURTERRORCV - LASTERRORCV) * (SINT32)250) + (CURTERRORCV * (SINT32)250)) >> 10; //4 4
           }
           DELTCC = (((CURTERRORCC - LASTERRORCC) * (SINT32)60) + (CURTERRORCC * (SINT32)60)) >> 10; //4 4

           LASTERRORCV = CURTERRORCV;
           LASTERRORCC = CURTERRORCC;
           if(CURTERRORCV < 20){
             DEBUG_PIN_low();
           }else{
             DEBUG_PIN_high();
           }
           CURTPeriod = CURTPeriod + (DELTCV <= DELTCC ? DELTCV : DELTCC);

           //burst mode------------------------------------------------------------
           if (CURTPeriod < PWMPeriodMin){
               BurstModeFlag = 1;
               vEpwm_PinConfig_Disable();//PWM_OFF();
           }else if((CURTPeriod > (PWMPeriodMin + 10))&&(BurstModeFlag)){
               BurstModeFlag = 0;
               vEpwm_PinConfig_Enable();//PWM_ON();
           }
           //burst mode------------------------------------------------------------

           if (CURTPeriod > PWMPeriodMax) {
               CURTPeriod = PWMPeriodMax; //Min Frequency 125k
           } else if (CURTPeriod < PWMPeriodMin) {
               CURTPeriod = PWMPeriodMin; //Max Frequency 300kHz
           }

           static UINT16 TDvsPeriod,TDvsVout;
           TDvsPeriod = (142 * CURTPeriod >> 10);
           if (TDvsPeriod < 50) {
               TDvsPeriod = 50;
           }
           if (GETVOLT < 3800) {
               TDvsVout = 50;
           } else if (GETVOLT < 4300) {
               TDvsVout = ((GETVOLT * 72) >> 10) - 216;
           } else if(GETVOLT < 4800) {
               TDvsVout = ((GETVOLT * 31) >> 10) - 44 ;
           } else {
               TDvsVout = 101;
           }

          // TDvsVout = 105;
           TD = (TDvsPeriod <= TDvsVout ? TDvsPeriod : TDvsVout);
           PWMUpdateFlag = 1;
       } else {
           LASTERRORCV = 0;
           LASTERRORCC = 0;
       }
   }


   ////////////////////////////////////////////////////////////////////////////LLC Fast Protect  20us/time
   ////////////////////////////////////////////////////////////////////////////PFC Ready detect
   if (PFC_READY == 0) {
       PROTECT |= PC_PFCOFF;
   }
   ////////////////////////////////////////////////////////////////////////////Fast Vout UVP
   if (ONTIMECNT >= 499) {
       static UINT16 TempCnt1, TempCnt11;
       if (SETVOLTCURT < VoutUVPFast_AD + 300) { //UVP while softstart
           if (CURTERRORCV > 500) { //voltage error > 10V
               if (TempCnt11 < 400) { //debounce time = 50*12us
                   TempCnt11++;
               } else {
                   PROTECT |= PC_VOUTUVP;
                   TempCnt11 = 0;
               }
           } else {
               TempCnt11 = 0;
           }
       } else {///////////////////UVP while PSU ON
           if (VOUT_ADCBUF < VoutUVPFast_AD) {
               if (TempCnt1 < 400) {
                   TempCnt1++;
               } else {
                   PROTECT |= PC_VOUTUVP;
                   TempCnt1 = 0;
               }
           } else {
               TempCnt1 = 0;
           }
       }
   }


   ////////////////////////////////////////////////////////////////////////////LLC Fast OCP
   static UINT08 TempCnt2;
   if (IOUT_ADCBUF > IoutOCPFast_AD) {
       if (TempCnt2 < 50) {
           TempCnt2++;
       } else {
           PROTECT |= PC_IOUTOCP;
       }
   } else {
       TempCnt2 = 0;
   }



   ////////////////////////////////////////////////////////////////////////////LLC Fast OVP
   static UINT16 TempCnt3;
   if(VOUT_ADCBUF > VoutOVPFast_AD + 30){
       TempCnt3 ++;
   }
   if (VOUT_ADCBUF > VoutOVPFast_AD) {
       if (TempCnt3 < 500) {
           TempCnt3++;
       } else {
           PROTECT |= PC_VOUTOVP;

       }
   } else {
       TempCnt3 = 0;
   }

}
//

void ePWM1_CMPB_INT_Handler(void)//every switching
{
if(PWMUpdateFlag){
//    DEBUG_PIN_high();
    EALLOW;
    vEpwm_SetParameter(CURTPeriod,CURTPeriod>>1,TD);
    EDIS;
    PWMUpdateFlag = 0;
//    DEBUG_PIN_low();
}


}

void epwm1_tz3_ISR_Handler(void){

    vEpwm_PinConfig_Disable();
    PROTECT |= PC_IPRICMP;
}





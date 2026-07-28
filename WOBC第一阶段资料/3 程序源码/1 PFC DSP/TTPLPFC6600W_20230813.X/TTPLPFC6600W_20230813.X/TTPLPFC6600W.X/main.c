#include "p33CK256MP506.h"
#include <stdbool.h>
#include "mcc_generated_files/memory/flash.h"
#include "math.h"

#pragma config   FNOSC     = FRC
#pragma config   IESO      = ON
#pragma config   POSCMD    = NONE
#pragma config   FCKSM     = CSECMD
#pragma config   FWDTEN    = ON_SW
#pragma config   ICS       = PGD1
#pragma config   JTAGEN    = OFF
#pragma config   OSCIOFNC  = ON

typedef unsigned char UINT08;
typedef unsigned short UINT16;
typedef unsigned long UINT32;
typedef signed short SINT16;
typedef signed long SINT32;

//Version number
#define FirmWareREV       0X01
#define HardwareREV       0X01
//Full Scale,Real Value while AD = 4095
#define VbulkFullScale     4600                                                 //real value while AD = 4095
#define VacFullScale       4836                                                 //
#define IacFullScale       1620                                                 //real value while AD = 4095
#define IacZeroAD          2047                                                  

//Limit Set Max Real value

#define VbulkSetValue      4100//4000                                             //452V
#define IacLimtValueLV1    100                                              //11A
#define IacLimtValueLV2    470//500                                              //11A

//protect value
#define VacUVPSlow         800                                             //200
#define VacUVPFast         400                                             //190
#define VacUVPRecover      850                                             //210
#define VacOVPSlow         2700                                             //455
#define VacOVPFast         2800                                             //460
#define VacOVPRecover      2650                                             //450
#define VbulkUVPFast       3000                                             //360
#define VbulkUVPSlow       3200                                             //400
#define VbulkUVPRecover    3800                                             //430
#define VbulkOVPSlow       4300                                             //460
#define VbulkOVPFast       4450                                             //470
#define VbulkOVPRecover    4300                                             //457V
#define IacOCPSlow        350                                              //11.5A
#define IacOCPFast        450                                              //13.5A
#define IacCMP            550                                              //14.5A
#define PinMaxHighLine   6800                                                  //Max output power   set 3300W
#define PinMaxLowLine    3400                                                  //Max output power   set 1000W

#define VacDropLimt      200

#define ACFrequencyMax    650
#define ACFrequencyMin    450

//Protect Code
#define VACUVP       0x0001
#define VACOVP       0x0002
#define IACOCP       0x0004
#define VBULKUVP      0x0008
#define VBULKOVP      0x0010
#define IACCMP       0x0020
#define VACDROP      0x0040
#define FACERROR     0x0080
#define PC_PFCUART    0//0x0100
#define PFCOFF     0// 0x4000
#define PFCERROR    0x8000

//PWM related define
#define PWM_FREQ         40000//                                              //Boost frequency 70k-->110k     //20230613
#define PWM_PERIOD       12500  //500MHz/40kHz
#define PWM_DUTY_MAX	 11875		//95%	 
#define PWM_DUTY_MIN     0			//0%
#define PWMDeadtime      250                                                   // deadtime = 200 *2ns =500ns

//Retry delay time
#define RETRYTIME         3000                                                   //500ms  

//IO 
#define oRELAY_CTRL_Hold	    (LATCbits.LATC7)
#define oRELAY_CTRL_PickUp	    (LATCbits.LATC5)
#define oPFC_OK                 (LATDbits.LATD4)                                            //Output pin, high = Boost Ready 
#define oDEBUG_PIN               (LATBbits.LATB10)                                            //                                           //
#define oGATE_NH		         (LATBbits.LATB12) 
#define oGATE_NL		         (LATBbits.LATB13)  
#define iDAB_READY               (PORTDbits.RD3) 


#define OFF_ISR()				{SRbits.IPL = 7;}	/*Interrupts disabled.(CPU Interrupt priority level is 7)*/
#define ON_ISR()			    {SRbits.IPL = 0;}	/*Interrupt priority 3 ~ 7 Enable*/

//ADC Buffer define
static UINT16 ADCRDBUF[13];
#define IAC_ADCBUF      ADCRDBUF[0] 
#define VBULK_ADCBUF    ADCRDBUF[1] 
#define VACL_ADCBUF    ADCRDBUF[3] 
#define VACN_ADCBUF    ADCRDBUF[4] 
#define TEMP_ADCBUF     ADCRDBUF[12]




//Variable send to DAB
static UINT16 PROTECT; //PSFB Protect Code
static UINT16 GETVBULK; //Report Output Voltage, 0.1V/bit
static UINT16 GETIAC; //Report Output Current, 0.1A/bit   
static UINT16 GETVAC; //Report Output Current, 0.1A/bit   
static UINT16 GETFREQ; //AC frequency
static UINT08 TemperatureAVG; //Report Temperature, offset 40D
//Variable received from DAB
static UINT08 PFCONOFF;

//Temperature Table
static UINT16 TEMPTABLE[170] = {
    5, 5, 6, 6, 6, 7, 7, 7, 8, 8, //-40 -31
    9, 9, 10, 10, 11, 11, 12, 12, 13, 14, //-30 -21
    14, 15, 16, 17, 17, 18, 19, 20, 21, 22, //-20 -11
    23, 24, 25, 26, 28, 29, 30, 31, 33, 34, //-10 -01
    36, 37, 39, 40, 42, 44, 46, 48, 49, 51, //000 009
    54, 56, 58, 60, 62, 65, 67, 70, 72, 75, //010 019
    78, 81, 84, 87, 90, 93, 96, 100, 103, 106, //020 029
    110, 114, 117, 121, 125, 129, 133, 138, 142, 146, //030 039
    151, 155, 160, 164, 169, 174, 179, 184, 189, 194, //040 049
    199, 205, 210, 215, 221, 227, 232, 238, 244, 250, //050 059
    255, 261, 268, 274, 280, 286, 292, 299, 305, 311, //060 069
    318, 324, 331, 337, 344, 350, 357, 364, 370, 377, //070 079
    384, 390, 397, 404, 411, 417, 424, 431, 438, 444, //080 089
    451, 458, 465, 471, 478, 485, 491, 498, 505, 511, //090 099
    518, 524, 531, 537, 544, 550, 556, 562, 569, 575, //100 109
    581, 587, 593, 599, 605, 611, 616, 622, 628, 633, //110 119
    639, 644, 650, 655, 660, 665, 670, 674, 678, 683 //120 129
};

static UINT08 UART1TXBUF[15], UART1RXBUF[8];
static UINT08 UART1TXCNT, UART1RXCNT, UART1TXFG, UART1RXFG;
static UINT08 UART1TXTIMECNT, UART1RXOTIMEPCNT, UART1RXIDLECNT;

//////////////////////////////////////////////////////BOOST variable
static UINT16 KVbulk_RealToAD, KVbulk_ADToReal, KVac_RealToAD, KVac_ADToReal, KIac_RealToAD, KIac_ADToReal;
static UINT16 VbulkSetValue_AD;
static UINT16 VacUVPFast_AD, VacOVPFast_AD, IacOCPFast_AD, VbulkUVPFast_AD, VbulkOVPFast_AD;
static UINT16 IacLimtValueLV1_AD_Q15, IacLimtValueLV2_AD_Q15;

static SINT32 ONTIMECNT, OFTIMECNT, SETVOLTCURT, LMTVOLT, SETCURRCURT, LMTCURR;
static SINT32 CURTERRORCV, LASTERRORCV, DELTCV;
static SINT32 CURTERRORCC, LASTERRORCC, DELTCC;
static SINT32 VbulkLoopOutput, IacLoopOutput;
static UINT16 CURTMDC;
static SINT32 IacP, IacI;

static SINT32 IacConv, VacConv;
static SINT32 VacConvFiltered, IacConvFiltered, VbulkFiltered;
static UINT32 vin_squared_average, iin_squared_average;

//PFC Flags
static UINT08 SoftStartState; //0 -- NOT start; 1 -- Ongoing;  2 -- DONE

static UINT16 RCONRecord;
static UINT16 Temperature;

static UINT32 positive_cycle_counter, negative_cycle_counter;
static UINT08 positive;
static UINT16 cir_buff[128];
static UINT08 emi_pointer, cir_buff_ptr;

void half_cycle_processing();
void VoltageLoopControl(void);
void CurrentLoopControl(void);

static UINT16 DEBUGCNT;
static UINT08 PWMEnableFlag;

int main(void) {
    RCONRecord = RCON;
    RCON = 0;
    OFF_ISR();
    KVbulk_RealToAD = __builtin_muluu(4095, 1024) / VbulkFullScale; 
    KVbulk_ADToReal = __builtin_muluu(VbulkFullScale, 1024) / 4095;
    KVac_RealToAD = __builtin_muluu(4095, 1024) / VacFullScale;
    KVac_ADToReal = __builtin_muluu(VacFullScale, 1024) / 4095;
    KIac_RealToAD = __builtin_muluu(4095, 1024) / IacFullScale;
    KIac_ADToReal = __builtin_muluu(IacFullScale, 1024) / 4095;

    VbulkSetValue_AD = __builtin_muluu(VbulkSetValue, KVbulk_RealToAD) >> 10; //4520/1024*825=3642

    VacUVPFast_AD = (__builtin_muluu(VacUVPFast, KVac_RealToAD) >> 10);
    VacOVPFast_AD = (__builtin_muluu(VacOVPFast, KVac_RealToAD) >> 10);
    IacOCPFast_AD = __builtin_muluu(IacOCPFast, KIac_RealToAD) >> 10;
    VbulkOVPFast_AD = __builtin_muluu(VbulkOVPFast, KVbulk_RealToAD) >> 10;
    VbulkUVPFast_AD = __builtin_muluu(VbulkOVPFast, KVbulk_RealToAD) >> 10;

    IacLimtValueLV1_AD_Q15 = __builtin_muluu(IacLimtValueLV1, KIac_RealToAD) >> 7;
    IacLimtValueLV2_AD_Q15 = __builtin_muluu(IacLimtValueLV2, KIac_RealToAD) >> 7;
//PLL&APLL Config
    CLKDIVbits.PLLPRE = 1;
    PLLFBDbits.PLLFBDIV = 90; // 8*90/2
    PLLDIVbits.POST1DIV = 2;
    PLLDIVbits.POST2DIV = 1;
    __builtin_write_OSCCONH(0x01);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    while (OSCCONbits.OSWEN != 0);
    while (OSCCONbits.LOCK != 1);

    ACLKCON1bits.FRCSEL = 1;  
    ACLKCON1bits.APLLPRE = 1; 
    APLLFBD1bits.APLLFBDIV = 125; 
    APLLDIV1bits.APOST1DIV = 2; 
    APLLDIV1bits.APOST2DIV = 1;
    ACLKCON1bits.APLLEN = 1;
    ////////////////////////////////////////IO config
    ANSELBbits.ANSELB4 = 0;
    TRISBbits.TRISB4 = 0; //���

    TRISCbits.TRISC5 = 0;
    TRISCbits.TRISC7 = 0;
    TRISDbits.TRISD4 = 0;
    TRISBbits.TRISB10 = 0;
    TRISBbits.TRISB12 = 0;
    TRISBbits.TRISB13 = 0;
    TRISDbits.TRISD3 = 1;
    TRISCbits.TRISC10 = 0;
    ////////////////////////////////UART1 TO
    ANSELDbits.ANSELD10 = 0;
	
    __builtin_write_RPCON(0x0000); 
    _U1RXR = 59; //RC11->UART1:U1RX
    _RP58R = 1; //RC10->UART1:U1TX
    __builtin_write_RPCON(0x8000);
    U1MODEHbits.BCLKSEL = 0;  //Fsoc/2 40MHZ
    U1MODEbits.UARTEN = 1;
    U1BRG = 585;//�����ʼĴ���
    U1MODEbits.URXEN = 1;
    U1MODEbits.UTXEN = 1;

    //ADC Config   
    ADCON3Hbits.CLKSEL = 2; //FSRC = AFVCODIV = 125MHz
    ADCON3Hbits.CLKDIV = 0; //FCORESRC = F SRC/1 =125MHz
    ADCORE0Hbits.ADCS = 0; //FADCCORE0 = FCORESRC/2=62.5Mhz
    ADCORE1Hbits.ADCS = 0; //FADCCORE1 = FCORESRC/2=62.5Mhz
    ADCON2Lbits.SHRADCS = 0; //FADCCORECOM = FCORESRC/2=70Mhz
    ADCON2Hbits.SHRSAMC = 50;

    ADCON4Hbits.C1CHS = 1; //AN1 input = ANA1 
    ADTRIG0Lbits.TRGSRC0 = 4; //AN0 PWM1 triger1
    ADTRIG0Lbits.TRGSRC1 = 4; //AN1 PWM1 triger1
    ADTRIG0Hbits.TRGSRC3 = 4; //AN3 PWM1 triger1
    ADTRIG1Lbits.TRGSRC4 = 4; //AN3 PWM1 triger1
    //    ADTRIG1Hbits.TRGSRC7 = 1; //AN7 soft triger
    //    ADTRIG2Lbits.TRGSRC8 = 1; //AN8 soft triger
    //    ADTRIG2Lbits.TRGSRC9 = 1; //AN9 PWM1 triger1
    ADTRIG3Lbits.TRGSRC12 = 1; //AN12 soft triger
    //ADTRIG3Lbits.TRGSRC13 = 8; //AN13 PWM3 triger1
    //    ADTRIG3Hbits.TRGSRC14 = 8; //AN14 PWM3 triger1
    //    ADTRIG4Lbits.TRGSRC17 = 1; //AN17 soft triger
    //    ADTRIG4Hbits.TRGSRC19 = 1; //AN19 soft triger 

    ADIELbits.IE0 = 1; //���˲����������ʱ����ADCͨ���жϡ�
    IPC22bits.ADCAN0IP = 6;  //�����ж����ȼ�
    IFS5bits.ADCAN0IF = 0;
    IEC5bits.ADCAN0IE = 1;

    ADIELbits.IE3 = 1;
    IFS5bits.ADCAN3IF = 0;
    IEC5bits.ADCAN3IE = 1;

    ADIELbits.IE4 = 1;
    IFS5bits.ADCAN4IF = 0;
    IEC5bits.ADCAN4IE = 1;

    ADCON5Hbits.WARMTIME = 15; // Set initialization time to maximum   
    ADCON1Lbits.ADON = 1; // Turn on ADC module  

    ADCON5Lbits.C0PWR = 1; // Turn on analog power for dedicated core 0   
    while (ADCON5Lbits.C0RDY == 0); // Wait when the core 0 is ready for operation   
    ADCON3Hbits.C0EN = 1; // Turn on digital power to enable triggers to the core 0   

    ADCON5Lbits.C1PWR = 1; // Turn on analog power for dedicated core 1  
    while (ADCON5Lbits.C1RDY == 0); // Wait when the core 1 is ready for operation  
    ADCON3Hbits.C1EN = 1; // Turn on digital power to enable triggers to the core 1   

    ADCON5Lbits.SHRPWR = 1; // Turn on analog power for shared core    
    while (ADCON5Lbits.SHRRDY == 0); // Wait when the shared core is ready for operation
    ADCON3Hbits.SHREN = 1; // Turn on digital power to enable triggers to the shared core   

    ADCON3Lbits.SWCTRG = 1; //ADC common soft triger 


    //CMP2 config  LLC
    DACCTRL1Lbits.DACON = 1;  
    DACCTRL1Lbits.CLKSEL = 2;
    DACCTRL2L = 0x0055;  
    DACCTRL2H = 0x008A; 
    DAC2CONLbits.DACEN = 1;
    DAC2CONLbits.IRQM = 2; 
    DAC2DATH = 405; //= 2048 - 65A/162A *4095   
    DACCTRL1Lbits.DACON = 1;
    IFS4bits.CMP2IF = 0;  
    IEC4bits.CMP2IE = 1;


    //CMP1 config  BOOST
    DAC1CONLbits.DACEN = 1;
    DAC1CONLbits.IRQM = 1;
    DAC1CONLbits.INSEL = 1;
    DAC1DATH = 3691; // = 2048 + 65A/162A *4095
    DACCTRL1Lbits.DACON = 1;
    IFS4bits.CMP1IF = 0;  
    IEC4bits.CMP1IE = 1;  

    //PWM1 cofig   PFC driver
    PCLKCONbits.MCLKSEL = 3; //FPWM=AFPLLO=500MHz 
    PG1DTH = 250; //PWMDeadtime;  
    PG1DTL = 250; //PWMDeadtime;
    MPER = PWM_PERIOD; //40K 
    MDC = 0;
    PG1TRIGA = 1;
    PG1PHASE = 0;
    PG1CONH = 0xC800;
    PG1CONL = 0x0009;
    PG1EVTL = 0x0118;

    PG1IOCONLbits.OVRENH = 1; 
    PG1IOCONLbits.OVRENL = 1; 
    PG1IOCONHbits.PENH = 0; // 
    PG1IOCONHbits.PENL = 0; // 
    PG1CONLbits.ON = 1;


    //PWM4H    Vbulk PWM ADC to DAB
    PG4PER = 2048; 
    PG4DC = 0; 
    PG4CONLbits.CLKSEL = 1; 
    PG4EVTLbits.UPDTRG = 1; 
    PG4IOCONHbits.PMOD = 1;
    PG4STATbits.UPDREQ = 1; 
//UPDATE״̬λ��ָʾ1��

    PG4IOCONLbits.OVRENH = 0; 
    PG4IOCONLbits.OVRENL = 1; 
    PG4IOCONHbits.PENH = 1; 
    PG4CONLbits.ON = 1;

    //PWM3H  for debug   
    PG3PER = 2048;
    PG3DC = 0;
    PG3CONLbits.CLKSEL = 1;
    PG3EVTLbits.UPDTRG = 1;
    PG3IOCONHbits.PMOD = 1;
    PG3STATbits.UPDREQ = 1;

    PG3IOCONLbits.OVRENH = 0;
    PG3IOCONLbits.OVRENL = 1;
    PG3IOCONHbits.PENH = 1;
    PG3CONLbits.ON = 1;

    //PWM8H  PFC floating VCC driver
    PG8PER = 2500; //200kHz
    PG8DC = PG8PER >> 1;
    PG8CONLbits.CLKSEL = 1;
    PG8EVTLbits.UPDTRG = 1;
    PG8IOCONHbits.PMOD = 1;
    PG8STATbits.UPDREQ = 1;

    PG8IOCONLbits.OVRENH = 0;
    PG8IOCONLbits.OVRENL = 1;
    PG8IOCONHbits.PENH = 1;
    PG8CONLbits.ON = 1;



    INTCON1bits.NSTDIS = 0; 
    INTCON2bits.GIE = 1; 
     T1CONbits.TECS = 1;  
     PR1 = 1080; 	 
    IPC0bits.T1IP = 2; 
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    T1CONbits.TON = 1;


    CCP2CON1Lbits.CLKSEL = 0;
    CCP2CON1Lbits.CCSEL = 0;  
    CCP2CON1Lbits.T32 = 1; 
    CCP2CON1Lbits.MOD = 0; 
    CCP2CON1Hbits.SYNC = 0; 
    CCP2PRH = 0x0001;  
    CCP2PRL = 0x5F90;
    IFS1bits.CCT2IF = 0;
    CCP2CON1Lbits.CCPON = 1;

    PROTECT |= PFCERROR;
    PFCONOFF = 1;
    oRELAY_CTRL_Hold = 0; //�̵���
    oRELAY_CTRL_PickUp = 0;
    ON_ISR();

    PG3IOCONHbits.PENH = 0;

    for (;;) {
 oDEBUG_PIN = 0;        
        if (ADSTATLbits.AN1RDY == 1) 
			{
            ADSTATLbits.AN1RDY = 0;
            ADCRDBUF[1] = ADCBUF1;
            //update PWM4 DAC of Vbulk    Vbulk:���������ѹ
            //            PG4DC = VBULK_ADCBUF >> 1;
            VbulkFiltered = ((VbulkFiltered + VBULK_ADCBUF) >> 1); //TESTen
        }
        if (ADSTATLbits.AN12RDY == 1) {
            ADSTATLbits.AN12RDY = 0;
            ADCRDBUF[12] = ADCBUF12 >> 2;
        }

        if ((IFS0bits.U1TXIF == 1) && (UART1TXFG == 1)) {
            IFS0bits.U1TXIF = 0;
            U1TXREGbits.TXREG = UART1TXBUF[UART1TXCNT++];
            if (UART1TXCNT >= 12) {
                UART1TXCNT = 0;
                UART1TXFG = 0;
            }
        }

        if (IFS0bits.U1RXIF == 1) {
            IFS0bits.U1RXIF = 0;
            if (UART1RXCNT < 4) {
                UART1RXBUF[UART1RXCNT] = U1RXREGbits.RXREG;
                UART1RXCNT++;
            }
            UART1RXIDLECNT = 5;
        }

        ///////////////////////////////////////////////////////Temperature transform �����
        static UINT16 TSERCHND;
        static UINT08 TSERCHL, TSERCHR, TSERCHM;
        if (TSERCHL < TSERCHR) {
            TSERCHM = (TSERCHL + TSERCHR) >> 1;
            if (TSERCHM > 169) {
                TSERCHM = 169;
            } else if (TSERCHM < 0) {
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
            TSERCHND = TEMP_ADCBUF;
        }

//                static UINT08 debugflag;
//        debugflag = !debugflag;
//        oDEBUG_PIN = debugflag; 
        ///////////////////////////////////////////////////////1ms task
        if (IFS1bits.CCT2IF == 1) {
            IFS1bits.CCT2IF = 0;
            ADCON3Lbits.SWCTRG = 1;       
            GETVAC = (UINT32) ((sqrt((double) (vin_squared_average << 9))) * KVac_ADToReal) >> 10;
            GETIAC = (UINT32) ((sqrt((double) (iin_squared_average << 9))) * KIac_ADToReal) >> 10;
            ///////////////////////////////////////////////////////LLC VOUT avg filiter
            static UINT08 FilterCNT0 = 0;
            static UINT16 VBULK_ADCBUF_MAX, VBULK_ADCBUF_MIN, VBULK_ADCBUF_AVG;
            static UINT32 VBULK_ADCBUF_SUM;
            VBULK_ADCBUF_SUM += VBULK_ADCBUF;
            if (VBULK_ADCBUF_MAX < VBULK_ADCBUF) {
                VBULK_ADCBUF_MAX = VBULK_ADCBUF;
            }
            if (VBULK_ADCBUF_MIN > VBULK_ADCBUF) {
                VBULK_ADCBUF_MIN = VBULK_ADCBUF;
            }
            FilterCNT0++;
            if (FilterCNT0 == 34) {
                FilterCNT0 = 0;
                VBULK_ADCBUF_AVG = (VBULK_ADCBUF_SUM - VBULK_ADCBUF_MAX - VBULK_ADCBUF_MIN) >> 5;
                GETVBULK = __builtin_muluu(VBULK_ADCBUF_AVG, KVbulk_ADToReal) >> 10;
                VBULK_ADCBUF_SUM = 0;
                VBULK_ADCBUF_MAX = 0;
                VBULK_ADCBUF_MIN = 4095;
            }

            //////////////////////////////////////////////////////////Temperature avg filter
            static UINT08 FilterCNT4 = 4;
            static UINT16 Temperature_MAX, Temperature_MIN;
            static UINT32 Temperature_SUM;
            Temperature_SUM += Temperature;
            if (Temperature_MAX < Temperature) {
                Temperature_MAX = Temperature;
            }
            if (Temperature_MIN > Temperature) {
                Temperature_MIN = Temperature;
            }
            FilterCNT4++;
            if (FilterCNT4 == 34) {
                FilterCNT4 = 0;
                TemperatureAVG = (Temperature_SUM - Temperature_MAX - Temperature_MIN) >> 5;
                Temperature_SUM = 0;
                Temperature_MAX = 0;
                Temperature_MIN = 4095;
            }
            ///////////////////////////////////////////////////////////////////Slow PROTECT
            ////////////////////////////////////////////////////BOOST ON OFF detect
            if (PFCONOFF == 0) {
                PROTECT |= PFCOFF;
            }
            ///////////////////////////////////////////////////////VOUT slow OVP
            static UINT08 TmpCNT1;
            if (GETVBULK > VbulkOVPSlow) {
                if (TmpCNT1 < 100) {
                    TmpCNT1++;
                } else {
                    PROTECT |= VBULKOVP;
                }
            } else {
                TmpCNT1 = 0;
            }
            ///////////////////////////////////////////////////////VOUT slow UVP
            static UINT08 TmpCNT2;
            if ((GETVBULK < VbulkUVPSlow)&&(SoftStartState == 2)) {
                if (TmpCNT2 < 100) {
                    TmpCNT2++;
                } else {
                    PROTECT |= VBULKUVP;
                }
            } else {
                TmpCNT2 = 0;
            }
            ///////////////////////////////////////////////////////IOUT slow OCP 
            static UINT08 TmpCNT3;
            if (GETIAC > IacOCPSlow) {
                if (TmpCNT3 < 200) {
                    TmpCNT3++;
                } else {
                    PROTECT |= IACOCP;
                }
            } else {
                TmpCNT3 = 0;
            }
            ///////////////////////////////////////////////////////IOUT slow OCP 
            static UINT08 TmpCNT4;
            if (GETVAC < VacUVPSlow) {
                if (TmpCNT4 < 200) {
                    TmpCNT4++;
                } else {
                    PROTECT |= VACUVP;
                }
            } else {
                TmpCNT4 = 0;
            }
            ///////////////////////////////////////////////////////IOUT slow OCP 
            static UINT08 TmpCNT5;
            if (GETVAC > VacOVPSlow) {
                if (TmpCNT5 < 200) {
                    TmpCNT5++;
                } else {
                    PROTECT |= VACOVP;
                }
            } else {
                TmpCNT5 = 0;
            }

            ///////////////////////////////////////////////////////IOUT slow OCP 
            static UINT08 TmpCNT6;
            if ((GETFREQ > ACFrequencyMax) || (GETFREQ < ACFrequencyMin)) {
                if (TmpCNT6 < 200) {
                    TmpCNT6++;
                } else {
                    PROTECT |= FACERROR;
                }
            } else {
                TmpCNT6 = 0;
            }

            /////////////////////////////////////////////////BOOST ON OFF Control
            if (PROTECT) {
                ONTIMECNT = 0;
                if (OFTIMECNT < RETRYTIME) {
                    OFTIMECNT++;
                }
                if (OFTIMECNT == 1) {
                    oPFC_OK = 0;
                    PROTECT |= PFCERROR;
                    SoftStartState = 0;
                }
                if (OFTIMECNT == 2) {
oDEBUG_PIN = 1;   
                    MDC = 0;
                    PG1TRIGA = 1;
                    IacLoopOutput = 0;
                    IacI = 0;
                    VbulkLoopOutput = 0;
                    PG1IOCONLbits.OVRENH = 1; //PWMOFF   
                    PG1IOCONLbits.OVRENL = 1; //PWMOFF  
                    PG1IOCONHbits.PENH = 0;
                    PG1IOCONHbits.PENL = 0;
                    oGATE_NH = 0;
                    oGATE_NL = 0;
                    PWMEnableFlag = 0;
                }
                if (OFTIMECNT == 4) {
                    oRELAY_CTRL_Hold = 0;
                    oRELAY_CTRL_PickUp = 0;
                }
                if (OFTIMECNT >= RETRYTIME) {
                    //////////////////////////////////////////////////////////////BOOST Fault Recover                    
                    if (GETVAC > VacUVPRecover) {////////VIN UVP RECOVER
                        PROTECT &= ~VACUVP;
                    }
                    if (GETVAC < VacOVPRecover) {///////VIN OVP RECOVER
                        PROTECT &= ~VACOVP;
                    }
                    PROTECT &= ~IACOCP;
                    PROTECT &= ~VBULKUVP;
                    if (GETVBULK < VbulkOVPRecover) {
                        PROTECT &= ~VBULKOVP;
                    }
                    PROTECT &= ~IACCMP;
                    PROTECT &= ~VACDROP;
                    if ((GETFREQ < ACFrequencyMax) && (GETFREQ > ACFrequencyMin)) {
                        PROTECT &= ~FACERROR;
                    }

                    if (UART1RXOTIMEPCNT > 0) {
                        PROTECT &= ~PC_PFCUART;
                    }
                    if (PFCONOFF == 1) {
                        PROTECT &= ~PFCOFF;
                    }
                    PROTECT &= ~PFCERROR;
                }
            } else {
                OFTIMECNT = 0;
                if (ONTIMECNT < 1500) {
                    ONTIMECNT++;
                }
                if (ONTIMECNT == 800) {
                    oRELAY_CTRL_PickUp = 1;
                    oRELAY_CTRL_Hold = 1;
                }
                if (ONTIMECNT == 1000) {
                    oRELAY_CTRL_PickUp = 0;
                }
                if (ONTIMECNT == 850) {
                    SETVOLTCURT = VBULK_ADCBUF;
                    if (SETVOLTCURT > VbulkSetValue_AD) {
                        SETVOLTCURT = VbulkSetValue_AD;
                    }
                    SoftStartState = 1;
                    LMTCURR = IacLimtValueLV1_AD_Q15;

                    PG1IOCONHbits.PENH = 1; //Enable PWM
                    PG1IOCONHbits.PENL = 1;
                    PWMEnableFlag = 1;
                    //start PFC
                }
                if (ONTIMECNT > 850) { //soft start                       
                    if (SETVOLTCURT < VbulkSetValue_AD) {//3642
                        SETVOLTCURT = SETVOLTCURT + 10;
                    } else {
                        SETVOLTCURT = VbulkSetValue_AD;
                        LMTCURR = IacLimtValueLV2_AD_Q15;
                        SoftStartState = 2;
                    }
                }
                if ((PROTECT == 0)&&(ONTIMECNT >= 1500)) {
                    oPFC_OK = 1;
                }
            }

            ///////////////////////////////////////////////////////////UART1 connect to PFC
            if (UART1TXTIMECNT == 1) {//Response
                UART1TXTIMECNT = 0;
                UART1TXBUF[0] = 0xFF;
                UART1TXBUF[1] = 0x55;              
                UART1TXBUF[2] = (UINT08) (PROTECT >> 8);
                UART1TXBUF[3] = (UINT08) (PROTECT);
                UART1TXBUF[4] = (UINT08) (GETVAC >> 8);
                UART1TXBUF[5] = (UINT08) (GETVAC);
                UART1TXBUF[6] = (UINT08) (GETIAC >> 8);
                UART1TXBUF[7] = (UINT08) (GETIAC);                
                UART1TXBUF[8] = (UINT08) (GETFREQ >> 8);
                UART1TXBUF[9] = (UINT08) (GETFREQ);
                UART1TXBUF[10] = (UINT08) (TemperatureAVG);
				//CRCУ��λ
				UART1TXBUF[11] = 0x54 + UART1TXBUF[2] + UART1TXBUF[3] + UART1TXBUF[4] + UART1TXBUF[5] + UART1TXBUF[6] + UART1TXBUF[7] +
                        + UART1TXBUF[8] + UART1TXBUF[9] + UART1TXBUF[10];
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
                    if ((UART1RXCNT == 4) && (UART1RXBUF[0] == 0xFF) && (UART1RXBUF[1] == 0x55)) {
                        UINT08 CheckSum = 0;
                        CheckSum = UART1RXBUF[0] + UART1RXBUF[1] + UART1RXBUF[2];
                        if (CheckSum == UART1RXBUF[3]) {
                            UART1RXOTIMEPCNT = 250;
                            UART1TXTIMECNT = 1; //PFC SCI Response to DAB
                            PFCONOFF = UART1RXBUF[2];
                        }
                    }
                    UART1RXCNT = 0;
                    UART1RXBUF[0] = 0;
                    UART1RXBUF[1] = 0;
                }
            }
        }
    }
}

void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void) {
    IFS0bits.T1IF = 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _CMP2Interrupt(void) {
    IFS4bits.CMP2IF = 0;
    PROTECT |= IACCMP;
}

void __attribute__((__interrupt__, no_auto_psv)) _CMP1Interrupt(void) {
    IFS4bits.CMP1IF = 0;
    PROTECT |= IACCMP;
}

void __attribute__((__interrupt__, auto_psv)) _ADCAN3Interrupt(void) {
    ADCRDBUF[3] = ADCBUF3;
    IFS5bits.ADCAN3IF = 0;
}

void __attribute__((__interrupt__, auto_psv)) _ADCAN4Interrupt(void) {
    ADCRDBUF[4] = ADCBUF4;
    IFS5bits.ADCAN4IF = 0;
}

#define VAC_BROWNOUT_SQURE	  6393      //80V/460V << 15

static UINT32 PWMCount, ClearPWMCount;
static UINT08 SoftStart_ZC, BurstModeFlag;

static UINT08 VACPol, ZCRange, BurstMode, PWMUpdateStop, BurstModeDetect, VACPol_last, ZCRange_last, BurstCounter, CompFreeze, dffOff;
static UINT16 PWM_Counter, AvgCounter;
static UINT16 VOUTAvg;
static UINT32 VOUTAvgSum, VrmsSqure;


/**
 * @file _ADCAN0Interrupt
 * @brief ADC通道0中断服务程序 - PFC变换器实时控制核心
 * 
 * @details
 * 本中断服务程序负责PFC变换器的所有实时控制功能，执行频率为25μs（40kHz）。
 * 主要功能包括：
 * - 交流电压极性检测与过零同步
 * - 输出电压平均值计算
 * - 四阶段过零检测与PWM控制
 * - 软启动控制（占空比渐进增加）
 * - 双环控制（电压环+电流环）
 * - Burst模式管理（轻载效率优化）
 * - 快速保护机制（交流跌落、过流、过压、欠压）
 * 
 * @note
 * - 中断周期：25μs
 * - 软启动持续时间：12个PWM周期（约300μs）
 * - 电压环执行频率：每11个中断周期执行一次
 * - 电流环执行频率：每个中断周期执行一次
 * 
 * @warning
 * - 本函数为中断服务程序，执行时间必须严格控制
 * - 保护标志触发后会立即关闭PWM输出
 * - Burst模式下不执行软启动控制
 */
void __attribute__((__interrupt__, auto_psv)) _ADCAN0Interrupt(void) {
    ADCRDBUF[0] = ADCBUF0; //VACL
    IFS5bits.ADCAN0IF = 0;

    half_cycle_processing();
    if (PWM_Counter < 3000) {
        PWM_Counter++;
    }

    //////////////////////////////////////////VACPOL dected
    VACPol_last = VACPol;
    ZCRange_last = ZCRange;
   
    if ((VACL_ADCBUF > VACN_ADCBUF + 100)) { 
        VACPol = 1; 
    } else if ((VACN_ADCBUF > VACL_ADCBUF + 100)) {
        VACPol = 0; 
    }
    ///////////////////////////////////////Vout Avg 
    AvgCounter++;
    VOUTAvgSum += VBULK_ADCBUF;
    if (((AvgCounter > 1) && (!VACPol_last) && (VACPol))
            || (AvgCounter > 1500)) {
        VOUTAvg = (uint16_t) __builtin_divud(VOUTAvgSum, AvgCounter - 1);
        VOUTAvgSum = 0;
        AvgCounter = 0;
    }

    if (PROTECT) {    // 检查是否有保护标志置位
        if(PWMEnableFlag){ //PWM ON
        oDEBUG_PIN = 1;    // 调试引脚置高，用于示波器观测保护触发时刻           
        PWMEnableFlag = 0; // 清除PWM使能标志，表示PWM已关闭
        oPFC_OK = 0;  // PFC状态指示输出低电平，表示PFC故障
        //PFC OFF
        MDC = 0;  // 主占空比寄存器清零，停止PWM输出
        PG1TRIGA = 1;  // PWM触发寄存器设为最小值，确保立即响应
        IacLoopOutput = 0; // 电流环PI输出清零，防止积分饱和
        VbulkLoopOutput = 0; // 电压环PI输出清零，防止积分饱和
        PG1IOCONLbits.OVRENH = 1; //PWMOFF 上管PWM输出强制关闭（OVRENH=1表示覆盖使能，输出关闭)
        PG1IOCONLbits.OVRENL = 1; //PWMOFF 下管PWM输出强制关闭（OVRENL=1表示覆盖使能，输出关闭） 
        PG1IOCONHbits.PENH = 0; // 上管PWM引脚功能禁用，转为GPIO模式
        PG1IOCONHbits.PENL = 0;  // 下管PWM引脚功能禁用，转为GPIO模式
        }
    } else {   // 无保护标志，系统正常运行
        if (ONTIMECNT >= 850) {  // 系统启动延时达到850*20us=17ms后开始正常工作
            //ZC Detecting-------------------------------------------------------
            // phase 1, real AC pos to neg but still pos, STOP (inverted here) 正→负过渡，但仍为正（关闭PWM）
            if ((VACPol) && (VacConv < 40) && (!ZCRange)) {
                if (!BurstMode) { //BURST                
                    PG1IOCONLbits.OVRENH = 1; //PWM OFF
                    PG1IOCONLbits.OVRENL = 1; //PWM OFF
                    oGATE_NL = 0; // 下管驱动输出置低
                    oGATE_NH = 0;  // 上管驱动输出置低
                }
                PWMUpdateStop = 1;// 设置PWM更新停止标志
                ZCRange = 1; // 设置过零区间标志，表示处于过零过渡区
            }// phase 2, real AC pos to neg but just neg, RESTART (inverted here) 正→负过渡，刚变为负（重启PWM+软启动）
            else if ((!VACPol) && ((VacConv > (40 + 80))) && (ZCRange)) {//40+80=120：过零后重启阈值，电压高于120（约12V）认为已过零稳定
                PG1IOCONLbits.SWAP = 0;  // 切换PWM极性 0=负半周配置，1=正半周配置
                if (!BurstMode) { //BURST               
                    if (SoftStartState == 2)  // 如果软启动状态为2（正常运行）
                        PG1IOCONLbits.OVRENH = 0;  // 上管PWM使能（负半周上管工作）
                    else PG1IOCONLbits.OVRENH = 1; // 否则上管关闭（软启动期间）
                        PG1IOCONLbits.OVRENL = 1; //0  // 下管PWM强制关闭
                   
                }
                SoftStart_ZC = 1;  // 触发软启动
                PWM_Counter = 1; // 重置软启动计数器
                PWMUpdateStop = 0;
                ZCRange = 0;  // 设置过零区间标志
            }// phase 3, real AC neg to pos but still neg, STOP (inverted here)
            // 阶段3：交流负半周向正半周过渡，但仍为负电压（过零前关闭PWM）
            else if ((!VACPol) && ((VacConv < (40 + 0))) && (!ZCRange)) {
                if (!BurstMode) { // 非Burst模式
                    PG1IOCONLbits.OVRENH = 1; //PWM OFF
                    PG1IOCONLbits.OVRENL = 1; //PWM OFF
                    oGATE_NL = 0; // 下管驱动输出置低
                    oGATE_NH = 0;  // 上管驱动输出置低
                }
                PWMUpdateStop = 1; // 设置PWM更新停止标志
                ZCRange = 1; // 设置过零区间标志
            }// phase 4, real AC neg to pos but just pos, RESTART (inverted here)
             // 阶段4：交流负半周向正半周过渡，刚变为正电压（过零后重启PWM+软启动）
            else if ((VACPol) && ((VacConv > (40 + 80))) && (ZCRange)) {
                PG1IOCONLbits.SWAP = 1; // 设置PWM极性交换位，正半周配置
                if (!BurstMode) {  // 非Burst模式
                    PG1IOCONLbits.OVRENH = 1; //0; // 上管PWM强制关闭
                    if (SoftStartState == 2)  // 如果软启动状态为2（正常运行）
                        PG1IOCONLbits.OVRENL = 0; // 下管PWM使能（正半周下管工作）
                    else 
                        PG1IOCONLbits.OVRENL = 1; // 否则下管关闭（软启动期间）
                }
                SoftStart_ZC = 1;
                PWM_Counter = 1;
                PWMUpdateStop = 0;
                ZCRange = 0;
            }
            //ZC Detecting-------------------------------------------------------

            ////PWM soft start after ZC----------------------------------------------
            if (!BurstMode) { //确保在Burst模式下不执行软启动，避免冲突。
                if (SoftStart_ZC) { //SoftStart_ZC标志表示需要执行软启动（在过零检测中设置）。
                    if (PWM_Counter < 12) { //软启动持续12个PWM周期，约300μs（25μs × 12）
                        if (VACPol) {  // 正半周
                            PG1IOCONLbits.OVRENH = 1; //PWM OFF
                            PG1IOCONLbits.OVRENL = 0; //PWM ON
                        } else { // 负半周
                            PG1IOCONLbits.OVRENL = 1; //PWM OFF
                            PG1IOCONLbits.OVRENH = 0; //PWM ON
                        }
                        //渐进式衰减：电流环输出逐步减小; 避免积分饱和：防止启动时电流环输出过大
                        IacLoopOutput = IacLoopOutput - (IacLoopOutput >> 6); //IacLoopOutput = IacLoopOutput × (1 - 1/64) = IacLoopOutput × 63/64
                        if(PWM_Counter < 7){
                            MDC = (PWM_Counter << 8); // 阶段1：较慢增加
                        }else{
                            MDC = (PWM_Counter << 10); // 阶段2：较快增加
                        }
                        if ((MDC > CURTMDC)) { //如果软启动占空比超过目标值，提前结束
                            PWM_Counter = 20; // 提前结束软启动
                        }
                    } else {
                        SoftStart_ZC = 0; //check startup must be OFF 
                    }
                }
            }
            ////PWM soft start after ZC----------------------------------------------

            ///////voltage loop control------------------------------------------
            static UINT08 ISRCountVLOOP; // 静态计数器，记录中断次数
            if ((ISRCountVLOOP++ > 10) && !ZCRange && !CompFreeze) 
            { // 每11个ADC中断周期执行一次（降频）  // 不在过零区间内（避免过零干扰） // 比较器未冻结（非Burst模式）
                VoltageLoopControl();
                ISRCountVLOOP = 0;
            }
            ///////voltage loop control------------------------------------------

            //burst mode------------------------------------------------------------
            static uint16_t BurstActiveCounter = 0; // 静态变量，Burst激活计数器（用于退出判断）
            // 条件1：电压环输出小于10（轻载指示）  // 条件2：当前不在Burst模式 // 条件3：软启动已完成
            if ((VbulkLoopOutput < 10) && (!BurstMode) && (SoftStartState == 2)) {
                BurstModeDetect = 1; // 设置Burst模式检测标志
            }
            if (BurstModeDetect) {   // 检测到Burst模式请求
                BurstModeDetect = 0;  // 清除检测标志（单次触发）
                BurstMode = 1; // ★设置Burst模式标志★
                PG1IOCONLbits.OVRENH = 1; //PWM OFF  // 硬件强制覆盖，立即生效 不受软件延迟影响
                PG1IOCONLbits.OVRENL = 1; //PWM OFF
                oGATE_NL = 0;  // 下管驱动输出置低
                oGATE_NH = 0;  // 上管驱动输出置低
                VbulkLoopOutput = 10;  // 保持最小电压环输出
                IacLoopOutput = 0;  // 电流环输出清零
                CompFreeze = 1;         // ★冻结比较器（停止PI计算）★         
            }
            //这段代码实现了PFC变换器Burst模式的退出机制，采用自适应电压阈值+过零计数的混合策略，确保Burst模式平滑退出。
            //BurstMode：仅在Burst模式下计数
            //VACPol_last == 1 && VACPol == 0：检测交流电压正半周到负半周的过零点
            //BurstCounter：记录Burst期间经历的过零周期数,过零时电压最低，开关冲击最小
            if (BurstMode && (VACPol_last == 1) && (VACPol == 0)) {
                BurstCounter++;
            }

            if (VbulkFiltered > 3738)//400*1.05
                dffOff = 1; //	dffOff强制退出
            else if (VbulkFiltered > 3667)//400*1.03 
                BurstActiveCounter = 4; //4个半周（约40ms@50Hz）
            else if (VbulkFiltered > 3596)//400*1.01
                BurstActiveCounter = 2; //2个半周（约20ms@50Hz）
            else
                BurstActiveCounter = 1;

            if (BurstCounter > BurstActiveCounter) {
                BurstMode = 0; // ★清除Burst模式标志★
                BurstCounter = 0; // 重置过零计数器
                CompFreeze = 0; //1_2_20 // ★解冻比较器，恢复PI计算★
                dffOff = 0; // 清除立即退出标志
            }
            //burst mode------------------------------------------------------------

            //current loop control 避免过零时电流给定突变 	Burst模式时跳过计算
            if (!ZCRange && !CompFreeze) {
                //current refence caculate
                if (vin_squared_average < VAC_BROWNOUT_SQURE) {
                    VrmsSqure = VAC_BROWNOUT_SQURE;  // 欠压保护，使用最小值
                } else {
                    VrmsSqure = vin_squared_average; // 正常使用计算值
                }
                //电流给定 = (电压环输出 × 瞬时电压波形) / 电压有效值²= (功率需求 × 电压波形) / 电压归一化
                SETCURRCURT = __builtin_muluu(VbulkLoopOutput, (cir_buff[emi_pointer] << 3)); //Q15 * Q12 << 3   =Q30    
                SETCURRCURT = __builtin_divud(SETCURRCURT, VrmsSqure); //Q30 /Q15 =Q15
                //过流保护：限制最大电流给定，防止过载。
                if (SETCURRCURT > LMTCURR) {
                    SETCURRCURT = LMTCURR;
                }
                //current loop PI control----------------------------------------------------------
                //功能：执行增量式PI算法，输出IacLoopOutput（Q15格式）。
                CurrentLoopControl();
                //current loop PI control-----------------------------------------------------------

                //Q15 Duty to PWM MDC---------------------------------------------------------------   
                //CURTMDC = IacLoopOutput × PWM_PERIOD / 32768
                //= Q15 × 周期 / 32768
                //= 实际占空比计数值     
                CURTMDC = (UINT16) (__builtin_muluu(IacLoopOutput, PWM_PERIOD) >> 15);
                安全保护：限制占空比在有效范围内，防止极端值。
                if (CURTMDC > PWM_DUTY_MAX) {
                    CURTMDC = PWM_DUTY_MAX;
                } else if (CURTMDC < PWM_DUTY_MIN) {
                    CURTMDC = PWM_DUTY_MIN;
                }
               //过零期间暂停更新,不在软启动 
                if ((!PWMUpdateStop) && (!SoftStart_ZC)) {
                    MDC = CURTMDC;
                }
                if(MDC >2){   // 占空比大于2时，正常设置
                    PG1TRIGA = MDC >> 1; //可能是半周期模式或移相控制
                }else{
                    PG1TRIGA = 1;   // 占空比过小时，设为最小值1
                }
            }
//            //slow MOS control--------------------
//            if ((SoftStartState == 2)&&(!BurstMode)) {
//                if (VacConv > 200) {
//                    if (IacConv > 130) {
//                        if (VACPol) {
//                            oGATE_NH = 0;
//                            oGATE_NL = 1;
//                        } else {
//                            oGATE_NL = 0;
//                            oGATE_NH = 1;
//                        }
//                    } else if (IacConv < 50) {
//                        oGATE_NH = 0;
//                        oGATE_NL = 0;
//                    }
//                } else {
//                    oGATE_NH = 0;
//                    oGATE_NL = 0;
//                }
//            }
// 两个慢管（或辅助管）始终关闭，完全依靠快管的体二极管。
            oGATE_NH = 0;
            oGATE_NL = 0;
        } else {
            IacLoopOutput = 0;
            IacI = 0;
            VbulkLoopOutput = 0;
        }
    }

    //Fast Protect  25us/time
    //VAC cycle drop decteing 电网正常 → 突然掉电 → VacConv<100 → 4ms后VACDROP保护 → PFC关闭
    static UINT16 ACDropCnt;
    if (VacConv < 100) { // 输入电压差<100（约10V），判断为电压跌落
        ACDropCnt++;
        if (ACDropCnt > 40) { // 40*25us = 1ms延时
            // IacLoopOutput = 0; // 被注释：立即清零电流环（过于激进）
            // IacI = 0;  // 被注释：清零积分项
            // VbulkLoopOutput = 0; // 被注释：清零电压环
        }
        if (ACDropCnt > 160) { //160*25us  // 160*25us = 4ms延时 去抖动考虑：4ms确认后再动作更可靠
            ACDropCnt = 0; // 计数器清零
            PROTECT |= VACDROP;  // ★设置交流跌落保护标志★
        }

    } else { // 电压正常（VacConv >= 100）
        ACDropCnt = 0; // 计数器清零
    }
    ////////////////////////////////////////////////////////////////////////////IAC fast OCP
    //实现了PFC输入电流的快速过流保护，当输入电流超过设定阈值并持续一定时间后，触发保护动作。
    static UINT08 TempCnt2; //TempCnt2：记录电流超过阈值的连续次数
    if ((IacConv > IacOCPFast_AD)) { // 输入电流超过快速过流阈值
        if (TempCnt2 < 100) { // 计数器未满100
            TempCnt2++; // 计数器递增
        } else {  // 计数器达到100
            PROTECT |= IACOCP; // ★设置输入电流过流保护标志★
        }
    } else {
        TempCnt2 = 0;
    }
    ///////////////////////////////////////////////////////////////////////////Fast BULK OVP
    //实现了PFC母线电压的快速过压保护，当母线电压超过设定阈值并持续一定时间后，触发保护动作，防止母线电容和功率器件过压损坏
    static UINT08 TempCnt3;
    if ((VBULK_ADCBUF > VbulkOVPFast_AD)) {// 母线电压超过快速过压阈值
        if (TempCnt3 < 100) {  // 计数器未满100
            TempCnt3++;
        } else {
            PROTECT |= VBULKOVP;   // ★设置母线过压保护标志★
        }
    } else {
        TempCnt3 = 0;
    }

    ////////////////////////////////////////////////////////////////////////BOOST Fast Vout UVP
    if (ONTIMECNT >= 850) { // 系统启动850*20us=17ms后才检测
        static UINT08 TempCnt4, TempCnt5;
        if (SoftStartState == 1) { //UVP while softstart  
            if (CURTERRORCV > 400) { //voltage error > 45V  基于电压误差而非绝对电压
                if (TempCnt4 < 200) { //debounce time = 200*25us
                    TempCnt4++;
                } else {
                   // PROTECT |= VBULKUVP;
                    TempCnt4 = 0;
                }
            } else {
                TempCnt4 = 0;
            }
        } else if (SoftStartState == 2) {///////////////////UVP while PSU ON
            if (VBULK_ADCBUF < VbulkUVPFast_AD) {
                if (TempCnt5 < 200) {
                    TempCnt5++;
                } else {
                    //PROTECT |= VBULKUVP;
                }
            } else {
                TempCnt5 = 0;
            }
        }
    }
}



//stuff for Vin
static UINT32 vin_squared, iin_squared;
// stuff for half cycle measurement
static UINT32 negative_vin_squared_accumulate;
static UINT32 positive_vin_squared_accumulate;
static UINT32 iin_squared_accumulate;

/**
 * @brief 清除负半周期累积变量
 * 
 * 该函数重置负半周期相关的计数器和累加器，用于在过零点切换时
 * 清除负半周期的累积数据，为下一个正半周期的计算做准备。
 * 
 * @note 此函数通常在检测到过零点后调用，确保正负半周期的计算独立进行
 */
void clear_negative_accumulators(void) {
    negative_cycle_counter = 0;
    negative_vin_squared_accumulate = 0;
}

/**
 * @brief 清除正半周累积计数器
 * 
 * 该函数重置正半周相关的累积变量，包括：
 * - 正半周周期计数器
 * - 输入电压平方累积值
 * 
 * 通常在完成一个正半周期的计算后调用，为下一个正半周期
 * 的数据采集和计算做准备。
 */
void clear_positive_accumulators(void) {
    positive_cycle_counter = 0;
    positive_vin_squared_accumulate = 0;
}

/**
 * @brief 存储负半周期的计算值
 * 
 * 该函数根据负半周期计数器计算并存储以下值：
 * - 电网频率（GETFREQ）：基于200000周期数计算
 * - 输入电压平方的平均值
 * - 输入电流平方的平均值
 * 
 * 计算完成后，重置输入电流平方累加器为0。
 * 当计算出的频率低于392时，将频率置为0。
 * 
 * @note 使用GCC内置函数__builtin_divud进行无符号除法运算
 * @note 依赖于全局变量：negative_cycle_counter、negative_vin_squared_accumulate、iin_squared_accumulate
 * @note 修改全局变量：GETFREQ、vin_squared_average、iin_squared_average、iin_squared_accumulate
 */
void store_negative_cycle_values(void) {
    GETFREQ = (UINT16) (__builtin_divud(200000, negative_cycle_counter));
    if (GETFREQ < 392) { //	若频率<392Hz（约50Hz的7.8倍），则清零 
        GETFREQ = 0;
    }
    vin_squared_average = __builtin_divud(negative_vin_squared_accumulate, negative_cycle_counter); 
    iin_squared_average = __builtin_divud(iin_squared_accumulate, negative_cycle_counter);
    iin_squared_accumulate = 0;

}

/**
 * @brief 存储正半周期的测量值并计算平均值
 * 
 * 该函数在交流输入电压的正半周期结束时被调用，用于：
 * - 计算并更新输入频率（基于正半周期采样点数）
 * - 计算正半周期输入电压平方的平均值（用于RMS计算）
 * - 计算正半周期输入电流平方的平均值（用于RMS计算）
 * - 清零电流平方累加器，为下一周期做准备
 * 
 * @note 频率计算基于200kHz采样频率，若计算值低于392Hz则视为无效
 * @note 电压平方累加器（positive_vin_squared_accumulate）需在外部清零
 * @note 该函数与 store_negative_cycle_values() 配合使用，交替处理正负半周期
 * 
 * @return 无
 */
void store_positive_cycle_values(void) {

    GETFREQ = (UINT16) (__builtin_divud(200000, positive_cycle_counter));
    if (GETFREQ < 392) {
        GETFREQ = 0;
    }
    vin_squared_average = __builtin_divud(positive_vin_squared_accumulate, positive_cycle_counter);
    iin_squared_average = __builtin_divud(iin_squared_accumulate, positive_cycle_counter);
    iin_squared_accumulate = 0;

}

/**
 * @brief 累加负半周期的电压和电流平方值
 * 
 * 在交流输入负半周期内，对瞬时电压平方值和瞬时电流平方值进行累加，
 * 用于计算负半周期的有效值（RMS）。累加结果将在负半周期结束时用于
 * 计算平方的平均值，进而通过开平方得到电压和电流的有效值。
 * 
 * @note 使用定点数格式：vin_squared 为 Q15 格式，累加后为 Q24 格式
 * @note 此函数在每个采样周期（25μs）调用一次
 * @note 累加器在负半周期结束时由 store_negative_cycle_values() 清零
 * 
 * @see store_negative_cycle_values() 负半周期结束时的处理函数
 * @see accumulate_positive_cycle_values() 正半周期的累加函数
 */
void accumulate_negative_cycle_values() {
    negative_vin_squared_accumulate = vin_squared + negative_vin_squared_accumulate; //Q15 * Q9 =Q24
    iin_squared_accumulate = iin_squared + iin_squared_accumulate;
}

/**
 * @brief 累加AC正半周期的电压和电流平方值，用于RMS计算
 * 
 * 该函数在每个PWM中断周期中被调用，累加正半周期内的电压平方值和电流平方值。
 * 累加结果将在正半周期结束时用于计算输入电压和电流的RMS值。
 * 
 * @note 电压平方值使用独立的累加器(positive_vin_squared_accumulate)，
 *       电流平方值与负半周期共用累加器(iin_squared_accumulate)
 * @note vin_squared为Q15格式，累加结果为Q24格式
 * 
 * @warning 函数内部未进行溢出保护，需确保累加器不会溢出
 * 
 * @param 无
 * 
 * @return 无
 */
void accumulate_positive_cycle_values() {
    positive_vin_squared_accumulate = vin_squared + positive_vin_squared_accumulate; //Q15 * Q9 =Q24
    iin_squared_accumulate = iin_squared + iin_squared_accumulate;
}

/**
 * @brief AC信号半周期处理函数
 * @details 该函数与AC信号同步，执行以下操作：
 *          - 检测电压和电流极性，进行整流处理
 *          - 对电压和电流信号进行低通滤波
 *          - 计算电压和电流的平方值（用于RMS计算）
 *          - 管理正负半周期的计数器和累加器
 *          - 处理半周期数据的存储和清除
 *          - 实现过零检测和毛刺过滤逻辑
 * @note 该函数在PWM中断中周期性调用，使用循环缓冲区实现EMI滤波延迟
 * @see store_positive_cycle_values()
 * @see store_negative_cycle_values()
 * @see accumulate_positive_cycle_values()
 * @see accumulate_negative_cycle_values()
 */
void half_cycle_processing() //processing that syncs with AC signal to filter it out.
{
    if (VACL_ADCBUF > VACN_ADCBUF)//this is the cycle for line
    {
        VacConv = VACL_ADCBUF - VACN_ADCBUF;  // 正半周
        positive = 1;

    } else //cycle for neutral
    {
        VacConv = VACN_ADCBUF - VACL_ADCBUF; // 负半周
        positive = 0;
    }

    if (IAC_ADCBUF > IacZeroAD) { //IacZeroAD = 2047（12位ADC中点）
        IacConv = IAC_ADCBUF - IacZeroAD;
    } else {
        IacConv = IacZeroAD - IAC_ADCBUF;
    }
    //等效截止频率：fc = fs/2π ≈ 6.4kHz (@40kHz采样)
    VacConvFiltered = ((VacConvFiltered + VacConv) >> 1); //TESTen
    IacConvFiltered = ((IacConvFiltered + IacConv) >> 1); //TESTen

    cir_buff[cir_buff_ptr] = VacConvFiltered;
    emi_pointer = (cir_buff_ptr - 15) & 0x7f; //get pointer to delayed signal // 15点延迟（375μs）
    cir_buff_ptr = (cir_buff_ptr + 1) & 0x7f; // 128点环形缓冲

    vin_squared = (VacConv * VacConv) >> 9; //Q12 * Q12 >>9 = Q15	 // 12位ADC，Q12格式，Q15格式
    iin_squared = (IacConv * IacConv) >> 9; //Q12 * Q12 >>9 = Q15   // 12位ADC，Q12格式，Q15格式    
    if (positive == 1) { // 正半周
        positive_cycle_counter++; // 正半周计数器加1
        if (positive_cycle_counter == 16) { // 正半周计数器达到16，开始处理
        }
        // 正半周计数器达到16，且负半周计数器大于80，则处理之前的负半周结果
        if ((positive_cycle_counter == 16) && (negative_cycle_counter >= 80)) 
            //if it really is a positive cycle and not a glitch, handle previous negative cycle results
            //if they are valid
        {
            store_negative_cycle_values();   // 强制保存已有数据
            clear_negative_accumulators();   // 强制重启

        } else if (positive_cycle_counter > 16) {
            clear_negative_accumulators(); //to deal with possible negative glitches
            if (positive_cycle_counter >= 511) {//here if pulse too long, just take what we've got and start over
                store_positive_cycle_values(); 
                clear_positive_accumulators();
            }
        }
        accumulate_positive_cycle_values();
    } else {
        negative_cycle_counter++;
        if ((negative_cycle_counter == 16) && (positive_cycle_counter >= 80))
            //if it really is a negative cycle, handle previous positive cycle
            //if it was valid
        {
            store_positive_cycle_values();
            clear_positive_accumulators();

        } else if (negative_cycle_counter > 16) {
            clear_positive_accumulators();

            if (negative_cycle_counter >= 511) {//here if pulse too long, just take what we've got and start over
                store_negative_cycle_values();
                clear_negative_accumulators();
            }
        }
        accumulate_negative_cycle_values();
    }
}


#define VbulkKp    6000            // << 15
#define VbulkKi    100            // << 15

#define VbulkKpL1  18000         // << 15
#define VbulkKiL1  2000          // << 15

#define VbulkKpL2  6364         // << 15
#define VbulkKiL2  1599          // << 15

#define VbulkKpSoftStart  3276            // << 15
#define VbulkKiSoftStart  819            // << 15

/**
 * @brief 电压环控制函数
 * 
 * 实现PFC母线电压的闭环控制，采用PI控制算法调节母线电压。
 * 根据误差大小选择不同的PI参数，大误差时使用低增益参数L1以避免超调。
 * 
 * @note 误差计算采用Q15定点数格式（原始Q12左移3位）
 * @note PI计算采用Q15定点数运算，乘法结果右移15位保持精度
 */

void VoltageLoopControl(void) {
    SINT32 SUM;
    CURTERRORCV = (SINT32) (SETVOLTCURT - VbulkFiltered) << 3; //Q12 << 3 = Q15
    //CURTERRORCV 当前误差; LASTERRORCV 上次误差
    if(CURTERRORCV < -150){  // 大负误差（电压远高于设定值）
        SUM = (((SINT32) (CURTERRORCV - LASTERRORCV) * (SINT32)VbulkKpL1) >> 15) //SUM = Q15 * Q15 >>15 =Q15
            + (((SINT32) CURTERRORCV * (SINT32)VbulkKiL1) >> 15)
            + VbulkLoopOutput;
    }else{
        SUM = (((SINT32) (CURTERRORCV - LASTERRORCV) * (SINT32)VbulkKp) >> 15) //SUM = Q15 * Q15 >>15 =Q15
            + (((SINT32) CURTERRORCV * (SINT32)VbulkKi) >> 15)
            + VbulkLoopOutput;
    }
    LASTERRORCV = CURTERRORCV;  // 保存当前误差，用于下次微分计算

    if (SUM < 0) {
        VbulkLoopOutput = 0; // 下限幅：最小输出为0
    } else if (SUM > 32766) {
        VbulkLoopOutput = 32766;  // 上限幅：最大输出为32766（接近Q15最大值32767）
    } else {
        VbulkLoopOutput = SUM;
    }
}

#define IacKp    2300//2500//4800//4800  // k << 12                4.645
#define IacKi    1100//1300//1920//1920   // 0.125*k <<12           0.584

/**
 * @brief 电流环控制函数
 * 
 * 实现PFC系统的电流环控制，采用增量式PI控制算法调节输入电流，
 * 使其跟踪电压环输出的参考电流，实现功率因数校正。
 * 
 * @details 控制算法包括：
 * - 电流误差计算：SETCURRCURT为参考电流，IacConv为采样电流
 * - 增量式PI计算：比例项基于误差变化量，积分项基于当前误差
 * - 输出限幅：限制在[0, 32766]范围内，防止PWM输出饱和
 * 
 * @note 
 * - 数据格式：Q15定点数格式
 * - IacConv左移3位转换为Q15格式
 * - 比例系数IacKp和积分系数IacKi为Q12格式
 * - 调用周期：在ADC中断中周期性执行
 * 
 * @param void 无参数
 * 
 * @return void 无返回值
 * 
 * @retval 无
 * 
 * @see SETCURRCURT - 电压环输出的参考电流
 * @see IacConv - 交流输入电流采样值
 * @see IacKp - 电流环比例系数
 * @see IacKi - 电流环积分系数
 * @see IacLoopOutput - 电流环输出，用于PWM占空比控制
 */
void CurrentLoopControl(void) {
    SINT32 SUM;
    CURTERRORCC = SETCURRCURT - (IacConv << 3) + 100; //Q15 - Q12 << 3 = Q15
    SUM = (((SINT32) (CURTERRORCC - LASTERRORCC) * (SINT32)IacKp) >> 12) //SUM = Q15 * Q12 >>12 =Q15
            + (((SINT32) CURTERRORCC * (SINT32)IacKi) >> 12)
            + IacLoopOutput;
    LASTERRORCC = CURTERRORCC;

    if (SUM < 0) {
        IacLoopOutput = 0;
    } else if (SUM > 32766) {
        IacLoopOutput = 32766;
    } else {
        IacLoopOutput = SUM;
    }

    //     PG3DC = SETCURRCURT>>1;
}

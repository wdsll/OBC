//###########################################################################
//
//###########################################################################

//
// Included Files
//
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "DeviceInit_28069.h"  

#pragma CODE_SECTION(cpu_timer1_isr,"ramfuncs")
#pragma CODE_SECTION(epwm1_timer_isr,"ramfuncs")
#pragma CODE_SECTION(epwm1_tzint_isr,"ramfuncs")
#pragma CODE_SECTION(AdcOffsetSelfCal,"ramfuncs")



__interrupt void cpu_timer1_isr(void);
__interrupt void epwm1_timer_isr(void);
__interrupt void epwm1_tzint_isr(void);

#define  SYSTEM_CLOCK_PERIOD  (90000000)//90M
//PWM related define
#define PWMPeriodMax      (125000)//125K
#define PWMPeriodMin      (250000)//250K
#define PWMDeadtime       18    //200ns

OBC_INFO_Data_HV OBCinfoData_HV;
char cpuTimer_1msFalg = 0;
char cpuTimer_20usFlag = 0;
void vEpwm_PinConfig_Enable(void)
{
    EALLOW;

    GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;   // SET LOW
    GpioDataRegs.GPASET.bit.GPIO1 = 1;   // SET High
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;   // GPIO = output
    GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;   // GPIO = output

       /* GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;    // Disable pull-up on GPIO0 (EPWMA)
        GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;    // Disable pull-up on GPIO1 (EPWM4B)
        GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;   // Configure GPIO0 as EPWM1A
        GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;   // Configure GPIO1 as EPWM1B*/

        GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;    // Disable pull-up on GPIO0 (EPWMA)
        GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;    // Disable pull-up on GPIO1 (EPWMB)
        GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;   // Configure GPIO0 as EPWM2A
        GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;   // Configure GPIO1 as EPWM2B

        GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;    // Disable pull-up on GPIO0 (EPWMA)
        GpioCtrlRegs.GPAPUD.bit.GPIO5 = 1;    // Disable pull-up on GPIO1 (EPWMB)
        GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;   // Configure GPIO0 as EPWM3A
        GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;   // Configure GPIO1 as EPWM3B

        GpioCtrlRegs.GPAPUD.bit.GPIO6 = 1;    // Disable pull-up on GPIO0 (EPWMA)
        GpioCtrlRegs.GPAPUD.bit.GPIO7 = 1;    // Disable pull-up on GPIO1 (EPWMB)
        GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1;   // Configure GPIO0 as EPWM4A
        GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 1;   // Configure GPIO1 as EPWM4B

    EDIS; 
}

void vEpwm_PinConfig_Disable(void)
{
    EALLOW;

        GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;   // SET LOW
        GpioDataRegs.GPASET.bit.GPIO1 = 1;   // SET High
        GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;   // GPIO = output
        GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;   // GPIO = output

        GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;   // SET LOW
        GpioDataRegs.GPASET.bit.GPIO3 = 1;   // SET HIGH
        GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;   // GPIO = output 
        GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;   // GPIO = output 

        GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;   // SET LOW
        GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;   // SET LOW
        GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;   // GPIO = output
        GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;   // GPIO = output

        GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;   // SET LOW
        GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;   // SET LOW
        GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;   // GPIO = output
        GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;   // GPIO = output

        ////////SET PWM IO as GPIO
        GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;   // Configure GPIO0 as GPIO
        GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;   // Configure GPIO1 as GPIO

        GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;   // Configure GPIO0 as GPIO
        GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;   // Configure GPIO1 as GPIO

        GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;   // Configure GPIO0 as GPIO
        GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0;   // Configure GPIO1 as GPIO

        GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;   // Configure GPIO0 as GPIO
        GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0;   // Configure GPIO1 as GPIO

    EDIS; 
}

void vEpwm_SetParameter(Uint16 Period, Uint16 Duty,Uint16 TDelay)
{
    EALLOW;
    EPwm1Regs.TBPRD = Period;
    EPwm1Regs.CMPA.half.CMPA = Duty;//Set 50% fixed duty

    EPwm2Regs.TBPRD = Period;
    EPwm2Regs.CMPA.half.CMPA = Duty;//Set 50% fixed duty
    EPwm2Regs.TBPHS.half.TBPHS = Period - TDelay;

    EPwm3Regs.TBPRD = Period;
    EPwm3Regs.CMPA.half.CMPA = Duty;//Set 50% fixed duty

    EPwm4Regs.TBPRD = Period;
    EPwm4Regs.CMPA.half.CMPA = Duty;//Set 50% fixed duty

    EPwm1Regs.CMPB = Period >> 2;//CMPB to triger ADC
    EDIS;  
}

Uint16 Get_Epwm1_2_PER(void)
{
    return EPwm1Regs.TBPRD;
}

void vEpwm1Config(void)
{
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;    // Disable pull-up on GPIO0 (EPWM1A)
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;    // enable pull-up on GPIO1 (EPWM1B)
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;   // GPIO = output
    GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;   // GPI1 = output

    //TBCLK = SYSCLKOUT / (HSPCLKDIV × CLKDIV)=100MHz/(HSPCLKDIV * CLKDIV)
    //Fpwm=TBCLK/TBPRD
    EPwm1Regs.TBPRD = 360;//(SYSTEM_CLOCK_PERIOD/PWMPeriodMin);           // Set timer period:Period 250K
    EPwm1Regs.CMPA.half.CMPA = EPwm1Regs.TBPRD >> 1;//Set 50% fixed duty
    EPwm1Regs.CMPB = EPwm1Regs.TBPRD >> 2;//CMPB to triger ADC
    EPwm1Regs.TBPHS.half.TBPHS = 0;   // Set Phase register to zero
    EPwm1Regs.TBCTR = 0;       // Clear counter

    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;   // slave module
    EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;    // PRD shadow
    EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO; // sync with

    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR=Zero
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR=Zero

    EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET; // Set PWM1A on Zero
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Clear PWM1A on event A, up count
    EPwm1Regs.AQCTLB.bit.ZRO = AQ_CLEAR;  // clear PWM1B on Zero
    EPwm1Regs.AQCTLB.bit.CAU = AQ_SET;// set PWM1B on event A, up count

    EPwm1Regs.DBCTL.bit.IN_MODE = DBB_ALL; // EPWMxA is the source for both delays
    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
    EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HI;//DB_ACTV_HIC; // Active Hi complementary
    EPwm1Regs.DBRED = 62;//PWMDeadtime;//PWMDeadtime*TBCLK
    EPwm1Regs.DBFED = 62;//PWMDeadtime;

    EPwm1Regs.ETSEL.bit.SOCBEN = 1;//allow EPWMxSOCA triger ADC convert
    EPwm1Regs.ETSEL.bit.SOCBSEL = ET_CTRU_CMPB;//allow
    EPwm1Regs.ETPS.bit.SOCBPRD = 1;
    EPwm1Regs.ETPS.bit.SOCBCNT = ET_2ND;
    EPwm1Regs.ETCLR.bit.SOCB = 1;//clear triger event


    EPwm1Regs.ETSEL.bit.INTSEL = ET_CTRU_CMPB;     // Select INT on Zero event
    EPwm1Regs.ETSEL.bit.INTEN = 1;                // Enable INT
    EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;           // Generate INT on 3rd event

    PieVectTable.EPWM1_INT = &epwm1_timer_isr;
    // Enable CPU INT3 which is connected to EPWM1-3 INT
    IER |= M_INT3;
    // Enable EPWM1 in the PIE: Group 3 interrupt 1
    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;


    //epwm1 TZ INT
    PieVectTable.EPWM1_TZINT = &epwm1_tzint_isr;
    IER |= M_INT2;
    PieCtrlRegs.PIEIER2.bit.INTx1 = 1;

    // Enable TZ3 as one shot trip sources
    EPwm1Regs.TZSEL.bit.OSHT3 = 1;
    EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO;
    EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_HI;
    EPwm1Regs.TZEINT.bit.OST = 1; // Enable TZ interrupt


/*
    EPwm1Regs.HRCNFG.all = 0x0;
    EPwm1Regs.HRCNFG.bit.EDGMODE = HR_BEP;    // MEP control on both edges
    EPwm1Regs.HRCNFG.bit.CTLMODE = HR_CMP;    // CMPAHR and TBPRDHR HR control
    EPwm1Regs.HRCNFG.bit.HRLOAD  = HR_CTR_ZERO_PRD;   // load on CTR = 0 and CTR = TBPRD
    EPwm1Regs.HRCNFG.bit.AUTOCONV = 1;    // Enable autoconversion for HR period
    EPwm1Regs.HRPCTL.bit.TBPHSHRLOADE = 1;    // Enable TBPHSHR sync (required for updwn count HR control)
    EPwm1Regs.HRPCTL.bit.HRPE = 1;    // Turn on high-resolution period control
*/
    EDIS;    
}


void vEpwm2Config(void)
{
    EALLOW;

    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;    // Disable pull-up on GPIO0 (EPWM1A)
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;    // enable pull-up on GPIO1 (EPWM1B)
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;   // GPIO = output
    GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;   // GPIO = output
    //TBCLK = SYSCLKOUT / (HSPCLKDIV × CLKDIV)=90MHz/(HSPCLKDIV * CLKDIV)
    //Fpwm=TBCLK/TBPRD
    EPwm2Regs.TBPRD = (SYSTEM_CLOCK_PERIOD/PWMPeriodMin);           // Set timer period:Period 250K
    EPwm2Regs.CMPA.half.CMPA = EPwm2Regs.TBPRD >> 1;//Set 50% fixed duty
    EPwm2Regs.TBPHS.half.TBPHS = EPwm2Regs.TBPRD - 1;   // Set Phase register to zero
    EPwm2Regs.TBCTR = 0;       // Clear counter

    EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
    EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
    EPwm2Regs.TBCTL.bit.PHSEN = TB_ENABLE;   // slave module
//    EPwm2Regs.TBCTL.bit.PHSDIR = TB_UP;
    EPwm2Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;    // epwm2 PRD must be immediately updated, otherwise PWM2 will loss when PRD load
    EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN; // sync with epwm4

    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR=Zero
    EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR=Zero

    EPwm2Regs.AQCTLA.bit.ZRO = AQ_SET; // Set PWM4A on Zero
    EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Clear PWM4A on event A, up count
    EPwm2Regs.AQCTLB.bit.ZRO = AQ_CLEAR; // ClearSet PWM4A on Zero
    EPwm2Regs.AQCTLB.bit.CAU = AQ_SET; // set PWM4A on event A, up count

    EPwm2Regs.DBCTL.bit.IN_MODE = DBA_ALL; // EPWMxA is the source for both delays
    EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
    EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HI;//DB_ACTV_HIC; // Active Hi complementary
    EPwm2Regs.DBRED = PWMDeadtime;//PWMDeadtime*TBCLK
    EPwm2Regs.DBFED = PWMDeadtime;


    EPwm2Regs.TZSEL.bit.OSHT3 = 1;
    EPwm2Regs.TZCTL.bit.TZA = TZ_FORCE_LO;
    EPwm2Regs.TZCTL.bit.TZB = TZ_FORCE_HI;

    EDIS;    
}

void vEpwm3Config(void)
{
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;    // Disable pull-up on GPIO0 (EPWM3A)
    GpioCtrlRegs.GPAPUD.bit.GPIO5 = 1;    // Disable pull-up on GPIO1 (EPWM3B)
    GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;   // GPIO = output
    GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;   // GPIO = output
    //TBCLK = SYSCLKOUT / (HSPCLKDIV × CLKDIV)=100MHz/(HSPCLKDIV * CLKDIV)
    //Fpwm=TBCLK/TBPRD

    EPwm3Regs.TBPRD = (SYSTEM_CLOCK_PERIOD/PWMPeriodMin);           // Set timer period:Period 250K
    EPwm3Regs.CMPA.half.CMPA = EPwm3Regs.TBPRD >> 1;//Set 50% fixed duty
    EPwm3Regs.TBPHS.half.TBPHS = 0;//EPwm3Regs.TBPRD >> 1;   // Set Phase register to zero
    EPwm3Regs.TBCTR = 0;       // Clear counter

    EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
    EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
    EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;   // slave module
    EPwm3Regs.TBCTL.bit.PRDLD = TB_SHADOW;    // PRD shadow
    EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN; // sync with Zero

    EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR=Zero
    EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR=Zero

    EPwm3Regs.AQCTLA.bit.ZRO = AQ_SET; // Set PWM4A on Zero
    EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Clear PWM4A on event A, up count
    EPwm3Regs.AQCTLB.bit.ZRO = AQ_CLEAR; // Clear PWM4A on Zero
    EPwm3Regs.AQCTLB.bit.CAU = AQ_SET; // SET PWM4A on event A, up count

    EPwm3Regs.DBCTL.bit.IN_MODE = DBA_ALL; // EPWMxA is the source for both delays
    EPwm3Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
    EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary
    EPwm3Regs.DBRED = 14;//9;//PWMDeadtime;//PWMDeadtime*TBCLK
    EPwm3Regs.DBFED = 14;//9;//PWMDeadtime;

    EPwm3Regs.TZSEL.bit.OSHT3 = 1;
    EPwm3Regs.TZCTL.bit.TZA = TZ_FORCE_LO;
    EPwm3Regs.TZCTL.bit.TZB = TZ_FORCE_LO;

    EDIS;    
}

//EPWM4 is master pwm while charging
void vEpwm4Config(void)
{
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO6 = 1;    // Disable pull-up on GPIO0 (EPWM4A)
    GpioCtrlRegs.GPAPUD.bit.GPIO7 = 1;    // Disable pull-up on GPIO1 (EPWM4B)
    GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;   // GPIO = output
    GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;   // GPIO = output
    //TBCLK = SYSCLKOUT / (HSPCLKDIV × CLKDIV)=100MHz/(HSPCLKDIV * CLKDIV)
    //Fpwm=TBCLK/TBPRD

    EPwm4Regs.TBPRD = (SYSTEM_CLOCK_PERIOD/PWMPeriodMin);           // Set timer period:Period 250K
    EPwm4Regs.CMPA.half.CMPA = EPwm4Regs.TBPRD >> 1;//Set 50% fixed duty
    EPwm4Regs.TBPHS.half.TBPHS = 0;   // Set Phase register to zero
    EPwm4Regs.TBCTR = 0x0000;       // Clear counter

    EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
    EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
    EPwm4Regs.TBCTL.bit.PHSEN = TB_DISABLE;   //
    EPwm4Regs.TBCTL.bit.PHSDIR = TB_DOWN; // Count DOWN on sync
    EPwm4Regs.TBCTL.bit.PRDLD = TB_SHADOW;    // PRD shadow
    EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN; // Sync down-stream module


    EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR=Zero
    EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR=Zero

    EPwm4Regs.AQCTLA.bit.ZRO = AQ_SET; // Set PWM4A on Zero
    EPwm4Regs.AQCTLA.bit.CAU = AQ_CLEAR; // Clear PWM4A on event A, up count
    EPwm4Regs.AQCTLB.bit.ZRO = AQ_CLEAR; // Clear PWM4A on Zero
    EPwm4Regs.AQCTLB.bit.CAU = AQ_SET; // SET PWM4A on event A, up count


    EPwm4Regs.DBCTL.bit.IN_MODE = DBB_ALL; // EPWMxA is the source for both delays
    EPwm4Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // enable Dead-band module
    EPwm4Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC; // Active Hi complementary
    EPwm4Regs.DBRED = 14;//9;//PWMDeadtime;//PWMDeadtime*TBCLK
    EPwm4Regs.DBFED = 14;//9;//PWMDeadtime;

    EPwm4Regs.TZSEL.bit.OSHT3 = 1;
    EPwm4Regs.TZCTL.bit.TZA = TZ_FORCE_LO;
    EPwm4Regs.TZCTL.bit.TZB = TZ_FORCE_LO;

    EDIS;    
}

void delayUs(Uint16 n)
{
    Uint16 i;
    Uint32 j;
    for(i=0;i<n;i++)
       for (j = 0; j < 90; j++);    //>= 1us
}

void ADC_Init(void)
{
    InitAdc();  // For this example, init the ADC
    AdcOffsetSelfCal();


    // Configure ADC
    EALLOW;
    AdcRegs.ADCCTL2.bit.ADCNONOVERLAP = 1; // Enable non-overlap mode
    // ADCINT1 trips after AdcResults latch
    AdcRegs.ADCCTL1.bit.INTPULSEPOS	= 1;
    



    AdcRegs.ADCSOC0CTL.bit.CHSEL 	= 3;  // set SOC0 channel select to ADCINA3----VOUT
    AdcRegs.ADCSOC0CTL.bit.TRIGSEL  = 2;//6;  // set SOC0 start trigger on EPWM1A CMPB, due to round-robin SOC0 converts
    AdcRegs.ADCSOC0CTL.bit.ACQPS    = 18;//6;  // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)

    AdcRegs.ADCSOC1CTL.bit.CHSEL    = 2;  // set SOC1 channel select to ADCINA2----IOUT
    AdcRegs.ADCSOC1CTL.bit.TRIGSEL  = 2;//6;  // set SOC1 start trigger on EPWM1A CMPB, due to round-robin SOC0 converts
    AdcRegs.ADCSOC1CTL.bit.ACQPS    = 18;//6;  // set SOC1 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)

    AdcRegs.ADCSOC2CTL.bit.CHSEL    = 4;  // set SOC2 channel select to ADCINA4----IOUT_DIS
    AdcRegs.ADCSOC2CTL.bit.TRIGSEL  = 2;//6;  // set SOC2 start trigger on EPWM1A CMPB, due to round-robin SOC0 converts
    AdcRegs.ADCSOC2CTL.bit.ACQPS    = 6;  // set SOC2 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)

    AdcRegs.ADCSOC3CTL.bit.CHSEL    = 5;  // set SOC3 channel select to ADCINA5----VBUS
    AdcRegs.ADCSOC3CTL.bit.TRIGSEL  = 2;//6;  // set SOC3 start trigger on EPWM1A CMPB, due to round-robin SOC0 converts
    AdcRegs.ADCSOC3CTL.bit.ACQPS    = 6;  // set SOC3 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)

    AdcRegs.ADCSOC4CTL.bit.CHSEL    = 0;  // set SOC4 channel select to ADCINA0----VBAT
    AdcRegs.ADCSOC4CTL.bit.TRIGSEL  = 2;//6;  // set SOC4 start trigger on EPWM1A CMPB, due to round-robin SOC0 converts
    AdcRegs.ADCSOC4CTL.bit.ACQPS    = 6;  // set SOC4 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)

    AdcRegs.ADCSOC5CTL.bit.CHSEL    = 12;  // set SOC4 channel select to ADCINB4----NTC
    AdcRegs.ADCSOC5CTL.bit.TRIGSEL  = 2;//6;  // set SOC4 start trigger on EPWM1A CMPB, due to round-robin SOC0 converts
    AdcRegs.ADCSOC5CTL.bit.ACQPS    = 6;  // set SOC4 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)

    EDIS;
}


//#################################################
// void CAN_Init(void)
//----------------------------------------------
// Initialize the CAN-A port for communications
// with the host.
//----------------------------------------------
void CAN_Init(void)
{
    InitECanaGpio();
    InitECana();
    // Write to the MSGID field of RECEIVE mailboxes MBOX4 - 7
    ECanaMboxes.MBOX0.MSGID.all = ((Uint32)0x301<<18);//transmit id must initial,MSGID.all highest bit is Standard or extern
    ECanaMboxes.MBOX1.MSGID.all = ((Uint32)0x302<<18);//transmit id
    ECanaMboxes.MBOX2.MSGID.all = ((Uint32)0x303<<18);//Receive id 
    ECanaMboxes.MBOX2.MSGCTRL.bit.DLC = 8;
    // Configure Mailboxes 0-3 as Tx, 4-31 as Rx
    // Since this write is to the entire register (instead of a bit
    // field) a shadow register is not required.
    ECanaRegs.CANMD.all = 0xFFFFFF04;
    // Enable 0~7 Mailboxes
    // Since this write is to the entire register (instead of a bit
    // field) a shadow register is not required.
    ECanaRegs.CANME.all = 0x00000007;
}

// InitScibGpio - SCIB-GPIO22-GPIO23
void InitSciBb(void)
{
    EALLOW;

    // Enable internal pull-up for the selected pins
    // Pull-ups can be enabled or disabled disabled by the user.
    // This will enable the pullups for the specified pins.
    GpioCtrlRegs.GPAPUD.bit.GPIO23 = 0; // Enable pull-up for GPIO23 (SCIRXDB)
    GpioCtrlRegs.GPAPUD.bit.GPIO22 = 0; // Enable pull-up for GPIO22 (SCITXDB)

    // Set qualification for selected pins to asynch only
    // Inputs are synchronized to SYSCLKOUT by default.
    // This will select asynch (no qualification) for the selected pins.
    GpioCtrlRegs.GPAQSEL2.bit.GPIO23 = 3;  // Asynch input GPIO23 (SCIRXDB)
    // Configure SCI-B pins using GPIO regs
    // This specifies which of the possible GPIO pins will be SCI functional 
    // Configure GPIO11 for SCIRXDB operation
    GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 3;   
    // Configure GPIO9 for SCITXDB operation
    GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 3;   
    EDIS;

    // 1 stop bit,  No loopback, No parity,8 char bits, async mode,
    // idle-line protocol
    ScibRegs.SCICCR.all =0x0007;
    // enable TX, RX, internal SCICLK, Disable RX ERR, SLEEP, TXWAKE
    ScibRegs.SCICTL1.all =0x0003;
    ScibRegs.SCICTL2.bit.TXINTENA = 0;
    ScibRegs.SCICTL2.bit.RXBKINTENA = 0;
    // 9600 baud @LSPCLK = 22.5MHz (90 MHz SYSCLK),90M/4/(9600*8)=292=0x0124
    // 100M/4/(9600*8)=325=0x0145
    ScibRegs.SCIHBAUD    =0x0001;
    ScibRegs.SCILBAUD    =0x0024;
    ScibRegs.SCICTL1.all =0x0023;  // Relinquish SCI from Reset
    //scia_fifo_init
    ScibRegs.SCIFFTX.all=0xE040;
    ScibRegs.SCIFFRX.all=0x2044;
    ScibRegs.SCIFFCT.all=0x0;    
}

void InitCpuTimer0(void)
{
    EALLOW;  // This is needed to write to EALLOW protected registers
    PieVectTable.TINT1 = &cpu_timer1_isr;
    EDIS;    // This is needed to disable write to EALLOW protected registers

    InitCpuTimers();   // For this example, only initialize the Cpu Timers
    // Configure CPU-Timer 0 to interrupt every 1 milliseconds:
    // 100MHz CPU Freq, 
    ConfigCpuTimer(&CpuTimer0, 90, 1000);//1ms
    // Use write-only instruction to set TSS bit = 0
    CpuTimer0Regs.TCR.all = 0x4000;

    ConfigCpuTimer(&CpuTimer1, 90, 20);//20us
    // Use write-only instruction to set TSS bit = 0
    CpuTimer1Regs.TCR.all = 0x4000;

    IER |= M_INT13;
}




void GpioInit(void)
{
    EALLOW;
    // Enable GPIO outputs on GPIO12, set it high-RELAY_EN
    GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;   // Enable pullup on GPIO8
    GpioDataRegs.GPACLEAR.bit.GPIO12 = 1;   // Load output latch
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;  // GPIO8 = GPIO8
    GpioCtrlRegs.GPADIR.bit.GPIO12 = 1;   // GPIO8 = output
    //for debug pin
    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;   // Enable pullup on GPIO16
    GpioDataRegs.GPASET.bit.GPIO16 = 1;   // Load output latch
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 0;  // GPIO function 
    GpioCtrlRegs.GPADIR.bit.GPIO16 = 1;   // GPIO = output

    // Make GPIO17 an input-PFC READY
   GpioCtrlRegs.GPAPUD.bit.GPIO17 = 1;  // Disable pullup on GPIO17
   GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 0; // GPIO17 = GPIO17
   GpioCtrlRegs.GPADIR.bit.GPIO17 = 0;  // GPIO17 = input

   //DEBUG PIN
   GpioCtrlRegs.GPBPUD.bit.GPIO57 = 0;   // Enable pullup on GPIO8
   GpioDataRegs.GPBSET.bit.GPIO57 = 1;   // Load output latch
   GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0;  // GPIO8 = GPIO8
   GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1;   // GPIO8 = output


   GpioCtrlRegs.GPBPUD.bit.GPIO52 = 0;    // Enable pull-up on GPIO52 (TZ3)
   GpioCtrlRegs.GPBQSEL2.bit.GPIO52 = 3;  // Asynch input GPIO52 (TZ3)
   GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 3;  // Configure GPIO52 as TZ3

   // Enable GPIO outputs on GPIO20, set it coil RELAY_EN
   GpioCtrlRegs.GPAPUD.bit.GPIO20 = 0;   // Enable pullup on GPIO8
   GpioDataRegs.GPACLEAR.bit.GPIO20 = 1;   // Load output latch
   GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 0;  // GPIO8 = GPIO8
   GpioCtrlRegs.GPADIR.bit.GPIO20 = 1;   // GPIO8 = output

   // Enable GPIO outputs on GPIO21, set it coil RELAY_EN
   GpioCtrlRegs.GPAPUD.bit.GPIO21 = 0;   // Enable pullup on GPIO8
   GpioDataRegs.GPACLEAR.bit.GPIO21 = 1;   // Load output latch
   GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 0;  // GPIO8 = GPIO8
   GpioCtrlRegs.GPADIR.bit.GPIO21 = 1;   // GPIO8 = output

   // Enable GPIO outputs on GPIO18, set it coil RELAY_EN
   GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;   // Enable pullup on GPIO8
   GpioDataRegs.GPACLEAR.bit.GPIO18 = 1;   // Load output latch
   GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 0;  // GPIO8 = GPIO8
   GpioCtrlRegs.GPADIR.bit.GPIO18 = 1;   // GPIO8 = output

    EDIS;
}


// cpu_timer1_isr -
__interrupt void cpu_timer1_isr(void)//12us
{
    cpuTimer_20usFlag = 1;
    //cpu_timer1_Handler();
}


// epwm1_timer_isr - Interrupt routines uses in this example
//
__interrupt void epwm1_timer_isr(void)
{
       //
    // Clear INT flag for this timer
    //
    EPwm1Regs.ETCLR.bit.INT = 1;

    ePWM1_CMPB_INT_Handler();
    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

void epwm1_tzint_isr(void){
     EALLOW;
     EPwm1Regs.TZCLR.bit.OST = 1;
     EPwm1Regs.TZCLR.bit.INT = 1;
     EDIS;

     epwm1_tz3_ISR_Handler();
    //
    // Acknowledge this interrupt to receive more interrupts from group 2
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;


}







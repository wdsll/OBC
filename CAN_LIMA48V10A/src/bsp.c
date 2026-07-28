#include "includeAll.h"
static void RCC_Init(void);
static void NVIC_Configuration(void);
static void GPIO_Configuration(void);
static void TIM3_Configuration(void);
static void usart1InterruptEnable(void);
static void usart2InterruptEnable(void) ;
static void ADC_Initial(void);
static void TIM_Initial(TIM_Module* TIMx);
/***
 *初始化函数，
 LEDR PA0
 LEDG PA1
 VBAT PA2  adc3
 Vout PA3  adc4
 Isen PA6  adc7
 TAMB PA7  adc8
 TMOS PB0  adc9
 CANS PA8
 485T PA9
 485R PA10
 CANR PA11
 CANT PA12
 MOSCtrl  PB3 PD0
 UARTT PB4
 UARTR PB5
 PWMI  PB6
 PWMV  PB7
*/


void initAll(void)
{

	  RCC_Init();
    NVIC_Configuration();
    GPIO_Configuration();
    TIM3_Configuration();
    ADC_Initial();
    usart2InterruptEnable();
    TIM_Initial(TIM4);

    CAN_CONFIG();

}



ErrorStatus HSEStartUpStatus;
ErrorStatus HSIStartUpStatus;
#define SYSCLK_SOURCE_HSI 1
#define SYSCLK_SOURCE_HSE 2
#define SYSCLK_SOURCE_HSI_PLL 3
#define SYSCLK_SOURCE_HSE_PLL 4

#ifndef SYSCLK_SOURCE_SELECT
#define SYSCLK_SOURCE_SELECT SYSCLK_SOURCE_HSE_PLL /*select sysclk source */
#endif

#define SYSCLK_USE_HSIDIV2_PLL 0
#define SYSCLK_USE_HSEDIV2_PLL 1
#define HSE_Value   (16000000)
#define SystemCoreClockFrequency 48000000
void SetSysClockTo48M() // 48M=16x6/2
{
    uint32_t pllmul;
    uint32_t latency;
    uint32_t pclk1div, pclk2div;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration
     * -----------------------------*/
    /* RCC system reset(for debug purpose) */
     RCC_Reset();

        /* Enable HSE */
        RCC_HSE_Config(RCC_HSE_ENABLE);

        /* Wait till HSE is ready */
        HSEStartUpStatus = RCC_HSE_Stable_Wait();

        if (HSEStartUpStatus != SUCCESS)
        {
            /* If HSE fails to start-up, the application will have wrong clock
               configuration. User can add here some code to deal with this
               error */

            /* Go to infinite loop */
            while (1);
        }

            latency  = FLASH_LATENCY_1;
               pllmul = RCC_PLL_MUL_6;
            pclk1div = RCC_HCLK_DIV2;
            pclk2div = RCC_HCLK_DIV1;


    FLASH_Latency_Set(latency);

    /* HCLK = SYSCLK */
    RCC_Hclk_Config(RCC_SYSCLK_DIV1);

    /* PCLK2 = HCLK */
    RCC_Pclk2_Config(pclk2div);

    /* PCLK1 = HCLK */
    RCC_Pclk1_Config(pclk1div);
    
        RCC_PLL_Config(RCC_PLL_SRC_HSE_DIV2,pllmul);


    /* Enable PLL */
    RCC_PLL_Enable();

    /* Wait till PLL is ready */
   // while (RCC_Flag_Status_Get(RCC_FLAG_PLLRD) == RESET);
     /* Wait till PLL is ready */
    while ((RCC->CTRL & RCC_CTRL_PLLRDF) == 0)
    {
    }
    /* Select PLL as system clock source */
    RCC_Sysclk_Config(RCC_SYSCLK_SRC_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while (RCC_Sysclk_Source_Get() != RCC_CFG_SCLKSTS_PLL);
}




 void RCC_Init(void) {
	//48M
	//外设时钟全开
	//
//    RCC_Reset();
	  //RCC_Hclk_Config(RCC_SYSCLK_DIV1);
    /* TIM3 clock enable */
    SetSysClockTo48M();

    /* GPIOA clock enable */
		RCC_AHB_Peripheral_Clock_Enable(RCC_AHB_PERIPH_GPIOA);
		RCC_AHB_Peripheral_Clock_Enable(RCC_AHB_PERIPH_GPIOB);
    RCC_APB1_Peripheral_Clock_Enable(RCC_APB1_PERIPH_CAN);
		RCC_APB2_Peripheral_Clock_Enable(RCC_APB2_PERIPH_AFIO);
	
    //RCC_APB1_Peripheral_Clock_Enable(RCC_APB1_PERIPH_TIM2);	
	  RCC_APB1_Peripheral_Clock_Enable(RCC_APB1_PERIPH_TIM3);	
		RCC_APB1_Peripheral_Clock_Enable(RCC_APB1_PERIPH_TIM4);	//pb6 4ch1; pb7 4ch2
	
		RCC_APB1_Peripheral_Clock_Enable(RCC_APB1_PERIPH_USART2);	//pb4 uart2tx pb5 uart2rx ;
		RCC_APB2_Peripheral_Clock_Enable(RCC_APB2_PERIPH_USART1);	//pa9 uart1tx pa10 uart1rx optional for 485;
	
	  RCC_APB1_Peripheral_Clock_Enable(RCC_APB1_PERIPH_PWR);
	  //RCC_APB1_Peripheral_Clock_Enable(RCC_APB1_PERIPH_WWDG);		


		RCC_AHB_Peripheral_Clock_Enable(RCC_AHB_PERIPH_ADC);
		ADC_Clock_Mode_Config(ADC_CKMOD_AHB, RCC_ADCHCLK_DIV16);
    RCC_ADC_1M_Clock_Config(RCC_ADC1MCLK_SRC_HSE, RCC_ADC1MCLK_DIV8);  //selsect HSE as RCC ADC1M CLK Source


    //RCC_AHB_Peripheral_Clock_Enable(RCC_AHB_PERIPH_ADC);
	  //RCC_ADC_Clock_Config(RCC_ADC_DIV12);
    /* enable ADC 1M clock */
    //RCC_ADC_1M_Clock_Config(RCC_ADC1MCLK_DIV48);
}


void ADC_Initial(void)
{   ADC_InitType ADC_InitStructure;
    /* ADC configuration ------------------------------------------------------*/
    ADC_InitStructure.MultiChEn      = DISABLE;
    ADC_InitStructure.ContinueConvEn = DISABLE;
    ADC_InitStructure.ExtTrigSelect  = ADC_EXT_TRIGCONV_REGULAR_SWSTRRCH;
    ADC_InitStructure.DatAlign       = ADC_DAT_ALIGN_R;
    ADC_InitStructure.ChsNumber      = ADC_REGULAR_LEN_1;
    ADC_Initializes(&ADC_InitStructure);

    /* Enable ADC */
    ADC_ON( );
    /* Check ADC Ready */
    while(ADC_Flag_Status_Get(ADC_RD_FLAG ,ADC_FLAG_AWDG ,ADC_FLAG_RDY) == RESET)
        ;
    /* Start ADC1 calibration */
    ADC_Calibration_Operation(ADC_CALIBRATION_ENABLE);
    /* Check the end of ADC1 calibration */
    while (ADC_Calibration_Operation(ADC_CALIBRATION_STS) == SET)
        ;
}


static void NVIC_Configuration(void)
{
    NVIC_InitType NVIC_InitStructure;
/***
 * 定时器3中断
 */
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Initializes(&NVIC_InitStructure);

/***
 * uart1  中断
 */
    /* Enable the UARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel         = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_PER_PRIORITY_0;
    NVIC_InitStructure.NVIC_IRQChannelCmd      = ENABLE;
    NVIC_Initializes(&NVIC_InitStructure);


}

/***
 *初始化函数，
 LEDR PA0 o
 LEDG PA1 o
 VBAT PA2  adc3
 Vout PA3  adc4
 Isen PA6  adc7
 TAMB PA7  adc8
 TMOS PB0  adc9
 CANS PA8 o
 485T PA9
 485R PA10
 CANR PA11
 CANT PA12
 MOSCtrl  PB3 PD0 o
 UARTT PB4
 UARTR PB5
 PWMI  PB6
 PWMV  PB7
*/


static void GPIO_Configuration(void)
{
    GPIO_InitType GPIO_InitStructure;
    /*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
    /**
     *  LEDR PA0 o
        LEDG PA1 o
	      CANS PA8 o
	      MOSCtrl  PB3 PD0 o
     */
    ioPinSet_RedLedOff();
    ioPinSet_GreenLedOff();
    GPIO_Pins_Reset(GPIOA , GPIO_PIN_0 |  GPIO_PIN_1 | GPIO_PIN_8);
    GPIO_Structure_Initialize(&GPIO_InitStructure);

    GPIO_InitStructure.Pin        = GPIO_PIN_0 |  GPIO_PIN_1 | GPIO_PIN_8;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUT_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;

    GPIO_Peripheral_Initialize(GPIOA, &GPIO_InitStructure);
	
	  iOPinSet_SCR_off();
    GPIO_Structure_Initialize(&GPIO_InitStructure);

    GPIO_InitStructure.Pin        = GPIO_PIN_3 ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUT_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;

    GPIO_Peripheral_Initialize(GPIOB, &GPIO_InitStructure);

	
    /*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/



    /*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
    /***
 VBAT PA2  adc3
 Vout PA3  adc4
 IFAN PA4  adc5
 Isen PA6  adc7
 TAMB PA7  adc8
 TMOS PB0  adc9
     */
    GPIO_Structure_Initialize(&GPIO_InitStructure);
    GPIO_InitStructure.Pin       = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_Peripheral_Initialize(GPIOA, &GPIO_InitStructure);

    GPIO_Structure_Initialize(&GPIO_InitStructure);
    GPIO_InitStructure.Pin       = GPIO_PIN_0 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_Peripheral_Initialize(GPIOB, &GPIO_InitStructure);
    /*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

    /*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
    /***
 485T PA9
 485R PA10
 CANR PA11
 CANT PA12
 UARTT PB4
 UARTR PB5
     */
    GPIO_Structure_Initialize(&GPIO_InitStructure);

    /* Configure USARTx Tx as alternate function push-pull */
    GPIO_InitStructure.Pin            = GPIO_PIN_4;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF5_USART2;
    GPIO_Peripheral_Initialize(GPIOB, &GPIO_InitStructure);   

    /* Configure USARTx Rx as alternate function push-pull */
    GPIO_InitStructure.Pin            = GPIO_PIN_5;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF7_USART2;
    GPIO_Peripheral_Initialize(GPIOB, &GPIO_InitStructure); 


    /*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
    /***
     * PWM输出引脚设置， 
		 PWMI  PB6
     PWMV  PB7
     */
    GPIO_Structure_Initialize(&GPIO_InitStructure);
    /* GPIOA Configuration: Channel 1(TIM1) as alternate function push-pull */

        GPIO_InitStructure.Pin        = GPIO_PIN_6;
        GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
        GPIO_InitStructure.GPIO_Current = GPIO_DS_4MA;
        GPIO_InitStructure.GPIO_Alternate = GPIO_AF3_TIM4;
        GPIO_Peripheral_Initialize(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.Pin        = GPIO_PIN_7;
        GPIO_InitStructure.GPIO_Alternate = GPIO_AF3_TIM4;
        GPIO_Peripheral_Initialize(GPIOB, &GPIO_InitStructure);


    /*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

}
static void TIM_Initial(TIM_Module* TIMx)
{
	TIM_TimeBaseInitType TIM_TimeBaseStructure;
	OCInitType TIM_OCInitStructure;
	uint16_t TimerPeriod   = 0;
	uint16_t Channel1Pulse = 0, Channel2Pulse = 0;
//	TIM_Reset(TIMx);
    /* TIMx Configuration
    Generate PWM signals with different duty cycles:
    TIM1CLK = SystemCoreClockFrequency, Prescaler = 0, TIM1 counter clock = SystemCoreClockFrequency

    The objective is to generate 7 PWM signal at 17.57 KHz:
     - TIMx_Period = (SystemCoreClockFrequency / 17570) - 1
    The Timer pulse is calculated as follows:
     - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100  */

    TimerPeriod = (SystemCoreClockFrequency / 48000 ) - 1;
    /* Compute CCDAT1 value to generate a duty cycle at 50% for channel 1 and 1N */
    Channel1Pulse = (uint16_t)(((uint32_t)PWM_CURRENT_RATED * (TimerPeriod - 1)) / 1000);//电流pwm基准
    /* Compute CCDAT2 value to generate a duty cycle at 37.5%  for channel 2 and 2N */
    Channel2Pulse = (uint16_t)(((uint32_t)PWM_VOLTAGE_NORMAL * (TimerPeriod - 1)) / 1000);//电压pwm基准


    /* Time Base configuration */
    TIM_TimeBaseStructure.Prescaler = 0;
    TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;
    TIM_TimeBaseStructure.Period    = TimerPeriod;
    TIM_TimeBaseStructure.ClkDiv    = 0;
    TIM_TimeBaseStructure.RepetCnt  = 0;

    TIM_Base_Initialize(TIMx, &TIM_TimeBaseStructure);

    /* Channel 1,2,3,4 Configuration in PWM mode */
    TIM_OCInitStructure.OcMode       = TIM_OCMODE_PWM2;
    TIM_OCInitStructure.OutputState  = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.OutputNState = TIM_OUTPUT_NSTATE_DISABLE;
    TIM_OCInitStructure.Pulse        = Channel1Pulse;
    TIM_OCInitStructure.OcPolarity   = TIM_OC_POLARITY_LOW;
    TIM_OCInitStructure.OcNPolarity  = TIM_OCN_POLARITY_LOW;
    TIM_OCInitStructure.OcIdleState  = TIM_OC_IDLE_STATE_SET;
    TIM_OCInitStructure.OcNIdleState = TIM_OC_IDLE_STATE_RESET;
    TIM_Output_Channel1_Initialize(TIMx, &TIM_OCInitStructure);

    TIM_OCInitStructure.Pulse = Channel2Pulse;
    TIM_Output_Channel2_Initialize(TIMx, &TIM_OCInitStructure);
//    TIM_OCInitStructure.Pulse = Channel3Pulse;
//    TIM_Output_Channel3_Initialize(TIMx, &TIM_OCInitStructure);
//    TIM_OCInitStructure.Pulse = Channel4Pulse;
//    TIM_Output_Channel4_Initialize(TIMx, &TIM_OCInitStructure);



    /* TIM1 counter enable */
    TIM_On(TIMx);

    /* TIM1 Main Output Enable */
    TIM_PWM_Output_Enable(TIMx);
}
static void TIM3_Configuration(void)
{
	TIM_TimeBaseInitType TIM_TimeBaseStructure;
//	TIM_Reset(TIM3);
    TIM_Base_Struct_Initialize(&TIM_TimeBaseStructure);
    /* Time base configuration */
    TIM_TimeBaseStructure.Period    = 4800;   //SLOW_FRE;//4800; // freq = 48MHz/480 = 100kHz
    TIM_TimeBaseStructure.Prescaler = 0;
    TIM_TimeBaseStructure.ClkDiv    = 0;
    TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;

    TIM_Base_Initialize(TIM3, &TIM_TimeBaseStructure);

    /* Prescaler configuration */
    TIM_Base_Reload_Mode_Set(TIM3, TIM_PSC_RELOAD_MODE_IMMEDIATE);
    TIM_Auto_Reload_Preload_Enable(TIM3);  //堵转2,新增
    /* TIM3 enable update irq */
    TIM_Interrupt_Enable(TIM3, TIM_INT_UPDATE);

    /* TIM3 enable counter */
    TIM_On(TIM3);
}
void setTim3Period(uint32_t newPeriod) {
	 TIM_Base_Auto_Reload_Set(TIM3, newPeriod);
}
static uint8_t periopd01 = 0;
void togglePeriod(void) {
	periopd01 = !periopd01;
	if(periopd01) {
		setTim3Period(HIGH_FRE);
	} else{
		setTim3Period(SLOW_FRE);
	}
}


static void usart1InterruptEnable(void) {
	USART_InitType USART_InitStructure;
//	USART_Reset(USART1);
    USART_InitStructure.BaudRate   = 9600;
    USART_InitStructure.WordLength = USART_WL_8B;
    USART_InitStructure.StopBits   = USART_STPB_1;
    USART_InitStructure.Parity     = USART_PE_NO;
    USART_InitStructure.Mode       = USART_MODE_RX | USART_MODE_TX;

    USART_Initializes(USART1, &USART_InitStructure);

    USART_Interrput_Enable(USART1, USART_INT_RXDNE);
  //  USART_Interrput_Enable(USARTy, USART_INT_TXDE);

    USART_Enable(USART1);
}
static void usart2InterruptEnable(void) {
	USART_InitType USART_InitStructure;
//	USART_Reset(USART2);
    USART_InitStructure.BaudRate   = 9600;
    USART_InitStructure.WordLength = USART_WL_8B;
    USART_InitStructure.StopBits   = USART_STPB_1;
    USART_InitStructure.Parity     = USART_PE_NO;
    USART_InitStructure.Mode       = USART_MODE_RX | USART_MODE_TX;

    USART_Initializes(USART2, &USART_InitStructure);

    USART_Interrput_Enable(USART2, USART_INT_RXDNE);
  //  USART_Interrput_Enable(USARTy, USART_INT_TXDE);

    USART_Enable(USART2);
}

/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

void swd_delay_init(void) {
	GPIO_InitType GPIO_InitStructure;
	  NVIC_InitType NVIC_InitStructure;
    /*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
    /**
     * PA9  设置为输出
     */
    GPIO_Structure_Initialize(&GPIO_InitStructure);

    GPIO_InitStructure.Pin        =  GPIO_PIN_9 ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUT_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;

    GPIO_Peripheral_Initialize(GPIOA, &GPIO_InitStructure);




    /*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/
    /***
     * usart1 tx rx引脚初始化
     */
    GPIO_Structure_Initialize(&GPIO_InitStructure);

    /* Configure USARTx Tx as alternate function push-pull */
    GPIO_InitStructure.Pin            = GPIO_PIN_9;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF5_USART1;
    GPIO_Peripheral_Initialize(GPIOA, &GPIO_InitStructure);   

    /* Configure USARTx Rx as alternate function push-pull */
    GPIO_InitStructure.Pin            = GPIO_PIN_10;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF5_USART1;
    GPIO_Peripheral_Initialize(GPIOA, &GPIO_InitStructure); 



      /* Enable the UARTy Interrupt */
      NVIC_InitStructure.NVIC_IRQChannel         = USART1_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
      NVIC_InitStructure.NVIC_IRQChannelCmd      = ENABLE;
      NVIC_Initializes(&NVIC_InitStructure);

    usart1InterruptEnable();


}



/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
/***
 * PWM API
 */
void PWM_Init(void) {
	
}


/***
 *channel  :     	PWM_CHANNEL_CURRENT
 *						
 *
 *	pwmval       小于10000
 */
static uint32_t pwm[2] = {0 , 0};
void PWM_set2(uint32_t channel , uint32_t pwmval) {
	//channel == PWM_CHANNEL_CURRENT(0)   电流通道PWM  PIN13
	//channel == PWM_CHANNEL_VOLTAGE（1）电压通道PWM   PIN15
	OCInitType TIM_OCInitStructure;
	uint16_t ChannelPulse = 0;
	uint16_t TimerPeriod   = 0;
	pwm[channel] = pwmval;
	 TimerPeriod = (SystemCoreClockFrequency / 48000 ) - 1;
	 ChannelPulse = (uint16_t)(((uint32_t)pwmval * (TimerPeriod - 1)) / 1000);//电流pwm基准


	    TIM_OCInitStructure.OcMode       = TIM_OCMODE_PWM2;
	    TIM_OCInitStructure.OutputState  = TIM_OUTPUT_STATE_ENABLE;
	    TIM_OCInitStructure.OutputNState = TIM_OUTPUT_NSTATE_DISABLE;
	    TIM_OCInitStructure.Pulse        = ChannelPulse;
	    TIM_OCInitStructure.OcPolarity   = TIM_OC_POLARITY_LOW;
	    TIM_OCInitStructure.OcNPolarity  = TIM_OCN_POLARITY_LOW;
	    TIM_OCInitStructure.OcIdleState  = TIM_OC_IDLE_STATE_SET;
	    TIM_OCInitStructure.OcNIdleState = TIM_OC_IDLE_STATE_RESET;
	if(channel == PWM_CHANNEL_VOLTAGE) {
		TIM_Output_Channel2_Initialize(TIM4, &TIM_OCInitStructure);
	} else if(channel == PWM_CHANNEL_CURRENT) {
		TIM_Output_Channel1_Initialize(TIM4, &TIM_OCInitStructure);
	} else {

	}
}


void PWM_set(uint32_t channel , uint32_t pwmval) {

	pwm[channel] = pwmval;
		if(channel == PWM_CHANNEL_VOLTAGE) {
		TIM4->CCDAT1 = pwmval;
	} else if(channel == PWM_CHANNEL_CURRENT) {
		TIM4->CCDAT2 = pwmval;
	} 

}



/***
 *channel  :     	PWM_CHANNEL_CURRENT
 *						PWM_CHANNEL_VOLTAGE
 *
 *	pwmval       小于10000
 */
uint32_t PWM_get(uint32_t channel ) {
	return pwm[channel];
}
/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
/**
*\*\name    ADC_Regular_Sequence_Single_Config.
*\*\fun     Configures ADC channel Single in regular sequence.
*\*\param   channel :
*\*\          - ADC_CH_0
*\*\          - ADC_CH_1
*\*\          - ADC_CH_2
*\*\          - ADC_CH_3
*\*\          - ADC_CH_4
*\*\          - ADC_CH_5
*\*\          - ADC_CH_6
*\*\          - ADC_CH_7
*\*\          - ADC_CH_8
*\*\          - ADC_CH_9
*\*\          - ADC_CH_10
*\*\return  none
**/
void ADC_Regular_Sequence_Single_Config(uint8_t ADC_Channel)
{
    ADC_Channel_Sample_Time_Config(ADC_Channel ,ADC_SAMP_TIME_13CYCLES5);
    ADC_Regular_Sequence_Conversion_Number_Config(ADC_Channel, ADC_REGULAR_NUMBER_1);
}



void adc_channel_select(uint32_t channel) {//ADC通道选择

    /***
 VBAT PA2  adc3
 Vout PA3  adc4
 IFAN PA4  adc5
 Isen PA6  adc7
 TAMB PA7  adc8
 TMOS PB0  adc9
     */
	
	switch (channel) {
	case  ADC_CHANNEL_CHARGING_CURRENT  :
		ADC_Regular_Sequence_Single_Config(ADC_CH_7);
		break;
	case  ADC_CHANNEL_CHARGING_VOLTAGE  :
		ADC_Regular_Sequence_Single_Config(ADC_CH_4);
		break;
	case  ADC_CHANNEL_TEMP_MOS          :
		ADC_Regular_Sequence_Single_Config(ADC_CH_9);
		break;
	case  ADC_CHANNEL_TEMP_ENV          :
		ADC_Regular_Sequence_Single_Config(ADC_CH_8);
		break;
	case  ADC_CHANNEL_FAN_CURRENT       :
		ADC_Regular_Sequence_Single_Config(ADC_CH_5);
		break;
	case  ADC_CHANNEL_BAT_VOLTAGE       :
		ADC_Regular_Sequence_Single_Config(ADC_CH_3);
		break;	
	default:break;
	}
}
uint32_t adc_overFlag(void) {//获得ADC转换完成标记状态，0表示转换未完成
	return ADC_Flag_Status_Get(ADC_RUN_FLAG ,ADC_FLAG_ENDC ,ADC_FLAG_RDY);
}
void adc_Start(void) {//开始ADC转换
	 ADC_Regular_Channels_Software_Conversion_Operation(ADC_EXTRTRIG_SWSTRRCH_ENABLE);
}
uint32_t adc_get(void) {//返回ADC结果寄存器的值
	return   ADC_Regular_Group_Conversion_Data_Get();
}
void adc_overFlagClear(void) {//清除转换完成标记。看是否需要？应该是已启动ADC自动清除的。
	ADC_Flag_Status_Clear(ADC_FLAG_ENDC);
	ADC_Flag_Status_Clear(ADC_FLAG_STR);
}
/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/
uint16_t ADC_GetData(uint8_t ADC_Channel)
{
    uint16_t dat;
    ADC_Channel_Sample_Time_Config(ADC_Channel ,ADC_SAMP_TIME_55CYCLES5);
    ADC_Regular_Sequence_Conversion_Number_Config(ADC_Channel, ADC_REGULAR_NUMBER_1);
    /* Start ADC Software Conversion */
    ADC_Regular_Channels_Software_Conversion_Operation(ADC_EXTRTRIG_SWSTRRCH_ENABLE);
    while(ADC_Flag_Status_Get(ADC_RUN_FLAG ,ADC_FLAG_ENDC ,ADC_FLAG_RDY)==0){
    }
    ADC_Flag_Status_Clear(ADC_FLAG_ENDC);
    ADC_Flag_Status_Clear(ADC_FLAG_STR);
    dat=ADC_Regular_Group_Conversion_Data_Get();
    return dat;
}




/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
void uart_send(char dat) {//填充发送寄存器
	 USART_Data_Send(USART2, dat);
}
uint32_t uart_get_TxOverFlag(void) {//获得发送完成标志状态,0表示发送未完成
	return USART_Flag_Status_Get(USART2, USART_FLAG_TXDE) != RESET;
}
void uart_clear_txOverFlag(void) {//清除发送完成标志
	
}
void uart2_send(char dat) {
	USART_Data_Send(USART1 , dat);
}
uint32_t uart2_txOverFlag_get(void) {
	return USART_Flag_Status_Get(USART1, USART_FLAG_TXDE) != RESET;
}
/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
void flashWriteIn(uint32_t dat[] , uint32_t length , uint32_t key) {
	uint32_t i;
	if(key == MYKEY) {
		FLASH_Unlock();
	}
	if(key == MYKEY) {
		FLASH_One_Page_Erase(LAST_ADDR_BLOCK);
	}
	for(i = 0 ;  i < length ; i++) {
		if(key == MYKEY) {
			FLASH_Word_Program(LAST_ADDR_BLOCK + i * 4 , dat[i]);
		}
	}
	FLASH_Lock();
}


uint32_t flash_read(uint32_t addr) {
	return (*(volatile uint32_t*)(addr));
}
/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/

/*AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA*/
/***
 * PA5控制拉掉输出电压参考电平
 */
void iOPinSet_VRef_Input2(void) {//拉输出参考电压的引脚，设为输入
	GPIO_InitType GPIO_InitStructure;
	GPIO_Structure_Initialize(&GPIO_InitStructure);
	GPIO_InitStructure.Pin       = GPIO_PIN_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;//引脚设为输入
#ifdef PA5_DEBUG
	GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
#else
	GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
#endif

	GPIO_Peripheral_Initialize(GPIOA, &GPIO_InitStructure);
}
void iOPinSet_VRef_OutAndLow2(void) {//拉输出参考电压的引脚，设为输出
	GPIO_InitType GPIO_InitStructure;
	GPIO_Pins_Reset(GPIOA , GPIO_PIN_5);	//引脚设为低
//引脚设为输出
	GPIO_Structure_Initialize(&GPIO_InitStructure);
	GPIO_InitStructure.Pin       = GPIO_PIN_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT_PP;//引脚设为输入
	GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
	GPIO_Peripheral_Initialize(GPIOA, &GPIO_InitStructure);
}
/***
 * PA15控制风扇
 */
void iOPinSet_FanOff(void) {//关闭风扇  PA4  低电平
	GPIO_Pins_Reset(GPIOA , GPIO_PIN_15) ;
}
void iOPinSet_FanOn(void) {//打开风扇  PA4  高电平
	GPIO_Pins_Set(GPIOA , GPIO_PIN_15);
}
uint8_t ioPinGet_fanState(void) {
	return GPIO_Output_Pin_Data_Get(GPIOA , GPIO_PIN_15);
}
/***
 * Pb3高电平时，可控硅触发电平拉低,可控硅可以关闭
 * Pb3低电平时，可控硅触发引脚有电平，可控硅打开
 */
void iOPinSet_SCR_off(void) {//PA6高电平时，可控硅触发电平拉低,可控硅可以关闭
	//GPIO_Pins_Reset(GPIOB , GPIO_PIN_3);
	GPIO_Pins_Set(GPIOB , GPIO_PIN_3);
}
void iOPinSet_SCR_on(void) {// PA6低电平时，可控硅触发引脚有电平，可控硅打开
	GPIO_Pins_Set(GPIOB , GPIO_PIN_3);
}


// PA0   
void ioPinSet_RedLedOn(void) {
	GPIO_Pins_Set(GPIOA , GPIO_PIN_0);
}

void ioPinSet_RedLedOff(void) {
	GPIO_Pins_Reset(GPIOA , GPIO_PIN_0) ;
}


// PA1
void ioPinSet_GreenLedOn(void) {
	GPIO_Pins_Set(GPIOA , GPIO_PIN_1);
}

void ioPinSet_GreenLedOff(void) {
	GPIO_Pins_Reset(GPIOA , GPIO_PIN_1) ;
}

/*
void ioPinSet_RedLedOn(void) {
	GPIO_Pins_Set(GPIOA , GPIO_PIN_1);
}

void ioPinSet_RedLedOff(void) {
	GPIO_Pins_Reset(GPIOA , GPIO_PIN_1) ;
}


// PA1
void ioPinSet_GreenLedOn(void) {
	GPIO_Pins_Set(GPIOA , GPIO_PIN_0);
}

void ioPinSet_GreenLedOff(void) {
	GPIO_Pins_Reset(GPIOA , GPIO_PIN_0) ;
}

*/

/***
 * PB1
 * 电池包移除，返回1
 * 否则返回0

uint8_t isBatPackTakeAway(void) {
	return GPIO_Input_Pin_Data_Get(GPIOB , GPIO_PIN_1) != PIN_RESET;
}
 */


/*VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV*/



void setPVD(void) {
	PWR_PVD_Level_Config(PWR_PVD_LEVEL_2V88);
	PWR_PVD_Enable();
	PWR_Flag_Status_Get(PWR_PVD_OUTPUT_FLAG);
}

void IWDG_Config(IWDG_CONFIG_PRESCALER IWDG_prescaler, uint16_t reload_value)
{
	/* The timeout may varies due to LSI frequency dispersion */
	/* Disable write protection to IWDG_PREDIV and IWDG_RELV registers */
	IWDG_Write_Protection_Disable();
	
	/* IWDG counter clock */
	IWDG_Prescaler_Division_Set(IWDG_prescaler);
	
	/* Sets IWDG reload value */
	/* Set counter reload value to obtain x second IWDG TimeOut.
     Counter Reload Value Time = x(second)/IWDG counter clock period
                               = x(second) / (LSI/IWDG_prescaler)
    */
	 (reload_value);
	
	/* Reload counter with IWDG_PREDIV value in IWDG_RELV register to prevent reset */
	IWDG_Key_Reload();
	
	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	IWDG_Enable();
}

void IWDG_Feed(void)
{
	/* Put the value of the reload register into the counter */
	IWDG_Key_Reload();
}


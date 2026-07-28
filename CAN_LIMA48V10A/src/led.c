#include "includeAll.h"

 uint8_t red_led , green_led;

static uint8_t tmCnt = 0;
static uint8_t flip1Hz = 0 , flip2Hz = 0;;
void led_ctr_exec(void) {
	tmCnt++;
	if(tmCnt >= 200) {
	  tmCnt = 0;
	}
	
	if(tmCnt % 50 == 0) {
		flip1Hz = !flip1Hz;
	}
	
	if(tmCnt % 25 == 0) {
		flip2Hz = !flip2Hz;
	}
	
	if(red_led == LED_ON || (red_led == LED_TWINKLE_1HZ && flip1Hz) || (red_led == LED_TWINKLE_2HZ && flip2Hz)) {
		ioPinSet_RedLedOn();
	} else {
		ioPinSet_RedLedOff();
	}

	if(green_led == LED_ON || (green_led == LED_TWINKLE_1HZ && flip1Hz) || (green_led == LED_TWINKLE_2HZ && flip2Hz)) {
		ioPinSet_GreenLedOn();
	} else {
		ioPinSet_GreenLedOff();
	}
}



void LED_Set_State(uint8_t whichLed , LEDState_T newState) {
	if(whichLed == RED_LED) {
		red_led = newState;
	} else if(whichLed == GREEN_LED) {
		green_led = newState;
	} else {
		
	}
}


//0b1111110,  126
//0b0110000,   48
//0b1101101,  109
//0b1111001,  121
//0b0110011,  51
//0b1011011,  91
//0b1011111,  95
//0b1110000,  112
//0b1111111,  127
//0b1111011,  123
//0b0000000};
const unsigned char SEG010[]={126,48,109,121,51,91,95,112,127,123,0};
volatile uint32_t led18=0;
void num2led(unsigned char n,unsigned char k1,unsigned char k2)
{
unsigned char a,b;
led18=0;
if(n<=188)
{
if(n>=100) led18 |= 0x30000;
if(n>9)
{
n=n%100;
a=n/10;
}
else a=10; //void
b=n%10;
led18 |= SEG010[a]<<9;
led18 |= SEG010[b]<<2;
}
if(k1>0) led18 |=2;
if(k2>0) led18 |=1;
}



void GPIO_LED_Set(GPIO_Module* GPIOx, uint16_t pin, unsigned char hlt)  // hlt : high1 low2,tristatus 0
{	
	GPIO_InitType GPIO_InitStructure;
	if(hlt==1) GPIO_Pins_Set(GPIOx, pin);	//引脚设为高
	else GPIO_Pins_Reset(GPIOx, pin);	//引脚设为低
	GPIO_Structure_Initialize(&GPIO_InitStructure);
	GPIO_InitStructure.Pin = pin;
	if(hlt==0) GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;//引脚设为输入
	else       GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT_PP;//引脚设为输出
	GPIO_Peripheral_Initialize(GPIOx, &GPIO_InitStructure);	
	
}

void GPIO_LED_Set2(GPIO_Module* GPIOx, uint16_t pin, unsigned char hlt)  // hlt : high1 low2,tristatus 0
{	
	if(hlt==1) GPIO_Pins_Set(GPIOx, pin);	//引脚设为高
	else GPIO_Pins_Reset(GPIOx, pin);	//引脚设为低
	if(hlt==0) 	GPIO_Mode_Set(GPIOx, GPIO_MODE_INPUT, pin);//引脚设为输入
	else    	GPIO_Mode_Set(GPIOx, GPIO_MODE_OUT_PP, pin); //引脚设为输出
}


void ledshow()
{
static unsigned char num=0;
unsigned char d1=0,d2=0,d3=0,d4=0,d5=0;  //0 float,1h, 2l
uint32_t temp;
temp=led18 & (1<<num);
if(temp>0){
switch(num)
{
	case 0: d2=1; d5=2; break;
	case 1: d3=1; d5=2; break;
	case 2: d1=2; d5=1; break;
	case 3: d1=2; d4=1; break;
	case 4: d1=1; d4=2; break;
	case 5: d1=2; d3=1; break;
	case 6: d1=1; d3=2; break;
	case 7: d1=2; d2=1; break;
	case 8: d1=1; d2=2; break;
	case 9: d4=2; d5=1; break;
	case 10: d3=2; d5=1; break;
	case 11: d2=2; d5=1; break;
	case 12: d2=2; d4=1; break;
	case 13: d3=2; d4=1; break;
	case 14: d2=2; d3=1; break;
	case 15: d2=1; d3=2; break;
	case 16: d2=1; d4=2; break;
	case 17: d3=1; d4=2; break;
}
}	

//pa4, pa5, pb1, pa10, pa9
GPIO_LED_Set(GPIOA,GPIO_PIN_4,d1);
GPIO_LED_Set(GPIOA,GPIO_PIN_5,d2);
GPIO_LED_Set(GPIOB,GPIO_PIN_1,d3);
GPIO_LED_Set(GPIOA,GPIO_PIN_10,d4);
GPIO_LED_Set(GPIOA,GPIO_PIN_9,d5);


num++;	
if(num>=18)
	num=0;
}

//void ledshow();
//void num2led(unsigned char n,unsigned char k1,unsigned char k2);
extern volatile uint32_t systick_100us; 
void lcdtest()
{
	static uint32_t t=0,t2=0;
	static uint8_t i=188;
	uint32_t d;
	d= systick_100us - t;
	if(d>7){
		t=systick_100us;
		ledshow();
	}
	
	d= systick_100us - t2;
	if(d>10000){
		t2=systick_100us;
		num2led(i++,1,1);
		if(i>188) i=0;
	}	
}

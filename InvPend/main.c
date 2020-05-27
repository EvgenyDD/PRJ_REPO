/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_exti.h"
#include "misc.h"
#include "core_cm3.h"

#include "hw_config.h"
#include "pcd8544.h"

#include <stdbool.h>

#include "LIS3DSH.h"

#include "controller.h"
#include "NRF24L01.h"
#include "string.h"
#include "debug.h"
extern float EError;

/*
#include "string.h"
#include "debug.h"
*/
uint16_t ii=0;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint32_t delay = 0;

uint8_t MEncoder = 0;
int16_t monitorRelAngle=0;
int32_t monitorPWM=0;
int32_t speedOfMove=0;
int16_t handEnc = 0;


uint32_t realTrack=15000;

/* Extern variables ----------------------------------------------------------*/
extern float Error;
extern float curSpeed;
extern int16_t Center;
extern float PGain, DGain, IGain;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : __delay_ms.
* Description    : Delay the code on the N cycles of SysTick Timer.
* Input          : N Delay Cycles.
*******************************************************************************/
void delay_ms(uint32_t nTime)
{
	delay = nTime;
	while(delay);
}


/*******************************************************************************
* Function Name  : SysTick_Handler.
* Description    : Handles SysTick Timer Interrupts every 1ms.
*******************************************************************************/
void SysTick_Handler(void)
{
	if(delay) delay--;

	static uint16_t LEDCounter = 0;
	if(LEDCounter == 0)
	{
		LEDCounter = 200;
		LED_Callback();
	}
	else
		LEDCounter--;
}


/*******************************************************************************
* Function Name  : main
* Description    : Main routine
*******************************************************************************/
int main(void)
{
	PeriphInit();
	DebugInit();

	LIS3DSH_Init();
	InitPWMController();
	NRF24L01_Init();
	delay_ms(50);


	LCD_Init();
	LCD_Line(1,1,20,20,1);
	LCD_Char(25,25,'A',0);

	NRF24L01_Config(15, 32);
	NRF24L01_SetRF(NRF24L01_DataRate_2M, NRF24L01_OutputPower_0dBm);

	uint8_t dataOut[32]={1}, dataIn[32]={5};

	for(uint8_t i=0; i<32; i++)
			dataOut[i]=i;// = dataIn[i];

	LCD_Refresh();

	LED(LED_RED, LED_FLASH);

    while(1)
    {
    	/*for(uint8_t i=0; i<32; i++)
    			{
    					dataOut[i] = dataIn[i];
    					//dataOut[i]++;// = dataIn[i];
    			}*/
    	dataOut[0] = monitorRelAngle & 0xFF;
    	dataOut[1] = (monitorRelAngle>>8) & 0xFF;

    	dataOut[2] = monitorPWM & 0xFF;
    	dataOut[3] = (monitorPWM>>8) & 0xFF;

    	dataOut[4] = speedOfMove & 0xFF;
    	dataOut[5] = (speedOfMove>>8) & 0xFF;

    	NRF24L01_Send(dataOut, 32);

    	while(NRF24L01_GetTransmissionStatus() == NRF24L01_Transmit_Status_Sending);
    	NRF24L01_PowerUpRx();
    	delay_ms(2);

    	bool flag = false;
    	if(NRF24L01_IsDataReady())
		{
			NRF24L01_GetData(dataIn);
			flag = true;
		}

    	if(flag)
    	{
    		int16_t p,d,i;
    		p=(dataIn[0]|(dataIn[1]<<8));
    		d=(dataIn[2]|(dataIn[3]<<8));
    		i=(dataIn[4]|(dataIn[5]<<8));
    		PGain = (float)p/1000;
    		DGain = (float)d/1000;
    		IGain = (float)i/1000;
    	}

    	LCD_Clear();
    	char a[20]="P:",f[10]="";
    	ftoa_(PGain, f, 3);
    	strcat_(a, f);
    	LCD_String(2,35,a, 0);
    	a[0] = 'D'; a[1] = ':'; a[2] = '\0';
    	ftoa_(DGain, f, 3);
    	strcat_(a, f);
    	LCD_String(2,45,a, 0);
    	a[0] = 'I'; a[1] = ':'; a[2] = '\0';
    	ftoa_(IGain, f, 3);
    	strcat_(a, f);
    	LCD_String(2,55,a, 0);
		/*LCD_Int(10,45, dataIn[2]|(dataIn[3]<<8), 0);
		LCD_Int(10,55, dataIn[4]|(dataIn[5]<<8), 0);*/

    	LCD_Int(10,0,realTrack, 0);


    	//LCD_Int(40,35,TIM1->CNT, 0);
    	LCD_IntWString(2,15,/*monitorRelAngle*/ii, "A:", 0);
    	LCD_IntWString(2,25,monitorPWM, "M:", 0);


    	LCD_IntWString(60,0,curSpeed, "Sp:", 0);
    	LCD_IntWString(60,10,Error, "Er:", 0);


    	for(uint8_t i=0; i<32; i++)
    	    dataOut[i] = dataIn[i];// = [i];

    	/*int16_t dat[3];
    	LIS3DSH_ReadAxes(&dat[0]);
    	LCD_IntWString(10,35, dat[0], "X:",0);
    	LCD_IntWString(10,45, dat[1], "Y:",0);
    	LCD_IntWString(10,55, dat[2], "Z:",0);*/

    	uint16_t RightTouchFlag, LeftTouchFlag;

    	GPIOD->CRL = 0x01;
    	GPIOC->CRH |= 0x10000000;
    	GPIOC->CRH &= ~0xE0000000;
    	asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");

    	__disable_irq();
    	GPIOD->BRR = GPIO_Pin_0;
    	GPIOC->BRR = GPIO_Pin_15;

    	GPIOD->CRL = 0x04;
    	GPIOC->CRH |= 0x40000000;
		GPIOC->CRH &= ~0xB0000000;

    	if(GPIOD->IDR & GPIO_Pin_0)
			RightTouchFlag = 0;
		else
			RightTouchFlag = 1;

    	for(uint8_t i=0,k; i<31; i++) k++;

    	if(GPIOC->IDR & GPIO_Pin_15)
			LeftTouchFlag = 0;
		else
			LeftTouchFlag = 1;

    	__enable_irq();

    	LED(LED_BLUE, RightTouchFlag);
    	LED(LED_GREEN, LeftTouchFlag);

    	static bool edit=false;
    	if(LeftTouchFlag)
    	{
    		if(edit == false)
    		{
    			edit = true;
    			Center--;
    		}
    	}
    	else if(RightTouchFlag)
    	{
    		if(edit == false)
			{
				edit = true;
				Center++;
				//Center = 1980;
				realTrack = 15000;
				EError = 0;
			}
		}
    	else
    		edit = false;

    	//if(abs(monitorRelAngle)>LIMIT)
    	//handEnc = (int16_t)TIM1->CNT / 2;
    	LCD_IntWString(60,30,monitorRelAngle, "E:", 0);

    	LCD_Refresh();
    	delay_ms(10);

    }
}


void EXTI2_IRQHandler()
{
	MEncoder++;


	EXTI_ClearITPendingBit(EXTI_Line2);
}

void EXTI9_5_IRQHandler()
{

	EXTI_ClearITPendingBit(EXTI_Line8);
}


void ADC1_IRQHandler(void)
{


	//70us
	//GPIO_SetBits(GPIOB, GPIO_Pin_1);
	uint16_t AbsoluteAngle = ADC_GetConversionValue(ADC1);
    monitorRelAngle = AbsoluteAngle - Center;

    int16_t motorVal = PWMController(Center, AbsoluteAngle);
   // monitorPWM = motorVal;

    ii++;

    if(ii<3000)
    	motorVal = 80;
    else if(ii>=3000 && ii<6000)
    	motorVal = 700;
    else if(ii>=6000 && ii<9000)
        motorVal = 850;
    else if(ii>=9000 && ii<12000)
        motorVal = 1000;
    else if(ii>12000)
        ii=0;

    Motor(motorVal);

    monitorPWM = motorVal;//EError;



   /* static uint8_t debugCnt=0;
    if(debugCnt++ >= 30)
    {
    	DebugSendPID(monitorRelAngle, monitorPWM);
    	debugCnt = 0;
    }*/


#if 1
    static uint16_t timerCntr = 0;
    if(++timerCntr >= 100)
    {
		#if 0
    	Center = VirtZeroController(realTrack, 15000);//1980 - ((int16_t)realTrack-15000)/100;
		#else
    	VirtZeroController(realTrack, 15000);
		#endif
    	/*if(Center>1980+10)Center = 1980+10;
    	if(Center<1980-10)Center = 1980-10;*/
    	//monitorPWM = (Center-1980)*20;

    	timerCntr = 0;
    }
#endif

    realTrack += (motorVal>0?1:(-1))*MEncoder;
    MEncoder = 0;

    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    //GPIO_ResetBits(GPIOB, GPIO_Pin_1);
}

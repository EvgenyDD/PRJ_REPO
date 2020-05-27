/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_exti.h"
#include "misc.h"

#include "hw_config.h"
#include "keyboard.h"
#include "debug.h"
#include "string.h"
#include <stdbool.h>
#include "flash.h"


/* Private typedef -----------------------------------------------------------*/
enum {IDLE,LOCK,ALARM,SAVECODE};
enum {RED,GRN,REDGRN}LEDConfig;
const char WorkModes[] = "IDLELOCKALRMSvCd";


/* Private define ------------------------------------------------------------*/
#define SENSORS_NUM	4

#define CODE_MIN_SYMB				3
#define CODE_MAX_SYMB				10


/* Private macro -------------------------------------------------------------*/
#define isCoverOpened() 			(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) != 0)
#define isBatteryConnected()		(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)
#define isPowerActive()				(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
#define isResetSwitchActivated()	(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0)

/* Private variables ---------------------------------------------------------*/
volatile uint16_t delayCounter = 0;
volatile uint16_t beepCounter = 0;
volatile uint16_t unlockCounter = 0;
volatile uint32_t SysTickCounter = 0;

uint8_t batteryFlag = 0;
uint8_t workMode = LOCK;
uint16_t tempCodeMass[CODE_MAX_SYMB];
uint16_t CodeMass[CODE_MAX_SYMB+1] = {5,32,32,32,32,32};

static uint8_t AlarmSignalIsPresent = DISABLE;
static uint8_t failInputedCode = false;
static uint8_t ptrInputCode = 0;

uint16_t sensConf[SENSORS_NUM];
const uint32_t SensorPin[]={
	GPIO_Pin_12,GPIO_Pin_11,
	GPIO_Pin_10,GPIO_Pin_9,
	GPIO_Pin_8, GPIO_Pin_15,
	GPIO_Pin_14,GPIO_Pin_13
};


/* Extern variables ----------------------------------------------------------*/
extern volatile uint16_t butCdTmr;


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : FlashReadData
* Description    : Read sensors config & code combination from flash
*******************************************************************************/
void FlashReadData()
{
	uint32_t st_address = FLASH_BASE + FLASH_PAGE * 1024;

	uint16_t j = 0;
	for(uint8_t i = 0; i < SENSORS_NUM / 2; i++, j += 4)
	{
		sensConf[i * 2] = FLASH_Read(st_address + j) >> 16;
		sensConf[i * 2 + 1] = FLASH_Read(st_address + j) & 0xFFFF;
	}

	j += 4;

	for(uint8_t i = 0; i < CODE_MAX_SYMB / 2; i++, j += 4)
	{
		CodeMass[i * 2] = FLASH_Read(st_address + j) >> 16;
		CodeMass[i * 2 + 1] = FLASH_Read(st_address + j) & 0xFFFF;
	}
}


/*******************************************************************************
* Function Name  : FlashWriteData
* Description    : Write sensors config & code combination to flash
*******************************************************************************/
void FlashWriteData()
{
	uint16_t mass[SENSORS_NUM + 2 + CODE_MAX_SYMB + 1];

	for(uint16_t i = 0, j = 0; i < SENSORS_NUM + 2 + CODE_MAX_SYMB + 1; i++)
	{
		if(i <= SENSORS_NUM)
			mass[i] = sensConf[i];
		else if(i > SENSORS_NUM && i < SENSORS_NUM + 2)
			mass[i] = 0xFFFF;
		else
			mass[i] = CodeMass[j++];
	}

	uint32_t st_address = FLASH_BASE + FLASH_PAGE * 1024;
	FLASH_WritePage(st_address, mass, SENSORS_NUM+2+CODE_MAX_SYMB+1);
}


/*******************************************************************************
* Function Name  : delay_ms
* Description    : Delay routine
*******************************************************************************/
void delay_ms(uint16_t time)
{
	delayCounter = time;
	while(delayCounter != 0);
}


/*******************************************************************************
* Function Name  : Sound
* Description    : Make sound
* Input		 	 : Frequency
*******************************************************************************/
void Sound(uint16_t freq)
{
	GPIO_InitTypeDef GPIOInitStructure;
	if(freq == 0)
	{
		GPIOInitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIOInitStructure);
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
		return;
	}

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIOInitStructure);


	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler = 2;
	TIM_TimeBaseStructure.TIM_Period = freq;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
}


void Sound2(uint16_t freq)
{
	TIM2->ARR = freq;
}



/*******************************************************************************
* Function Name  : SysTick_Handler
*******************************************************************************/
void SysTick_Handler()
{
	if(beepCounter)
	{
		if(--beepCounter == 0)
			Sound(0);
	}

	if(unlockCounter)
	{
		if(--unlockCounter == 0)
			failInputedCode = true;//Unlock(0xFFFF);
	}

	SysTickCounter++;

	if(butCdTmr)
		butCdTmr--;

	if(delayCounter)
		delayCounter--;

	static uint16_t LEDCounter = 0;
	if(LEDCounter == 0)
	{
		LEDCounter = 400;
	}
	else
		LEDCounter--;

	if(LEDCounter == 0 || LEDCounter == 20*4 || LEDCounter == 80*4)
		LED_Callback(LEDCounter/4); //counter is [0;400], function requires [0;100];
//
	if(AlarmSignalIsPresent)
	{
		static uint16_t SirenPhaseCounter = 5000;
		static uint8_t subSirenCnt = 0;
		if(++subSirenCnt >= 14)
		{
			subSirenCnt = 0;
			Sound(SirenPhaseCounter);
			SirenPhaseCounter -= 100;
			if(SirenPhaseCounter < 1000)
				SirenPhaseCounter = 5000;
		}
	}

	//if(!batteryFlag) GPIO_SetBits(GPIOA, GPIO_Pin_8);	
}


/*******************************************************************************
* Function Name  : ReadSensorsState
* Description    : Check sensors config
*******************************************************************************/
int ReadSensorsState()
{
	int isBroken = 0;
	for(uint8_t i=0; i<SENSORS_NUM; i++)
	{
		int snsCode=0;

		GPIO_InitTypeDef GPIOInitStructure;
		GPIOInitStructure.GPIO_Pin = SensorPin[i * 2];
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init((i * 2) < 5 ? GPIOA : GPIOB, &GPIOInitStructure);

		GPIOInitStructure.GPIO_Pin = SensorPin[i * 2 + 1];
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_IPD;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init((i * 2 + 1) < 5 ? GPIOA : GPIOB, &GPIOInitStructure);

		GPIO_SetBits((i * 2) < 5 ? GPIOA : GPIOB, SensorPin[2 * i]);
		delay_ms(2);
		if(GPIO_ReadInputDataBit((i * 2 + 1) < 5 ? GPIOA : GPIOB, SensorPin[i * 2 + 1]) == Bit_SET)
			snsCode = 1;

		GPIOInitStructure.GPIO_Pin = SensorPin[i * 2 + 1];
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init((i * 2 + 1) < 5 ? GPIOA : GPIOB, &GPIOInitStructure);

		GPIOInitStructure.GPIO_Pin = SensorPin[i * 2];
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_IPD;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init((i * 2) < 5 ? GPIOA : GPIOB, &GPIOInitStructure);

		GPIO_SetBits((i * 2 + 1) < 5 ? GPIOA : GPIOB, SensorPin[2 * i + 1]);
		delay_ms(2);
		if(GPIO_ReadInputDataBit((i * 2) < 5 ? GPIOA : GPIOB, SensorPin[i * 2]) == Bit_SET)
			snsCode = 2;

		if(sensConf[i] != snsCode)
			isBroken = 1;

		GPIOInitStructure.GPIO_Pin = SensorPin[i * 2];
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init((i * 2) < 5 ? GPIOA : GPIOB, &GPIOInitStructure);

		GPIOInitStructure.GPIO_Pin = SensorPin[i * 2 + 1];
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init((i * 2 + 1) < 5 ? GPIOA : GPIOB, &GPIOInitStructure);
	}
	return isBroken;
}


/*******************************************************************************
* Function Name  : WriteSensorsState
* Description    : Read sensors config
*******************************************************************************/
void WriteSensorsState()
{
	for(uint8_t i=0; i<SENSORS_NUM; i++)
	{
		sensConf[i] = 0;

		GPIO_InitTypeDef GPIOInitStructure;
		GPIOInitStructure.GPIO_Pin = SensorPin[i * 2];
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init((i * 2) < 5 ? GPIOA : GPIOB, &GPIOInitStructure);

		GPIOInitStructure.GPIO_Pin = SensorPin[i * 2 + 1];
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_IPD;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init((i * 2 + 1) < 5 ? GPIOA : GPIOB, &GPIOInitStructure);

		GPIO_SetBits((i * 2) < 5 ? GPIOA : GPIOB, SensorPin[2 * i]);
		delay_ms(2);
		if(GPIO_ReadInputDataBit((i * 2 + 1) < 5 ? GPIOA : GPIOB, SensorPin[i * 2 + 1]) == Bit_SET)
			sensConf[i] = 1;

		GPIOInitStructure.GPIO_Pin = SensorPin[i * 2 + 1];
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init((i * 2 + 1) < 5 ? GPIOA : GPIOB, &GPIOInitStructure);
		GPIOInitStructure.GPIO_Pin = SensorPin[i * 2];
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_IPD;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init((i * 2) < 5 ? GPIOA : GPIOB, &GPIOInitStructure);

		GPIO_SetBits((i * 2 + 1) < 5 ? GPIOA : GPIOB, SensorPin[2 * i + 1]);
		delay_ms(2);
		if(GPIO_ReadInputDataBit((i * 2) < 5 ? GPIOA : GPIOB, SensorPin[i * 2]) == Bit_SET)
			sensConf[i] = 2;

		GPIOInitStructure.GPIO_Pin = SensorPin[i * 2];
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init((i * 2) < 5 ? GPIOA : GPIOB, &GPIOInitStructure);

		GPIOInitStructure.GPIO_Pin = SensorPin[i * 2 + 1];
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init((i * 2 + 1) < 5 ? GPIOA : GPIOB, &GPIOInitStructure);
	}
}


/*******************************************************************************
* Function Name  : LED_Flash
*******************************************************************************/
void LED_Flash(uint8_t led, uint8_t nFlash, uint16_t delay)
{
	uint32_t temp = GPIO_ReadInputData(GPIOB);
	LED(LED_RED | LED_GREEN | LED_YLW, LED_OFF);
	for(uint32_t i=0; i<62500; i++) asm("nop");

	for(uint8_t i=0; i<nFlash; i++)
	{
		if(led & LED_YLW)
			GPIO_SetBits(GPIOB, GPIO_Pin_10);
		if(led & LED_RED)
			GPIO_SetBits(GPIOB, GPIO_Pin_2);
		if(led & LED_GREEN)
			GPIO_SetBits(GPIOB, GPIO_Pin_11);

		for(uint32_t i = 0; i < 625 * delay; i++)
			asm("nop");

		if(led & LED_YLW)
			GPIO_ResetBits(GPIOB, GPIO_Pin_10);
		if(led & LED_RED)
			GPIO_ResetBits(GPIOB, GPIO_Pin_2);
		if(led & LED_GREEN)
			GPIO_ResetBits(GPIOB, GPIO_Pin_11);

		for(uint32_t i = 0; i < 625 * delay; i++)
			asm("nop");
	}

	GPIO_Write(GPIOB, temp);
}


/*******************************************************************************
* Function Name  : Beep
*******************************************************************************/
void Beep(uint16_t tone, uint16_t len)
{
	Sound(tone);
	beepCounter = len;
}


/*******************************************************************************
* Function Name  : Unlock
* Description    : Process unlocking of SYSTEM
* Input		 	 : Button code
*******************************************************************************/
void Unlock(uint16_t btnCode)
{
	if(btnCode)
	{
		if(ptrInputCode <= CODE_MAX_SYMB)
		{
			tempCodeMass[ptrInputCode++] = btnCode;

			/* buff mass & SETT CODE sizes - equal */
			if(ptrInputCode == CodeMass[0])
			{
				bool equal = true;
				for(uint8_t i=0; i<ptrInputCode; i++)
				{
					if(CodeMass[i+1] != tempCodeMass[i])
					{
						equal = false;
						break;
					}
				}

				if(equal == true)
				{
					DebugSendString("# Unlocked!");
					workMode = IDLE;
					ptrInputCode = 0;
					unlockCounter = 0;
					return;
				}
			}
		}
		unlockCounter = 4000;
	}
}


/*******************************************************************************
* Function Name  : UpdSensorsConfig
* Description    : Write new sensors config to flash
*******************************************************************************/
int UpdSensorsConfig()
{
	LED_Flash(LED_RED, 4, 250);

	WriteSensorsState();
	FlashWriteData();

	LED_Flash(LED_GREEN, 4, 250);
	return 0;
}


/*******************************************************************************
* Function Name  : TraceCode
* Description    : Debug out - code combination
*******************************************************************************/
void TraceCode()
{
	DebugSendNumWDesc("# CODE: #", CodeMass[0]);

	if(CodeMass[0] > 20) CodeMass[0] = 20;
	for(uint8_t i=0; i<CodeMass[0]; i++)
		DebugSendNum(CodeMass[i+1]);

	DebugSendChar('\n');
}


/*******************************************************************************
* Function Name  : TraceSensorsState
* Description    : Debug out - sensors config
*******************************************************************************/
void TraceSensorsState()
{
	DebugSendString("# SENSORS: #");

	for(uint8_t i=0; i<SENSORS_NUM; i++)
		DebugSendNum(sensConf[i]);

	DebugSendChar('\n');
}


/*******************************************************************************
* Function Name  : BeepSensorsConfig
* Description    : Out sensors config
*******************************************************************************/
void BeepSensorsConfig()
{
	LED(LED_RED | LED_GREEN | LED_YLW, LED_ON);

	for(uint8_t i=0; i<SENSORS_NUM; i++)
	{
		for(uint8_t j=1; j<i+2; j++)
		{
			Beep(2000, 100);
			delay_ms(300);
		}

		if(sensConf[i] != 0)
		{
			Beep(1500, 100);
			delay_ms(500);
		}

		delay_ms(1500);
	}
}


/*******************************************************************************
* Function Name  : FLASH_WrCode
* Description    : Write code combination to flash
*******************************************************************************/
void FLASH_WrCode(uint16_t *mass, uint8_t len)
{
	CodeMass[0] = len;
	for(uint8_t i=0; i<len; i++)
		CodeMass[i+1] = mass[i];
}


/*******************************************************************************
* Function Name  : SaveCode
* Description    : Save new code combination
* Input		 	 : Button code
*******************************************************************************/
void SaveCode(uint16_t btnCode)
{
	static uint8_t ptr = 0;

	(SysTickCounter % 300 > 150)? LED(LED_GREEN, LED_ON): LED(LED_GREEN, LED_OFF);

	if(ptr >= CODE_MAX_SYMB)
		btnCode = BTN_NEWCODE;

	if(btnCode == BTN_NEWCODE)
	{
		if(ptr < CODE_MIN_SYMB)
		{
			DebugSendString("# NEW CODE: too short! Try again.");
			LED_Flash(LED_RED, 5, 250);
		}
		else
		{
			FLASH_WrCode(tempCodeMass, ptr);
			FlashWriteData();
			TraceCode();

			LED_Flash(LED_GREEN, 5, 250);
		}
		ptr = 0;
		workMode = IDLE;
		return;
	}

	if(btnCode == 0)
		return;
	else
		tempCodeMass[ptr++] = btnCode;
}


/*******************************************************************************
* Function Name  : main
* Description    : Main routine
*******************************************************************************/
int main(void)
{
	PeriphInit();

	DebugInit();

//	Sound(2000);
//	delay_ms(1000);
//	while(1)
//	{
//		for(uint16_t i=1000; i<4000; i+=100)
//			//for(uint16_t i=4000; i>1000; i-=100)
//		{
//			Sound2(i);
//			for(uint16_t i=0; i<25000; i++)
//				asm("nop");
//		}
//	}

	Beep(2000, 100);
	delay_ms(100);

	//DebugSendString("$ FLASH READ STARTED-->");

	if(isResetSwitchActivated())
	{
		Beep(1500, 200);
		delay_ms(200);
		Beep(1500, 200);
		delay_ms(200);
		Beep(1500, 200);

		//DebugSendString("$ RESETTING DATA ->");
		//Beep(1500, 800);

		for(uint8_t i = 0; i < SENSORS_NUM; i++)
			sensConf[i] = 0;

		uint16_t defaultCode[] = { BTN1, BTN1, BTN1, BTN1 };
		FLASH_WrCode(defaultCode, 4);

		FlashWriteData();
		workMode = IDLE;
	}

	FlashReadData();
	TraceCode();
	TraceSensorsState();

	//DebugSendString("$ SYSTEM STARTED-->");

    while(1)
    {
    	if(failInputedCode == true)
    	{
    		//DebugSendString("# UNLOCK FAIL!");

			ptrInputCode = 0;
			Beep(2000, 200);
			LED_Flash(LED_RED, 3, 400);
    		failInputedCode = false;
    	}

    	/* Read keyboard */
    	KBState btnCode = KeyboardGetValues();
    	if(btnCode != 0)
    	{
    		//DebugSendNumWDesc("=> KB: ", btnCode);
    		Beep(2000, 50);
    	}


    	if(isPowerActive())
    		LED(LED_YLW, LED_FLASH_20);
    	else if(isBatteryConnected())
    		LED(LED_YLW, LED_FLASH_80);
    	else
    		LED(LED_YLW, LED_OFF);

		/* Handlers */
		if(workMode == IDLE)
		{
			LED(LED_RED, LED_OFF);
			LED(LED_GREEN, LED_ON);

			switch(btnCode)
			{
			case BTN_LOCK:
				if(ReadSensorsState())
				{
					LED(LED_RED, LED_FLASH);
					Beep(4000, 200);
					delay_ms(200);
					Beep(4100, 200);
					delay_ms(200);
					Beep(4200, 200);
					delay_ms(200);
					Beep(4300, 200);
					LED(LED_RED, LED_OFF);
					break;
				}
				workMode = LOCK;
				LED(LED_GREEN, LED_OFF);
				LED(LED_RED, LED_FLASH_20);
				Beep(1000, 200);
				delay_ms(200);
				Beep(2500, 200);
			break;

			case BTN_NEWCODE:
				workMode = SAVECODE;
				LED_Flash(LED_GREEN, 5, 250);
				LED(LED_RED, LED_OFF);
				Beep(1500, 100);
				delay_ms(100);
				Beep(1500, 100);
				break;

			case BTN_SAVESENSORS:
				Beep(1000, 100);
				UpdSensorsConfig();
				BeepSensorsConfig();
				TraceSensorsState();
				//Beep(1000, 100);
			break;

			}
			btnCode = 0;
		}

		if(workMode == SAVECODE)
			SaveCode(btnCode);

		if(workMode == LOCK && isCoverOpened())
			workMode = ALARM;

		if(workMode == LOCK || workMode == ALARM)
		{
			LED(LED_GREEN, LED_OFF);
			LED(LED_RED, workMode == ALARM ? LED_FLASH_20 : LED_ON);

			if(ReadSensorsState())
				workMode = ALARM;

			Unlock(btnCode);
		}

		if(workMode == ALARM)
		{
			AlarmSignalIsPresent = ENABLE;
//			//Sound(800);
//			for(uint16_t i=1000; i<4000; i+=100)
//			//for(uint16_t i=4000; i>1000; i-=100)
//			{
//				Sound2(i);
//				for(uint16_t i = 0; i < 25000; i++)
//					asm("nop");
//			}
		}
		else
			AlarmSignalIsPresent = DISABLE;


//							static uint8_t oldMode = 255;
//
//							if(oldMode != workMode)
//							{
//								oldMode = workMode;
//
//								/* Debug output mode */
//								char tt[20]={'\0'},yy[5] = {'\0'};
//								strcat_(tt, "# new MODE: ");
//								for(uint8_t i=workMode*4,k=0; i<workMode*4+4; i++,k++)
//									yy[k] = WorkModes[i];
//								strcat_(tt, yy);
//								DebugSendString(tt);
//							}

    }
}

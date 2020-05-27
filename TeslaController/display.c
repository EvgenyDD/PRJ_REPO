/* Includes ------------------------------------------------------------------*/
#include "display.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_spi.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_tim.h"
#include "hal_init.h"

#include <math.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define POINT_MASK 	(1<<7)
#define RST_DATA 	GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_10 | GPIO_Pin_11)

#define OE_EN 		GPIO_SetBits(GPIOB, GPIO_Pin_4);
#define OE_DIS 		GPIO_ResetBits(GPIOB, GPIO_Pin_4);



/* Private variables ---------------------------------------------------------*/
static uint8_t digits[12] = {0};
static uint16_t points = 0;

float freq;
float bfreq;

bool startCondition = false;

static ModeType currentMode = MODE_FREQ;

static ButtonState lastButtons[4] = {BTN_OFF, BTN_OFF, BTN_OFF, BTN_OFF};

static const uint8_t fontArray[] = {
				//.GAFBCED
				0b00111111, //0
				0b00001100,
				0b01101011,
				0b01101101,
				0b01011100,
				0b01110101,
				0b01110111,
				0b00101100,
				0b01111111,
				0b01111101, //9
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0b01111110, //A
				0b01010111, //b
				0b00110011, //C
				0b01001111, //d
				0b01110011, //E
				0b01110010, //F
				0b00110111, //G
				0b01011110, //H
				0b00000100, //i
				0b00001101, //J
				0,
				0b00010011, //L
				0,
				0b01000110, //n
				0b01000111, //o
				0b01111010, //P
				0b01111100, //q
				0b01000010, //r
				0,
				0b01010011, //t
				0b00000111, //u
				0,
				0,
				0,
				0b01011101,  //Y
				0b01000011	 //Z
};


/* Extern variables ----------------------------------------------------------*/
extern uint32_t offTimer;


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Display_init
* Description    : Init display module
*******************************************************************************/
void Display_init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_6);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_6);

	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CRCPolynomial = 0;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_Init(SPI3, &SPI_InitStructure);

	SPI_NSSInternalSoftwareConfig(SPI3,SPI_NSSInternalSoft_Set);

	SPI_Cmd(SPI3, ENABLE);
}


/*******************************************************************************
* Function Name  : Display_setPoint
* Description    : Set point on and off to specified indicator
*******************************************************************************/
void Display_setPoint(uint8_t indicator, uint8_t digit, bool point)
{
	if(indicator == 0 && digit < 2)
		BitWrite(point, points, 1-digit);
	if(indicator == 1 && digit < 3)
		BitWrite(point, points, (2-digit)+2);
	if(indicator == 2 && digit < 3)
		BitWrite(point, points, (2-digit)+5);
	if(indicator == 3 && digit < 4)
		BitWrite(point, points, (3-digit)+8);
}

/*******************************************************************************
* Function Name  : Display_resetPoints
* Description    : Reset all indicator points
*******************************************************************************/
void Display_resetPoints()
{
	points = 0;
}

void Display_resetNumbers()
{
	Display_setText(0, "  ");
	Display_setText(1, "   ");
	Display_setText(2, "   ");
	Display_setText(3, "    ");
}


/*******************************************************************************
* Function Name  : Display_setNum
* Description    : Set number to indicator
*******************************************************************************/
void Display_setNum(uint8_t indicator, uint16_t num)
{
	if(indicator == 0)
	{
		digits[0] = num/10%10;
		digits[1] = num%10;
	}
	else if(indicator == 1)
	{
		digits[2] = num/100%10;
		digits[3] = num/10%10;
		digits[4] = num%10;
		if(digits[2] == 0)
		{
			digits[2] = 11;
			if(digits[3] == 0 && !(points & (1<<3)))
				digits[3] = 11;
		}
	}
	else if(indicator == 2)
	{
		digits[5] = num/100%10;
		digits[6] = num/10%10;
		digits[7] = num%10;
		if(digits[5] == 0)
		{
			digits[5] = 11;
			if(digits[6] == 0)
				digits[6] = 11;
		}
	}
	else if(indicator == 3)
	{
		digits[8] = num/1000%10;
		digits[9] = num/100%10;
		digits[10] = num/10%10;
		digits[11] = num%10;
		if(digits[8] == 0)
		{
			digits[8] = 11;
			if(digits[9] == 0)
			{
				digits[9] = 11;
				if(digits[10] == 0 && !(points & (1<<10)))
					digits[10] = 11;
			}
		}
	}
}


/*******************************************************************************
* Function Name  : Display_setText
* Description    : Set text to indicator
*******************************************************************************/
void Display_setText(uint8_t indicator, char *s)
{
	int i = 0;
	if(indicator == 0)
	{
		digits[0] = s[i++] - '0';
		digits[1] = s[i++] - '0';
	}
	else if(indicator == 1)
	{
		digits[2] = s[i++] - '0';
		digits[3] = s[i++] - '0';
		digits[4] = s[i++] - '0';
	}
	else if(indicator == 2)
	{
		digits[5] = s[i++] - '0';
		digits[6] = s[i++] - '0';
		digits[7] = s[i++] - '0';
	}
	else if(indicator == 3)
	{
		digits[8] = s[i++] - '0';
		digits[9] = s[i++] - '0';
		digits[10] = s[i++] - '0';
		digits[11] = s[i++] - '0';
	}
}


/*******************************************************************************
* Function Name  : Display_callback
* Description    : Redraw display
*******************************************************************************/
void Display_callback()
{
	uint8_t address = 0;
	static uint16_t phase = 0;
	if(++phase >= 12)
		phase = 0;

	//b2 b10 b11 b0
	RST_DATA;

	const uint8_t addrByte[8] = { 1 << 7, 1 << 6, 1 << 3, 1 << 4, 1 << 5, 1 << 2, 1 << 1, 1 << 0 };
	const uint16_t addrByteHW[4] = { GPIO_Pin_0, GPIO_Pin_2, GPIO_Pin_10, GPIO_Pin_11 };

	if(phase < 8)
		address = addrByte[phase];
	else
		GPIO_SetBits(GPIOB, addrByteHW[phase - 8]);

	address ^= 0xFF;

	//MSB - address
	//LSB - pixels
	OE_DIS;
	SPI_I2S_SendData16(SPI3, fontArray[digits[phase]] | ((points & (1 << phase)) ? POINT_MASK : 0) | (address << 8));
	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET);
	OE_EN;
}

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static void Display_PFandPL(float x3duty, float x4freq)
{
#define MAX_FREQ (50000)
	freq = map(expf(4 * ((float)x4freq / 4095.0 - 1.0)), expf(-4), 1., 5, /*99999*/MAX_FREQ); //0.1Hz
	float PL = map(x3duty, 0, 4095, 10, 120);

#define LO_LIM	1200
#define HI_LIM	120
	if(TIM1->PSC != HI_LIM-1 && freq > 420)
	{
		TIM1->PSC = HI_LIM - 1;
	}
	else if(TIM1->PSC != LO_LIM-1 && freq < 400)
	{
		TIM1->PSC = LO_LIM - 1;
	}

	//get ARR value
	float ARR = (float) TIM_FREQ * 10.0 / ((float) (TIM1->PSC + 1) * (float) freq);
	if(ARR > 65535)
		ARR = 65535;
	TIM1->ARR = ARR;

	//update real frequency
	freq = 10.0 * (float) TIM_FREQ / ((float) (TIM1->ARR) * (float) (TIM1->PSC + 1));

	// get ccr value
	uint32_t CCR = PL * (float) TIM_FREQ * 0.000001 / (float)(TIM1->PSC + 1);
	if(CCR > TIM1->ARR)
	{
		CCR = TIM1->ARR;
	}
	if(CCR < 1)
	{
		CCR = 1;
	}
	TIM1->CCR1 = CCR;

	//update real pulse length
	PL = ((float) (TIM1->PSC + 1)*1000000.*TIM1->CCR1)/((float) TIM_FREQ);

	Display_setNum(2, PL);

	if(freq < 10000)
	{
		Display_setNum(3, freq);
		Display_setPoint(3, 1, 1);
	} else
	{
		Display_setNum(3, freq / 10.);
		Display_setPoint(3, 1, 0);
	}
}

static void Display_burstMode(float x1duty, float x2burstFreq)
{
	bfreq = map(expf(4 * (x2burstFreq / 4095 - 1)), expf(-4), 1., 1, 201);
	float BRL = map(x1duty, 0.0, 4095.0, 2, 97);

	//update BRF
	float ARR = ((float) TIM_FREQ * 10.0) / ((float) (TIM2->PSC + 1) * (float) bfreq);
	if(ARR > 65535)
	{
		ARR = 65535;
	}

	TIM2->ARR = ARR;

	Display_setNum(0, BRL);

	//update BRL
	TIM2->CCR1 = TIM2->ARR * BRL / 100;
	Display_setNum(1, bfreq);
	Display_setPoint(1, 1, 1);
}

static void Display_singleMode(float x2pulseLen)
{
	float freqq = map(expf(4 * (x2pulseLen / 4095 - 1)), expf(-4), 1., 1, 50);

	//update length
	float ARR = ((float) TIM_FREQ * (float) freqq) / ((float) (TIM2->PSC + 1) * 10.0);
	if(ARR > 65535)
		ARR = 65535;

	TIM2->ARR = ARR;

	TIM2->CCR1 = TIM2->ARR;

	uint32_t freq2 = freqq;
	digits[2] = freq2 / 10 % 10;
	digits[3] = freq2 % 10;
	digits[4] = 'Z' - '0';

	Display_setPoint(1, 2, 1);
}



#define LPF(value, sample, coef) \
				(value -= (coef) * (value - (sample)))
/*******************************************************************************
* Function Name  : Menu_callback
* Description    : Main menu callback
*******************************************************************************/
void Menu_callback(void)
{
	static float y1=0, y2=0, y3=0, y4=0;

#define FILTR_COEF 0.01

	LPF(y1, ADC_read(12), FILTR_COEF);
	LPF(y2, ADC_read(5), FILTR_COEF);
	LPF(y3, ADC_read(4), FILTR_COEF);
	LPF(y4, ADC_read(2), FILTR_COEF);

	switch(currentMode)
	{
	case MODE_FREQ:
	{
		Display_PFandPL(y3, y4);
	}
	break;

	case MODE_SINGLE:
	{
		Display_PFandPL(y3, y4);
		Display_singleMode(y2);
	}
	break;

	case MODE_BURST:
	{
		Display_PFandPL(y3, y4);
		Display_burstMode(y1, y2);
	}
	break;

	case MODE_SOUND:
	{
		Display_setText(3, "AUD ");
	}
	break;

	case MODE_MIDI:
	{
		Display_setText(3, "CB  ");
	}
	break;

	case MODE_USB_SOUND:
	{
		Display_setText(3, "USB ");
	}
	break;

	default:
		break;
	}
}


void UpdateMode()
{
    GPIOA->MODER  &= ~(GPIO_MODER_MODER0 << (8 * 2));
    GPIOA->MODER |= (((uint32_t)GPIO_Mode_OUT) << (8 * 2));
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);

	TIM_Cmd(TIM1, DISABLE);
	TIM_Cmd(TIM2, DISABLE);

	TIM1->CNT = 0;
	TIM2->CNT = 0;

	TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

	switch(currentMode)
	{
	case MODE_NONE:
		break;

	case MODE_FREQ:
		TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Reset);
		TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Disable);
		TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);
		TIM_SelectInputTrigger(TIM1, TIM_TS_ITR0);

		Display_setPoint(3, 1, 1);

		Display_setText(0, "    ");
		Display_setText(1, "    ");
		break;

	case MODE_SINGLE:
		Display_setPoint(1, 2, 1);
		Display_setPoint(3, 1, 1);

		Display_setText(0, "    ");

		TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Gated);
		TIM_SelectInputTrigger(TIM1, TIM_TS_ITR1);

		TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_OC1Ref);
		TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
		break;

	case MODE_BURST:
		Display_setPoint(1, 1, 1);
		Display_setPoint(3, 1, 1);

		TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Gated);
		TIM_SelectInputTrigger(TIM1, TIM_TS_ITR1);

		TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_OC1Ref);
		TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

//		TIM_Cmd(TIM1, ENABLE);
//		TIM_Cmd(TIM2, ENABLE);
		break;

	default:
		break;
	}
}


extern uint32_t SysTickMSCounter;


uint16_t buttonsDelay[4] = {0,0,0,0};

void Buttons_delayCallback(void)
{
	for(uint8_t i=0; i<4; i++)
		if(buttonsDelay[i] > 0) {buttonsDelay[i]--;}
}

void Buttons_callback(void)
{
	static ModeType lastMode = MODE_NONE;

	if(currentMode == MODE_NONE)
	{
		currentMode = MODE_FREQ;
	}

	if(lastMode != currentMode)
	{
		lastMode = currentMode;
	}

	if(currentMode != MODE_MIDI)
	{
		static uint32_t blinkCnt = 0;
		if(SysTickMSCounter > blinkCnt + 400)
		{
			blinkCnt = SysTickMSCounter;

			switch(currentMode)
			{
			case MODE_FREQ:
				GPIO_SetBits(GPIOF, GPIO_Pin_0);
				break; //WHT
			case MODE_BURST:
				GPIO_SetBits(GPIOC, GPIO_Pin_13);
				break; //RED
			case MODE_SINGLE:
				GPIO_SetBits(GPIOC, GPIO_Pin_15);
				break; //YLW
			case MODE_SOUND:
				GPIO_SetBits(GPIOC, GPIO_Pin_14);
				break; //GRN
			case MODE_USB_SOUND:
				GPIO_SetBits(GPIOC, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
				GPIO_SetBits(GPIOF, GPIO_Pin_0);
				break;
			default:
				break;
			}
		}
		else if(SysTickMSCounter > blinkCnt + 20)
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
			GPIO_ResetBits(GPIOF, GPIO_Pin_0);
		}
	}
	if(currentMode == MODE_MIDI)
	{
		static uint32_t SysTickMSCounterLast=0;
		if(SysTickMSCounterLast + 150 < SysTickMSCounter)
		{
			SysTickMSCounterLast = SysTickMSCounter;

			static uint8_t lastLed = 0;
			GPIO_ResetBits(GPIOC, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
			GPIO_ResetBits(GPIOF, GPIO_Pin_0);
			switch(lastLed)
			{
			case 0:
				GPIO_SetBits(GPIOC, GPIO_Pin_15);
				break;
			case 1:
				GPIO_SetBits(GPIOC, GPIO_Pin_14);
				break;
			case 2:
				GPIO_SetBits(GPIOC, GPIO_Pin_13);
				break;
			}
			if(++lastLed > 2)
				lastLed = 0;
		}
	}


	if(buttonsDelay[0] == 0)
	{
		if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6))
		{
			if(lastButtons[0] == BTN_OFF)
				lastButtons[0] = BTN_PRESS;
			else
			{
				lastButtons[0] = BTN_ON;
				buttonsDelay[0] = 100;
			}
		}
		else
		{
			if(lastButtons[0] == BTN_ON)
				lastButtons[0] = BTN_RELEASE;
			else
				lastButtons[0] = BTN_OFF;
		}
	}

	if(buttonsDelay[1] == 0)
	{
		if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7))
		{
			if(lastButtons[1] == BTN_OFF)
				lastButtons[1] = BTN_PRESS;
			else
			{
				lastButtons[1] = BTN_ON;
				buttonsDelay[1] = 100;
			}
		}
		else
		{
			if(lastButtons[1] == BTN_ON)
				lastButtons[1] = BTN_RELEASE;
			else
				lastButtons[1] = BTN_OFF;
		}
	}

	if(buttonsDelay[2] == 0)
	{
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
		{
			if(lastButtons[2] == BTN_OFF)
				lastButtons[2] = BTN_PRESS;
			else
			{
				lastButtons[2] = BTN_ON;
				buttonsDelay[2] = 100;
			}
		}
		else
		{
			if(lastButtons[2] == BTN_ON)
				lastButtons[2] = BTN_RELEASE;
			else
				lastButtons[2] = BTN_OFF;
		}
	}

	if(buttonsDelay[3] == 0)
	{
		if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12))
		{
			if(lastButtons[3] == BTN_OFF)
				lastButtons[3] = BTN_PRESS;
			else
			{
				lastButtons[3] = BTN_ON;
				buttonsDelay[3] = 100;
			}
		}
		else
		{
			if(lastButtons[3] == BTN_ON)
				lastButtons[3] = BTN_RELEASE;
			else
				lastButtons[3] = BTN_OFF;
		}
	}

	for(int i=0; i<4; i++)
	{
		if(lastButtons[i] != BTN_OFF)
		{
			offTimer = 0;
		}
	}

	if(lastButtons[BTN_LEFT] == BTN_PRESS)
	{
		if(++currentMode > MODE_MIDI)
			currentMode = MODE_MIDI;

		UpdateMode();

		Display_resetPoints();
		Display_resetNumbers();
	}

	if((lastButtons[BTN_CENTER] == BTN_RELEASE && SysTickMSCounter > 3000) ||
			offTimer >= 300000)
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_15);
	}



	if(lastButtons[BTN_RIGHT] == BTN_PRESS)
	{
		if(--currentMode > MODE_MIDI)
			currentMode = MODE_MIDI;

		UpdateMode();

		Display_resetPoints();
		Display_resetNumbers();
	}

	if(lastButtons[BTN_FIRE] == BTN_PRESS)
	{
        GPIOA->MODER  &= ~(GPIO_MODER_MODER0 << (8 * 2));
        GPIOA->MODER |= (((uint32_t)GPIO_Mode_AF) << (8 * 2));

        startCondition = true;
		TIM_GenerateEvent(TIM1, TIM_EventSource_Update);
		TIM_GenerateEvent(TIM2, TIM_EventSource_Update);


		TIM_Cmd(TIM1, ENABLE);
		if(currentMode != MODE_FREQ)
		{
			TIM_Cmd(TIM2, ENABLE);
		}
	}

	if(lastButtons[BTN_FIRE] == BTN_RELEASE)
	{
		TIM_Cmd(TIM1, DISABLE);
		TIM_Cmd(TIM2, DISABLE);
		TIM1->CNT = 0;
		TIM2->CNT = 0;

        GPIOA->MODER  &= ~(GPIO_MODER_MODER0 << (8 * 2));
        GPIOA->MODER |= (((uint32_t)GPIO_Mode_OUT) << (8 * 2));
    	GPIO_ResetBits(GPIOA, GPIO_Pin_8);
	}
}


void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

		if(startCondition)
		{
			startCondition = false;
		}
		else
		{
			if(currentMode == MODE_SINGLE)
			{
				TIM_Cmd(TIM2, DISABLE);
				TIM_SetCounter(TIM2, 0);

				TIM_Cmd(TIM1, DISABLE);

				GPIOA->MODER  &= ~(GPIO_MODER_MODER0 << (8 * 2));
				GPIOA->MODER |= (((uint32_t)GPIO_Mode_OUT) << (8 * 2));

				GPIO_ResetBits(GPIOA, GPIO_Pin_8);
			}
		}
	}
}

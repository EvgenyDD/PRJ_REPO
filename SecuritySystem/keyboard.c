/* Includes ------------------------------------------------------------------*/
#include "keyboard.h"
#include "hw_config.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint16_t butCdTmr = 0;
uint16_t lastState = 0;


/* Extern variables ----------------------------------------------------------*/
void delay_ms(uint16_t time);


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  :
* Description    :
* Input		 :
*******************************************************************************/

/*******************************************************************************
* Function Name  :
* Description    :
*******************************************************************************/
KBState KeyboardGetValues()
{
	if(butCdTmr)
		return 0;

	KBState x=0;
	GPIO_InitTypeDef GPIOInitStructure;

	/* 1 */
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIOInitStructure);

	GPIO_ResetBits(GPIOA, GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
	delay_ms(1);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIOInitStructure);

		GPIOInitStructure.GPIO_Pin = ROW1;
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOB, &GPIOInitStructure);

		GPIOInitStructure.GPIO_Pin = ROW2 | ROW3;
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOA, &GPIOInitStructure);

	GPIO_SetBits(GPIOB, ROW1);
	//GPIO_ResetBits(GPIOA, ROW2);
	//GPIO_ResetBits(GPIOA, ROW3);
	delay_ms(1);

	BitWrite(BitIsSet(GPIOA->IDR, COL1), x, 9);
	BitWrite(BitIsSet(GPIOA->IDR, COL2), x, 8);
	BitWrite(BitIsSet(GPIOA->IDR, COL3), x, 7);

	GPIO_ResetBits(GPIOA, ROW1);
	delay_ms(1);

	/* 2 */
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIOInitStructure);

	GPIO_ResetBits(GPIOA, GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
	delay_ms(1);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIOInitStructure);

		GPIOInitStructure.GPIO_Pin = ROW1;
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOB, &GPIOInitStructure);

		GPIOInitStructure.GPIO_Pin = ROW2;
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOA, &GPIOInitStructure);

		GPIOInitStructure.GPIO_Pin = ROW3;
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOA, &GPIOInitStructure);

	///GPIO_ResetBits(GPIOB, ROW1);
	GPIO_SetBits(GPIOA, ROW2);
	//GPIO_ResetBits(GPIOA, ROW3);
	delay_ms(1);

	BitWrite(BitIsSet(GPIOA->IDR, COL1), x, 6);
	BitWrite(BitIsSet(GPIOA->IDR, COL2), x, 5);
	BitWrite(BitIsSet(GPIOA->IDR, COL3), x, 4);

	GPIO_ResetBits(GPIOA, ROW2);
	delay_ms(1);

	/* 3 */
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIOInitStructure);

	GPIO_ResetBits(GPIOA, GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
	delay_ms(1);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIOInitStructure);

		GPIOInitStructure.GPIO_Pin = ROW1;
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOB, &GPIOInitStructure);

		GPIOInitStructure.GPIO_Pin = ROW2;
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOA, &GPIOInitStructure);

		GPIOInitStructure.GPIO_Pin = ROW3;
		GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOA, &GPIOInitStructure);

	//GPIO_ResetBits(GPIOB, ROW1);
	//GPIO_ResetBits(GPIOA, ROW2);
	GPIO_SetBits(GPIOA, ROW3);
	delay_ms(1);

	BitWrite(BitIsSet(GPIOA->IDR, COL1), x, 3);
	BitWrite(BitIsSet(GPIOA->IDR, COL2), x, 2);
	BitWrite(BitIsSet(GPIOA->IDR, COL3), x, 1);

	GPIO_ResetBits(GPIOA, ROW3);

	/* unpress -> first press */
	if(lastState == 0)
	{
		if(x)
		{
			butCdTmr = 140;
			lastState = x;
			return 0;
		}
	}
	/* val returned -> wait while unpress */
	if(lastState == 0xFFFF)
	{
		if(x == 0)
		{
			lastState = 0;
		}
	}
	/* timer->0 -> need return val */
	if(lastState>0 && lastState!=0xFFFF)
	{
		uint16_t ret = lastState | x;
		lastState = 0xFFFF;
		return ret;
	}

	return 0;
}

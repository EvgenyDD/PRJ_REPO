/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "keyboard.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include <stdbool.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Note frequencies (in Hz)
 * from C - low octave
 * till H - 4th octale
 * octave length - 12
 */
const uint16_t NoteMass[12*5] =
{
	131, 139, 148, 156, 165, 175, 185, 196, 207, 220, 233, 247,
	262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
	523, 554, 587, 622, 659, 698, 740, 784, 830, 880, 932, 988,
	1046,1109,1174,1244,1318,1397,1480,1568,1661,1720,1865,1975,
	2093,2217,2349,2489,2637,2794,2960,3136,3332,3440,3729,3951
};


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name : PeriphInit
* Description   : Initialize peripheral
*******************************************************************************/
void PeriphInit()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	// Enable HSE
	RCC_HSEConfig(RCC_HSE_ON);
	if(RCC_WaitForHSEStartUp() != 0)
	{
		// HCLK = SYSCLK
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		// PCLK2 = HCLK
		RCC_PCLK2Config(RCC_HCLK_Div1);
		// PCLK1 = HCLK /2
		RCC_PCLK1Config(RCC_HCLK_Div1);
		// PLL Clock
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_2);
		// Enable PLL
		RCC_PLLCmd(ENABLE);
		// Wait till PLL is ready
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
		// Select PLL as system clock source
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		// Wait till PLL is used as system clock source
		while (RCC_GetSYSCLKSource() != 0x08);
	}

	//System Clock = (8Mhz /8 *8) / 1000 = 1000Hz = 1ms reload
	SysTick_Config(8000);

	/* Battery sense */
	GPIO_InitTypeDef GPIOInitStructure;
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIOInitStructure);

	GPIOInitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIOInitStructure);

	/* Code reset */
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIOInitStructure);

	/* COL[1:3] */
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIOInitStructure);

	/* Buzzer */
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIOInitStructure);

	/* ROW [2:3] + LEDS */
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIOInitStructure);

	/* ROW 1 */
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIOInitStructure);

	/* pwr sense */
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIOInitStructure);

	/* LEDS */
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIOInitStructure);

	/* Code reset */
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIOInitStructure);

	GPIO_ResetBits(GPIOB, ROW1);
	GPIO_ResetBits(GPIOA, ROW2);
	GPIO_ResetBits(GPIOA, ROW3);


	/* Sound */
	//GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler = 2;
	TIM_TimeBaseStructure.TIM_Period = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_Pulse = 128; //?
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);

	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}

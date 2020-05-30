/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include <stdbool.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
LEDState LED_R=LED_OFF, LED_Y=LED_OFF, LED_G=LED_OFF;


/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name : LED
* Input value   : led - LED_YLW | LED_RED | LED_GREEN
* 				  state - LED_OFF | LED_ON | LED_FLASH
*******************************************************************************/
void LED(LEDNum led, LEDState state)
{
	if(state == LED_OFF)
	{
		if(led & LED_GREEN)
			GPIO_ResetBits(GPIOB, GPIO_Pin_11);
		if(led & LED_YLW)
			GPIO_ResetBits(GPIOB, GPIO_Pin_10);
		if(led & LED_RED)
			GPIO_ResetBits(GPIOB, GPIO_Pin_2);
	}
	else if(state == LED_ON)
	{
		if(led & LED_GREEN)
			GPIO_SetBits(GPIOB, GPIO_Pin_11);
		if(led & LED_YLW)
			GPIO_SetBits(GPIOB, GPIO_Pin_10);
		if(led & LED_RED)
			GPIO_SetBits(GPIOB, GPIO_Pin_2);
	}
//	else if(state == LED_FLASH)
	{
		if(led & LED_GREEN)
			LED_G = state;
		if(led & LED_YLW)
			LED_Y = state;
		if(led & LED_RED)
			LED_R = state;
	}
}


/*******************************************************************************
* Function Name : LED_Callback
* Description   : Process LED flashing
*******************************************************************************/
void LED_Callback(uint8_t phase)
{
	if(phase == 0)
	{
		if(LED_G >= LED_FLASH)
			GPIO_ResetBits(GPIOB, GPIO_Pin_11);
		if(LED_Y >= LED_FLASH)
			GPIO_ResetBits(GPIOB, GPIO_Pin_10);
		if(LED_R >= LED_FLASH)
			GPIO_ResetBits(GPIOB, GPIO_Pin_2);
	}
	else if(phase == 20)
	{
		if(LED_G == LED_FLASH_20)
			GPIO_SetBits(GPIOB, GPIO_Pin_11);
		if(LED_Y == LED_FLASH_20)
			GPIO_SetBits(GPIOB, GPIO_Pin_10);
		if(LED_R == LED_FLASH_20)
			GPIO_SetBits(GPIOB, GPIO_Pin_2);
	}
	else if(phase == 80)
	{
		if(LED_G == LED_FLASH_80)
			GPIO_SetBits(GPIOB, GPIO_Pin_11);
		if(LED_Y == LED_FLASH_80)
			GPIO_SetBits(GPIOB, GPIO_Pin_10);
		if(LED_R == LED_FLASH_80)
			GPIO_SetBits(GPIOB, GPIO_Pin_2);
	}
}

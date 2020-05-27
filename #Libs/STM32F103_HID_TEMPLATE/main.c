/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"

#include "misc.h"
#include "core_cm3.h"

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include "usb_desc.h"


#include <stdbool.h>



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint32_t delay = 0;

uint8_t buttons;
uint8_t USBmode = STANDALONE; //PC sets cube working mode

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : __delay_ms.
* Description    : Delay the code on the N cycles of SysTick Timer.
* Input          : N Delay Cycles.
*******************************************************************************/
void __delay_ms( volatile uint32_t nTime )
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
	if( delay ) delay--;

	//if(bDeviceState == CONFIGURED)	CUBECheckState();
}


/*******************************************************************************
* Function Name  : main
* Description    : Main routine
*******************************************************************************/
int main(void)
{
	Initialization();


	while (1)
	{

#if 1
		if(bDeviceState == CONFIGURED)
		{
			//CUBECheckState();

			/*if(buttonsState& (1<<0)) CUBECheckState();
			if(buttonsState& (1<<1)) CUBECheckState2();*/
		}
		//else for(uint8_t i=0; i<7; i++) Matrix[i] = 1;
#endif



		if( (USBmode == STANDALONE) || (USBmode == PC_MONITORING) )
		{

		}


	}

	//NVIC_SystemReset();
}


void TIM2_IRQHandler()
{
	//main();
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

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

#include "Display.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint32_t delay = 0;


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
	DisplayInit();
	DisplayString("__________");


	while (1)
	{


#if 1
		if(bDeviceState == CONFIGURED)
		{
			handler_usb_rs();
			__delay_ms(10);

			static uint8_t sss = 0;

			DisplayNumAtPos(sss++, 8);

			static uint16_t rrr= 0;

			if(sss > 99)
			{
				sss = 0;

				if(++rrr == 5)
				{
					rrr = 0;
					To_USB_Send_Str("Device Is Alive!!!\n", 19);
				}
			}



			//CUBECheckState();

		}
#endif

	}

	//NVIC_SystemReset();
}


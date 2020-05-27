/* Includes ------------------------------------------------------------------*/
#include "hal_init.h"
#include "stm32f30x.h"
#include "stm32f30x_usart.h"

#include "display.h"


uint32_t delayCounter = 0;
uint32_t SysTickMSCounter = 0;
uint32_t offTimer = 0;

/*---------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
 * Function Name : SysTick_Handler
 * Description	 : 1ms handler
 *******************************************************************************/
void SysTick_Handler()
{
	SysTickMSCounter++; // for debug

	if(delayCounter)
		delayCounter--;

	Buttons_delayCallback();

	offTimer++;
}

// Прерывание
void USART2_IRQHandler()
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		//*uart_buffer_ptr++ = (uint8_t)(USART2->RDR & (uint16_t)0x00FF);

	}

	USART_ClearFlag(USART2,
					USART_FLAG_WU | USART_FLAG_CM | USART_FLAG_EOB | USART_FLAG_RTO | USART_FLAG_CTS | USART_FLAG_LBD
									| USART_FLAG_TC | USART_FLAG_IDLE | USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE
									| USART_FLAG_PE);
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


extern uint16_t phase;
/*******************************************************************************
 * Function Name  : main
 *******************************************************************************/
int main(void)
{
	PowerInit();
	LED_WRITE_0(1);
	LED_WRITE_1(1);

	SysTick_Config(36000);
	Display_init();

	UpdateMode();



	TIM_Cmd(TIM1, ENABLE);
	TIM_Cmd(TIM2, ENABLE);


	while(1)
	{
		delay_ms(1);

//		GPIO_WriteBit(GPIOC, GPIO_Pin_14, !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6));
//		GPIO_WriteBit(GPIOC, GPIO_Pin_13, !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7));
//		GPIO_WriteBit(GPIOC, GPIO_Pin_15, GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0));
//		GPIO_WriteBit(GPIOF, GPIO_Pin_0, !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12));

		//GPIO_SetBits(GPIOF, GPIO_Pin_0);//1 wht
		//GPIO_SetBits(GPIOC, GPIO_Pin_14);//3 grn
		//GPIO_SetBits(GPIOC, GPIO_Pin_13);//4 red
		//GPIO_SetBits(GPIOC, GPIO_Pin_15);//2 ylw

		Buttons_callback();


		static uint16_t cnt = 0;
		if(++cnt >= 10)
		{
			cnt = 0;

			static uint16_t i=0;
			if(++i >=1500)
				i = 0;
//			Display_setNum(0, (1000-i)/30);
//			Display_setNum(1, i*2/7);
//			Display_setNum(2, (1000-i)*8/7);
//			if(i<500)
//				Display_setText(3, "BAD");
//			else if(i<1000)
//				Display_setNum(3, i*34/7);
//			else
//				Display_setText(3, "GOOD");
		}

		//reset LEDs
		static uint16_t cntr1 = 0;
		if(++cntr1 >= 200)
		{
			LED_WRITE_0(0);
			LED_WRITE_1(0);
			cntr1 = 0;
		}

		Menu_callback();
		Display_callback();
	}
}

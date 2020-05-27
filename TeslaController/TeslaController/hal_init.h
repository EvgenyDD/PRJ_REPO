/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _POWER_H
#define _POWER_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32f30x.h"
#include "stm32f30x_gpio.h"

/* Exported macro ------------------------------------------------------------*/
#define LED_WRITE_0(x) 		GPIO_WriteBit(GPIOB, GPIO_Pin_12, x)
#define LED_WRITE_1(x) 		GPIO_WriteBit(GPIOB, GPIO_Pin_13, x)
#define LED_FLIP_0			GPIOB->ODR ^= (GPIO_Pin_12)
#define LED_FLIP_1			GPIOB->ODR ^= (GPIO_Pin_13)

/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void PowerInit(void);
void init_UART(void);
uint8_t CANInit(void);

#endif //_POWER_H

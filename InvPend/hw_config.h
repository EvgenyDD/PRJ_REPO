/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "misc.h"

/* Exported types ------------------------------------------------------------*/
typedef uint8_t LEDState;
typedef uint8_t LEDNum;

/* Exported constants --------------------------------------------------------*/
enum {LED_OFF, LED_ON, LED_FLASH};
enum {LED_RED=1, LED_GREEN=2, LED_BLUE=4};

/* Exported macro ------------------------------------------------------------*/
#define BitSet(p,m) ((p) |= (1<<(m)))
#define BitReset(p,m) ((p) &= ~(1<<(m)))
#define BitFlip(p,m) ((p) ^= (1<<(m)))
#define BitWrite(c,p,m) ((c) ? BitSet(p,m) : BitReset(p,m))
#define BitIsSet(reg, bit) (((reg) & (1<<(bit))) != 0)
#define BitIsReset(reg, bit) (((reg) & (1<<(bit))) == 0)

/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void PeriphInit();
void LED(LEDNum led, LEDState state);
void LED_Callback();
void Motor(int16_t value);

/* External variables --------------------------------------------------------*/

#endif  /*__HW_CONFIG_H*/

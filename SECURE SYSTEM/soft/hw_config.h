/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "misc.h"

/* Exported types ------------------------------------------------------------*/
typedef uint8_t LEDState;
typedef uint8_t LEDNum;

/* Exported constants --------------------------------------------------------*/
enum {LED_OFF, LED_ON, LED_FLASH, LED_FLASH_20, LED_FLASH_80};
enum {LED_RED=1, LED_GREEN=2, LED_YLW=4, LED_ALL=7};

enum Notes{C, Cd, D, Dd, E, F, Fd, G, Gd, A, B, H};


/* Exported macro ------------------------------------------------------------*/
#define BitSet(p,m) ((p) |= (1<<(m)))
#define BitReset(p,m) ((p) &= ~(1<<(m)))
#define BitFlip(p,m) ((p) ^= (m))
#define BitWrite(c,p,m) ((c) ? BitSet(p,m) : BitReset(p,m))
#define BitIsSet(reg, bit) (((reg) & (1<<(bit))) != 0)
#define BitIsReset(reg, bit) (((reg) & (1<<(bit))) == 0)


/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void PeriphInit();
void LED(LEDNum led, LEDState state);
void LED_Flash(LEDNum led, uint8_t numFlash, uint16_t delay);
void LED_Callback(uint8_t);
void PeriphInit();
void Unlock(uint16_t);


/* External variables --------------------------------------------------------*/
extern const uint16_t NoteMass[12*5];

#endif  /*__HW_CONFIG_H*/

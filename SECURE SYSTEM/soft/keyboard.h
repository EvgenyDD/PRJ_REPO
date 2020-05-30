/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef KEYBOARD_H
#define KEYBOARD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "debug.h"

/* Exported types ------------------------------------------------------------*/
typedef uint16_t KBState;

/* Exported constants --------------------------------------------------------*/
#define ROW1	GPIO_Pin_0
#define ROW2	GPIO_Pin_7
#define ROW3	GPIO_Pin_6
#define COL1	5
#define COL2	4
#define COL3	3

#define BTN1	(1<<1)
#define BTN2	(1<<2)
#define BTN3	(1<<3)
#define BTN4	(1<<4)
#define BTN5	(1<<5)
#define BTN6	(1<<6)
#define BTN7	(1<<7)
#define BTN8	(1<<8)
#define BTN9	(1<<9)

#define BTN_NEWCODE 	(BTN1|BTN3|BTN5|BTN7|BTN9)
#define BTN_LOCK		(BTN1|BTN3|BTN7|BTN9)
#define BTN_SAVESENSORS	(BTN2|BTN4|BTN6|BTN8)

/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
KBState KeyboardGetValues();


#endif //KEYBOARD_H

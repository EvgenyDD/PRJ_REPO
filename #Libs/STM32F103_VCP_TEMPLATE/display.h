/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DISPLAY_H
#define DISPLAY_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define DISPLAY_WIDTH	10


/* Exported define -----------------------------------------------------------*/
#define D_WR 	GPIO_Pin_12
#define D_EN 	GPIO_Pin_2
#define D_RS 	GPIO_Pin_9

#define D_WR_PORT 	GPIOC
#define D_EN_PORT 	GPIOD
#define D_RS_PORT 	GPIOC

#define D_DATA_PORT	GPIOB

// Function configuration
#define LCD_SET_FUNCTION 	(1 << 5)
#define LCD_FUNCTION_5X8 	0
#define LCD_FUNCTION_5X10 	(1 << 2)
#define LCD_FUNCTION_1LINE 	0
#define LCD_FUNCTION_2LINE	(1 << 3)
#define LCD_FUNCTION_4BIT 	0
#define LCD_FUNCTION_8BIT	(1 << 4)


// Curser home
#define LCD_CURSOR_HOME       0x02

// Entry mode
#define LCD_SET_ENTRY         (1 << 2)
#define LCD_ENTRY_NOSHIFT     0
#define LCD_ENTRY_SHIFT       (1 << 0)
#define LCD_ENTRY_DECREASE    0
#define LCD_ENTRY_INCREASE    (1 << 1)

// Display configuration
#define LCD_SET_DISPLAY       (1 << 3)
#define LCD_BLINKING_OFF      0
#define LCD_BLINKING_ON       (1 << 0)
#define LCD_CURSOR_OFF        0
#define LCD_CURSOR_ON         (1 << 1)
#define LCD_DISPLAY_OFF       0
#define LCD_DISPLAY_ON        (1 << 2)

// Shift configuration
#define LCD_SET_SHIFT         (1 << 4)
#define LCD_SHIFT_LEFT        0
#define LCD_SHIFT_RIGHT       (1 << 2)
#define LCD_CURSOR_MOVE       0
#define LCD_DISPLAY_SHIFT     (1 << 3)

// Clear Display
#define LCD_CLEAR_DISPLAY     0x01

// Cursor -> HOME
#define LCD_CURSOR_HOME       0x02

// Address configuration
#define LCD_SET_CGADR         (1 << 6)
#define LCD_SET_DDADR         (1 << 7)


/* Exported macro ------------------------------------------------------------*/
#define D_DATA_OUT(val)		GPIO_SetBits(D_DATA_PORT, (val)<<8);\
							GPIO_ResetBits(D_DATA_PORT, (~(val))<<8);

#define D_WR_WRITE 	GPIO_ResetBits(D_WR_PORT, D_WR);
#define D_WR_READ 	GPIO_SetBits(D_WR_PORT, D_WR);

#define D_RS_COM 	GPIO_ResetBits(D_RS_PORT, D_RS);
#define D_RS_DATA 	GPIO_SetBits(D_RS_PORT, D_RS);

#define D_EN_HIGH 	GPIO_SetBits(D_EN_PORT, D_EN);
#define D_EN_LOW 	GPIO_ResetBits(D_EN_PORT, D_EN);


/* Exported functions ------------------------------------------------------- */
void DisplayInit();
void DisplayWriteCom(uint8_t data);
void DisplayWriteData(uint8_t data);

void DisplaySetCursor(uint8_t position);
void DisplayClear();
void DisplayChar(uint8_t t);
void DisplayString(uint8_t * s);
void DisplayStringRelative(uint8_t * s, uint8_t pos);
void DisplayStringCentered(uint8_t * s);

void DisplayNum(int32_t num);
void DisplayNumAtPos(int32_t num, uint8_t pos);
void DisplayNumWDesc(uint8_t *s, int32_t num);

void DisplayBacklightOn();
void DisplayBacklightOff();
void DisplayBacklightSetState(bool state);

uint8_t DisplayCheckIfTheSameContent(uint8_t* s);

void DisplayWriteCustomChar(uint8_t t);
char IsCharInEngPage(uint8_t t);

void DisplayCursorBlink(uint8_t pos);

void DisplayCursorOn();
void DisplayCursorOff();


#endif //DISPLAY_H

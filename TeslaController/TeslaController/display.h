/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DISPLAY_H
#define DISPLAY_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"
#include <stdbool.h>


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define BitSet(p,m) ((p) |= (1<<(m)))
#define BitReset(p,m) ((p) &= ~(1<<(m)))
#define BitFlip(p,m) ((p) ^= (m))
#define BitWrite(c,p,m) ((c) ? BitSet(p,m) : BitReset(p,m))


/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Display_init();
void Display_setPoint(uint8_t indicator, uint8_t digit, bool point);
void Display_setNum(uint8_t indicator, uint16_t num);
void Display_setText(uint8_t indicator, char *s);
void Display_callback();


#endif //DISPLAY_H

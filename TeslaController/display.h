/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DISPLAY_H
#define DISPLAY_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"
#include <stdbool.h>


/* Exported types ------------------------------------------------------------*/
typedef enum{
	MODE_NONE=0,

	MODE_FREQ,
	MODE_BURST,
	MODE_SINGLE,
	MODE_SOUND,
	MODE_MIDI,
	MODE_USB_SOUND
}ModeType;

enum{
	BTN_LEFT=0,
	BTN_RIGHT,
	BTN_CENTER,
	BTN_FIRE
};


typedef enum{
	BTN_OFF,
	BTN_PRESS,
	BTN_RELEASE,
	BTN_ON
}ButtonState;


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
void Display_resetPoints();
void Display_setNum(uint8_t indicator, uint16_t num);
void Display_setText(uint8_t indicator, char *s);
void Display_callback(void);

void UpdateMode();

void Menu_callback(void);

void Buttons_delayCallback(void);
void Buttons_callback(void);


#endif //DISPLAY_H

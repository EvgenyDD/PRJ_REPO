/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CONTROLLER_H
#define CONTROLLER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
//#define CENTER (1910+7) //1900 for short arm link
#define LIMIT 500
#define STRT_VAL 80.0


/* Exported macro ------------------------------------------------------------*/
#define abs(x) ((x) >= 0 ? (x) : -(x))


/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void InitPWMController();
int16_t PWMController(uint16_t center, uint16_t newVal);
int16_t VirtZeroController(int16_t realTrack, uint16_t targetPos);

#endif //CONTROLLER_H

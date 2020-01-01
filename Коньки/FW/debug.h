/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DEBUG_H
#define DEBUG_H


/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <ioavr.h>
#include <intrinsics.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void DebugInit();
void DebugSendString(char*);
void DebugSendChar(char);
void DebugSysTimeEnd();
void DebugSysTimeStart();
void DebugSendNum(int Num);
void DebugSendNumWDesc(char *string, int Num);

#endif //DEBUG_H

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FLASH_H
#define FLASH_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdbool.h>


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define FLASH_KEY1                         ((uint32_t)0x45670123)
#define FLASH_KEY2                         ((uint32_t)0xCDEF89AB)
#define FLASH_PAGE                         ((uint8_t)0x7F)

/* Exported functions ------------------------------------------------------- */
uint8_t FLASH_IsReady();
void FLASH_ErasePage(uint32_t address);
void FLASH_Unlock();
void FLASH_Lock();
void FLASH_Write(uint32_t address,uint32_t data);
uint32_t FLASH_Read(uint32_t address);
void FLASH_WritePage(uint32_t st_address, uint16_t *mass, uint16_t len);

#endif //FLASH_H

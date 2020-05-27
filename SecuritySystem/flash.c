/* Includes ------------------------------------------------------------------*/
#include "flash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  :
* Description    :
* Input		 :
*******************************************************************************/

/*******************************************************************************
* Function Name  : FLASH_IsReady
* Description    : Check if FLASH is ready
*******************************************************************************/
uint8_t FLASH_IsReady() {
        return !(FLASH->SR & FLASH_SR_BSY);
}


/*******************************************************************************
* Function Name  : FLASH_ErasePage
* Description    : Erase flash page
* Input			 : page adress
*******************************************************************************/
void FLASH_ErasePage(uint32_t address)
{
        FLASH->CR |= FLASH_CR_PER;
        FLASH->AR = address;
        FLASH->CR |= FLASH_CR_STRT;
        while(!FLASH_IsReady());
        FLASH->CR &= ~FLASH_CR_PER;
}


/*******************************************************************************
* Function Name  : FLASH_Unlock
* Description    : Unlock the flash
*******************************************************************************/
void FLASH_Unlock(){
          FLASH->KEYR = FLASH_KEY1;
          FLASH->KEYR = FLASH_KEY2;
}


/*******************************************************************************
* Function Name  : FLASH_Lock
* Description    : Lock te flash
*******************************************************************************/
void FLASH_Lock(){
        FLASH->CR |= FLASH_CR_LOCK;
}


/*******************************************************************************
* Function Name  : FLASH_Write
* Description    : Write uint32_t to FLASH
* Input			 : adress, data
*******************************************************************************/
void FLASH_Write(uint32_t address,uint32_t data)
{
        FLASH->CR |= FLASH_CR_PG;
        while(!FLASH_IsReady());

        *(__IO uint16_t*)address = (uint16_t)data;
        while(!FLASH_IsReady());

        address += 2;
        data>>=16;
        *(__IO uint16_t*)address = (uint16_t)data;
        while(!FLASH_IsReady());

        FLASH->CR &= ~(FLASH_CR_PG);

}


/*******************************************************************************
* Function Name  : FLASH_Read
* Description    : Read uint32_t var from FLASH
*******************************************************************************/
uint32_t FLASH_Read(uint32_t address)
{
	uint32_t tmp = 0;
	tmp = *(__IO uint32_t*) address;
    return tmp;
}


/*******************************************************************************
* Function Name  : FLASH_WritePage
* Description    : Write data to FLASH Page
*******************************************************************************/
void FLASH_WritePage(uint32_t st_address, uint16_t *mass, uint16_t len)
{
	//uint32_t st_address = FLASH_BASE + FLASH_PAGE * 1024;

	FLASH_Unlock();
	FLASH_ErasePage(st_address);
	FLASH_Lock();

	FLASH_Unlock();
	for(uint16_t ptr=0; ptr<256; ptr++)
	{
		if(ptr < len/2)
			FLASH_Write(st_address+ptr*4, mass[ptr*2]<<16 | mass[ptr*2+1]);
		else
			FLASH_Write(st_address+ptr*4, 0xFFFFFFFF);//4bytes per call, ptr in 1byte
	}

	FLASH_Lock();
}

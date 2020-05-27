/* Includes ------------------------------------------------------------------*/
#include "LIS3DSH.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define LIS3DSH_CS_LOW	GPIO_ResetBits(GPIOB, GPIO_Pin_15)
#define LIS3DSH_CS_HIGH	GPIO_SetBits(GPIOB, GPIO_Pin_15)

/* Private variables ---------------------------------------------------------*/
float LIS3DSH_Sensitivity;


/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  :
* Description    :
* Input		 :
*******************************************************************************/
/*******************************************************************************
* Function Name  : SPI_Send
* Description    :
*******************************************************************************/
uint8_t SPI_Write(uint8_t data)
{
	/* Fill output buffer with data */
	SPI1->DR = data;
	/* Wait for transmission to complete */
	while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
	/* Wait for received data to complete */
	while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
	/* Wait for SPI to be ready */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
	/* Return data from buffer */
	return SPI1->DR;
}


void SPI_WriteMulti(uint8_t* data, uint8_t count)
{
	for(uint8_t i=0; i<count; i++)
		SPI_Write(data[i]);
}


void SPI_ReadMulti(uint8_t* data, uint8_t dummy, uint8_t count)
{
	for(uint16_t i=0; i<count; i++)
		data[i] = SPI_Write(dummy);
}


void LIS3DSH_WriteSPI(uint8_t* data, uint8_t addr, uint8_t count)
{
	/* Start SPI transmission */
	LIS3DSH_CS_LOW;

	/* Send address */
	SPI_Write(addr);

	/* Send data */
	SPI_WriteMulti(data, count);

	/* Stop SPI transmission */
	LIS3DSH_CS_HIGH;
}


void LIS3DSH_ReadSPI(uint8_t* data, uint8_t addr, uint8_t count)
{
	/* Start SPI transmission */
	LIS3DSH_CS_LOW;

	/* Add read bit */
	addr |= 0x80;

	if (count > 1) {
		/* Add autoincrement bit */
		addr |= 0x40;
	}

	/* Send address */
	SPI_Write(addr);

	/* Receive data */
	SPI_ReadMulti(data, 0, count);

	/* Stop SPI transmission */
	LIS3DSH_CS_HIGH;
}


void LIS3DSH_Init()
{
	uint8_t temp;

	temp = 0x47;
	LIS3DSH_WriteSPI(&temp, LIS3DSH_CTRL_REG1_ADDR, 1);

	temp = 0x38;
	LIS3DSH_WriteSPI(&temp, LIS3DSH_CTRL_REG2_ADDR, 1);

	temp = 0x80;
	LIS3DSH_WriteSPI(&temp, LIS3DSH_CTRL_REG4_ADDR, 1);

}


void LIS3DSH_ReadAxes(int16_t *data)
{
	uint8_t buffer[6];

	LIS3DSH_ReadSPI((uint8_t*)&buffer[0], LIS3DSH_OUT_X_L_ADDR, 6);

	/* Set axes */
	data[0] = (int16_t)((buffer[1] << 8) + buffer[0]);
	data[1] = (int16_t)((buffer[3] << 8) + buffer[2]);
	data[2] = (int16_t)((buffer[5] << 8) + buffer[4]);
}

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LIS3DSH_H
#define LIS3DSH_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"

/* Exported types ------------------------------------------------------------*/
typedef enum {
	/* LIS3DSH */
	LIS3DSH_Sensitivity_2G,
	LIS3DSH_Sensitivity_4G,
	LIS3DSH_Sensitivity_6G,
	LIS3DSH_Sensitivity_8G,
	LIS3DSH_Sensitivity_16G,
	/* LIS302DL */
	Sensitivity_2_3G,
	Sensitivity_9_2G
} LIS3DSH_Sensitivity_t;


typedef enum {
	/* LIS3DSH */
	LIS3DSH_Filter_800Hz,
	LIS3DSH_Filter_400Hz,
	LIS3DSH_Filter_200Hz,
	LIS3DSH_Filter_50Hz
} LIS3DSH_Filter_t;


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define LIS3DSH_WHO_AM_I_ADDR				0x0F
#define LIS3DSH_CTRL_REG1_ADDR				0x20
#define LIS3DSH_CTRL_REG2_ADDR				0x21
#define LIS3DSH_CTRL_REG3_ADDR				0x22
#define LIS3DSH_CTRL_REG4_ADDR				0x23
#define LIS3DSH_CTRL_REG5_ADDR				0x24
#define LIS3DSH_CTRL_REG6_ADDR				0x25
#define LIS3DSH_OUT_X_L_ADDR				0x28
#define LIS3DSH_OUT_X_H_ADDR				0x29
#define LIS3DSH_OUT_Y_L_ADDR				0x2A
#define LIS3DSH_OUT_Y_H_ADDR				0x2B
#define LIS3DSH_OUT_Z_L_ADDR				0x0C
#define LIS3DSH_OUT_Z_H_ADDR				0x0D

#define LIS3DSH_SENSITIVITY_0_06G            0.06  /* 0.06 mg/digit*/
#define LIS3DSH_SENSITIVITY_0_12G            0.12  /* 0.12 mg/digit*/
#define LIS3DSH_SENSITIVITY_0_18G            0.18  /* 0.18 mg/digit*/
#define LIS3DSH_SENSITIVITY_0_24G            0.24  /* 0.24 mg/digit*/
#define LIS3DSH_SENSITIVITY_0_73G            0.73  /* 0.73 mg/digit*/

#define LIS3DSH_DATARATE_100				((uint8_t)0x60)

#define LIS3DSH_FULLSCALE_2					((uint8_t)0x00)  /* 2 g  */
#define LIS3DSH_FULLSCALE_4					((uint8_t)0x08)  /* 4 g  */
#define LIS3DSH_FULLSCALE_6					((uint8_t)0x10)  /* 6 g  */
#define LIS3DSH_FULLSCALE_8					((uint8_t)0x18)  /* 8 g  */
#define LIS3DSH_FULLSCALE_16				((uint8_t)0x20)  /* 16 g */
#define LIS3DSH__FULLSCALE_SELECTION		((uint8_t)0x38)

#define LIS3DSH_FILTER_BW_800				((uint8_t)0x00)  /* 800 Hz */
#define LIS3DSH_FILTER_BW_400				((uint8_t)0x40)//((uint8_t)0x08) /* 400 Hz  */
#define LIS3DSH_FILTER_BW_200				((uint8_t)0x80)//((uint8_t)0x10)  /* 200 Hz */
#define LIS3DSH_FILTER_BW_50				((uint8_t)(0x80 | 0x40))//((uint8_t)0x18)  /* 50 Hz  */
#define LIS3DSH_SELFTEST_NORMAL				((uint8_t)0x00)
#define LIS3DSH_XYZ_ENABLE					((uint8_t)0x07)
#define LIS3DSH_SERIALINTERFACE_4WIRE		((uint8_t)0x00)
#define LIS3DSH_SM_ENABLE					((uint8_t)0x01)
#define LIS3DSH_SM_DISABLE					((uint8_t)0x00)

/* ----------------------------------------- */
/* LIS302DL registers                        */
/* ----------------------------------------- */
#define CTRL_REG1_ADDR							0x20
#define CTRL_REG2_ADDR							0x21
#define CTRL_REG3_ADDR							0x22
#define OUT_X_ADDR								0x29
#define OUT_Y_ADDR								0x2B
#define OUT_Z_ADDR								0x2D

#define SENSITIVITY_2_3G						18  /* 18 mg/digit*/
#define SENSITIVITY_9_2G						72  /* 72 mg/digit*/

#define DATARATE_100							((uint8_t)0x00)
#define DATARATE_400							((uint8_t)0x80)

#define LOWPOWERMODE_ACTIVE					((uint8_t)0x40)
#define FULLSCALE_2_3							((uint8_t)0x00)
#define FULLSCALE_9_2							((uint8_t)0x20)
#define SELFTEST_NORMAL						((uint8_t)0x00)
#define XYZ_ENABLE								((uint8_t)0x07)
#define SERIALINTERFACE_4WIRE					((uint8_t)0x00)
#define BOOT_NORMALMODE						((uint8_t)0x00)
#define BOOT_REBOOTMEMORY						((uint8_t)0x40)
#define FILTEREDDATASELECTION_OUTPUTREGISTER	((uint8_t)0x20)
#define HIGHPASSFILTERINTERRUPT_OFF			((uint8_t)0x00)
#define HIGHPASSFILTERINTERRUPT_1				((uint8_t)0x04)
#define HIGHPASSFILTERINTERRUPT_2				((uint8_t)0x08)
#define HIGHPASSFILTERINTERRUPT_1_2			((uint8_t)0x0C)
#define HIGHPASSFILTER_LEVEL_0					((uint8_t)0x00)
#define HIGHPASSFILTER_LEVEL_1					((uint8_t)0x01)
#define HIGHPASSFILTER_LEVEL_2					((uint8_t)0x02)
#define HIGHPASSFILTER_LEVEL_3					((uint8_t)0x03)


/* Exported functions ------------------------------------------------------- */
uint8_t SPI_Write(uint8_t data);
void SPI_WriteMulti(uint8_t* data, uint8_t count);
void SPI_ReadMulti(uint8_t* data, uint8_t dummy, uint8_t count);
void LIS3DSH_WriteSPI(uint8_t* data, uint8_t addr, uint8_t count);
void LIS3DSH_ReadSPI(uint8_t* data, uint8_t addr, uint8_t count);
void LIS3DSH_Init();
void LIS3DSH_ReadAxes(int16_t *data);


#endif //LIS3DSH_H

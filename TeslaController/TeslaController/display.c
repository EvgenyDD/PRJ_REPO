/* Includes ------------------------------------------------------------------*/
#include "display.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_spi.h"
#include "stm32f30x_rcc.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define POINT_MASK 	(1<<7)
#define RST_DATA 	GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_10 | GPIO_Pin_11)

#define OE_EN 		GPIO_SetBits(GPIOB, GPIO_Pin_4);
#define OE_DIS 		GPIO_ResetBits(GPIOB, GPIO_Pin_4);


/* Private variables ---------------------------------------------------------*/
static uint8_t digits[12] = {0};
static uint16_t points = 0;

static const uint8_t fontArray[] = {
				//.GAFBCED
				0b00111111, //0
				0b00001100,
				0b01101011,
				0b01101101,
				0b01011100,
				0b01110101,
				0b01110111,
				0b00101100,
				0b01111111,
				0b01111101, //9
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0b01111110, //A
				0b01010111, //b
				0b00110011, //C
				0b01001111, //d
				0b01110011, //E
				0b01110010, //F
				0b00110111, //G
				0b01011110, //H
				0b00000100, //i
				0b00001101, //J
				0,
				0b00010011, //L
				0,
				0b01000110, //n
				0b01000111, //o
				0b01111010, //P
				0b01111100, //q
				0b01000010, //r
				0,
				0b01010011, //t
				0b00000111, //u
				0,
				0,
				0,
				0b01011101  //Y
};


/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Display_init
* Description    : Init display module
*******************************************************************************/
void Display_init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_6);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_6);

	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CRCPolynomial = 0;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_Init(SPI3, &SPI_InitStructure);

	SPI_NSSInternalSoftwareConfig(SPI3,SPI_NSSInternalSoft_Set);

	SPI_Cmd(SPI3, ENABLE);
}


/*******************************************************************************
* Function Name  : Display_setPoint
* Description    : Set point on and off to specified indicator
*******************************************************************************/
void Display_setPoint(uint8_t indicator, uint8_t digit, bool point)
{
	if(indicator == 0 && digit < 2)
		BitWrite(point, points, 1-digit);
	if(indicator == 1 && digit < 3)
		BitWrite(point, points, (2-digit)+2);
	if(indicator == 2 && digit < 3)
		BitWrite(point, points, (2-digit)+5);
	if(indicator == 3 && digit < 4)
		BitWrite(point, points, (3-digit)+8);
}


/*******************************************************************************
* Function Name  : Display_setNum
* Description    : Set number to indicator
*******************************************************************************/
void Display_setNum(uint8_t indicator, uint16_t num)
{
	if(indicator == 0)
	{
		digits[0] = num/10%10;
		digits[1] = num%10;
	}
	else if(indicator == 1)
	{
		digits[2] = num/100%10;
		digits[3] = num/10%10;
		digits[4] = num%10;
	}
	else if(indicator == 2)
	{
		digits[5] = num/100%10;
		digits[6] = num/10%10;
		digits[7] = num%10;
	}
	else if(indicator == 3)
	{
		digits[8] = num/1000%10;
		digits[9] = num/100%10;
		digits[10] = num/10%10;
		digits[11] = num%10;
	}
}


/*******************************************************************************
* Function Name  : Display_setText
* Description    : Set text to indicator
*******************************************************************************/
void Display_setText(uint8_t indicator, char *s)
{
	int i = 0;
	if(indicator == 0)
	{
		digits[0] = s[i++] - '0';
		digits[1] = s[i++] - '0';
	}
	else if(indicator == 1)
	{
		digits[2] = s[i++] - '0';
		digits[3] = s[i++] - '0';
		digits[4] = s[i++] - '0';
	}
	else if(indicator == 2)
	{
		digits[5] = s[i++] - '0';
		digits[6] = s[i++] - '0';
		digits[7] = s[i++] - '0';
	}
	else if(indicator == 3)
	{
		digits[8] = s[i++] - '0';
		digits[9] = s[i++] - '0';
		digits[10] = s[i++] - '0';
		digits[11] = s[i++] - '0';
	}
}


/*******************************************************************************
* Function Name  : Display_callback
* Description    : Redraw display
*******************************************************************************/
void Display_callback()
{
	uint8_t address = 0;
	static uint16_t phase = 0;
	if(++phase >= 12)
		phase = 0;

	//b2 b10 b11 b0
	RST_DATA;

	const uint8_t addrByte[8] = { 1 << 7, 1 << 6, 1 << 3, 1 << 4, 1 << 5, 1 << 2, 1 << 1, 1 << 0 };
	const uint16_t addrByteHW[4] = { GPIO_Pin_0, GPIO_Pin_2, GPIO_Pin_10, GPIO_Pin_11 };

	if(phase < 8)
		address = addrByte[phase];
	else
		GPIO_SetBits(GPIOB, addrByteHW[phase - 8]);

	address ^= 0xFF;

	//MSB - address
	//LSB - pixels
	OE_DIS;
	SPI_I2S_SendData16(SPI3, fontArray[digits[phase]] | ((points & (1 << phase)) ? POINT_MASK : 0) | (address << 8));
	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
	while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET);
	OE_EN;
}

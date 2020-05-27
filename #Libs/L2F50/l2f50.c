#include <stm32f4xx.h>
#include "stm32f4xx_spi.h"
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include "stm32f4xx_rng.h"
#include "l2f50.h"


unsigned int CHAR_POS;

void Delay_mS();

void L2F50_Cmd(unsigned char dat)
{
	LCD_RS_0;
	SPI_I2S_SendData(SPI1, dat);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, 0x00);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	LCD_RS_1;
}

void L2F50_Dat8(unsigned char dat)
{
	SPI_I2S_SendData(SPI1, dat);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, 0x00);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
}


void L2F50_Dat16(unsigned int dat)
{
	SPI_I2S_SendData(SPI1, dat>>8);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, dat);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
}


void L2F50_CSpulse(void)
{
	LCD_CS_1;
	Delay_mS(1);
	LCD_CS_0;
}

/* */
void L2F50_HW_Configure(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/* Configure GPIO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //GPIO_Mode_IN,GPIO_Mode_OUT,GPIO_Mode_AF,GPIO_Mode_AN
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//GPIO_Speed_2MHz Low speed,GPIO_Speed_25MHz, GPIO_Speed_50MHz, GPIO_Speed_100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//GPIO_OType_PP,GPIO_OType_OD
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//GPIO_PuPd_NOPULL, GPIO_PuPd_UP, GPIO_PuPd_DOWN
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/* */
void L2F50_Init(void)
{
	L2F50_HW_Configure();

	unsigned int i;
	static const int disctl[9] = {0x4C, 0x01, 0x53, 0x00, 0x02, 0xB4, 0xB0, 0x02, 0x00};
	static const int gcp64_0[29] = {0x11,0x27,0x3C,0x4C,0x5D,0x6C,0x78,0x84,0x90,0x99,0xA2,0xAA,0xB2,0xBA,
	0xC0,0xC7,0xCC,0xD2,0xD7,0xDC,0xE0,0xE4,0xE8,0xED,0xF0,0xF4,0xF7,0xFB,
	0xFE};
	static const int gcp64_1[34] = {0x01,0x03,0x06,0x09,0x0B,0x0E,0x10,0x13,0x15,0x17,0x19,0x1C,0x1E,0x20,
	0x22,0x24,0x26,0x28,0x2A,0x2C,0x2D,0x2F,0x31,0x33,0x35,0x37,0x39,0x3B,
	0x3D,0x3F,0x42,0x44,0x47,0x5E};
	static const int gcp16[15] = {0x13,0x23,0x2D,0x33,0x38,0x3C,0x40,0x43,0x46,0x48,0x4A,0x4C,0x4E,0x50,0x64};


	// generate clean display reset
	LCD_RST_0; // reset display
	LCD_CS_1; // CS is high during reset release
	LCD_RS_1; // RS is set to high
	Delay_mS(1);
	LCD_RST_1; // release reset
	Delay_mS(1);

	LCD_CS_0; // select display

	L2F50_Cmd(DATCTL);
	L2F50_Dat8(0x2A); // 0x2A=565 mode, 0x0A=666mode, 0x3A=444mode

	L2F50_CSpulse();

	L2F50_Cmd(DISCTL);
	for (i=0; i<9; i++)
	{
		L2F50_Dat8(disctl[i]);
	}

	L2F50_Cmd(GCP64);
	for (i=0; i<29; i++)
	{
		L2F50_Dat8(gcp64_0[i]);
		L2F50_Dat8(0x00);
	}
	for (i=0; i<34; i++)
	{
		L2F50_Dat8(gcp64_1[i]);
		L2F50_Dat8(0x01);
	}

	L2F50_Cmd(GCP16);
	for (i=0; i<15; i++)
	{
		L2F50_Dat8(gcp16[i]);
	}

	L2F50_Cmd(GSSET);
	L2F50_Dat8(0x00);

	L2F50_Cmd(OSSEL);
	L2F50_Dat8(0x00);

	L2F50_Cmd(SLPOUT);

	L2F50_Cmd(SD_CSET);
	L2F50_Dat8(0x08);
	L2F50_Dat8(0x01);
	L2F50_Dat8(0x8B);
	L2F50_Dat8(0x01);

	L2F50_Cmd(SD_PSET);
	L2F50_Dat8(0x00);
	L2F50_Dat8(0x8F);


	L2F50_Cmd(ASCSET);
	L2F50_Dat8(0x00);
	L2F50_Dat8(0xAF);
	L2F50_Dat8(0xAF);
	L2F50_Dat8(0x03);

	L2F50_Cmd(SCSTART);
	L2F50_Dat8(0x00);

	LCD_RS_0;
	L2F50_Dat8(DISON);

	LCD_CS_1; // deselect display
}

/* */
void L2F50_SetWindow (unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{
	L2F50_Cmd(SD_CSET);
	L2F50_Dat8(8+x1); // start is 8, not 0
	L2F50_Dat8(0x01);
	L2F50_Dat8(8+x2); // is 00x8B = 0x83+8
	L2F50_Dat8(0x01);

	L2F50_Cmd(SD_PSET);
	L2F50_Dat8(y1);
	L2F50_Dat8(y2);
}


int L2F50_abs(int x)
{
	if(x<0) return -x;
	else return x;
}

int L2F50_sign(int x)
{
	if(x<0) return -1;
	if(x>0) return 1;
	else return 0;
}


/*  DISPLAY GUI COMMANDS  */
void L2F50_Fill_Screen(unsigned int color) //	Fill the screen with one color
{
	unsigned int i;

	LCD_CS_0; // select display
	L2F50_SetWindow(0,0,131,175);

	L2F50_Cmd(RAMWR);
	for (i=0; i<DISP_W*DISP_H; i++)
	{
		//if(i%2 == 0)`
		//L2F50_Dat16((unsigned int)RNG_GetRandomNumber());
		L2F50_Dat16(color);
	}

	LCD_CS_1; // deselect display
}

void L2F50_Put_Pixel(unsigned int x, unsigned int y, int color) // 	Draw one pixel on the screen
{
	LCD_CS_0;  // select display
	L2F50_Cmd(SD_CSET);
	L2F50_Dat8(0x08+y);  // start is 8, not 0
	L2F50_Dat8(0x01);
	L2F50_Cmd(SD_PSET);
	L2F50_Dat8(x);
	L2F50_Cmd(RAMWR);
	L2F50_Dat16(color);
	LCD_CS_1; // deselect display
}

// draw graphic....
void L2F50_Draw_Line(unsigned int x1, unsigned int y1,
					unsigned int x2, unsigned int y2, unsigned int color)
{
	LCD_CS_0; // select display
	L2F50_SetWindow(0,0,131,175);
	LCD_CS_1; // deselect display

	int dX = L2F50_abs(x2 - x1);
	int dY = L2F50_abs(y2 - y1);
	int signX = L2F50_sign(x2 - x1);
	int signY = L2F50_sign(y2 - y1);
	int err = dY - dX;
	int err2;

	while(1)
	{
		L2F50_Put_Pixel(x1, y1, color);

		if(y1 == y2 && x1 == x2) break;
		err2 = err * 2;

		if(err2 > -dX)
		{
			err -= dX;
			y1 += signY;
		}
		if(err2 < dY)
		{
			err += dY;
			x1 += signX;
		}
	}
}


void L2F50_Draw_Rect(unsigned int x, unsigned int y,
					unsigned int width, unsigned int heigh, unsigned int color)
{
	L2F50_Draw_Line(x, y, x, y + heigh, color);
	L2F50_Draw_Line(x, y + heigh, x + width, y + heigh, color);
	L2F50_Draw_Line(x + width, y + heigh, x + width, y, color);
	L2F50_Draw_Line(x + width, y, x, y, color);
}


void L2F50_Draw_Rect_Fill(unsigned int x, unsigned int y,
					unsigned int width, unsigned int heigh, unsigned int contourColor, unsigned int fillColor)
{
	L2F50_Draw_Line(x, y, x, y + heigh, contourColor);
	L2F50_Draw_Line(x, y + heigh, x + width, y + heigh, contourColor);
	L2F50_Draw_Line(x + width, y + heigh, x + width, y, contourColor);
	L2F50_Draw_Line(x + width, y, x, y, contourColor);


	LCD_CS_0;  		// select display

	L2F50_SetWindow(y+1, x+1, y+heigh-1, x+width-1);

	L2F50_Cmd(RAMWR);
	unsigned int i;
	for (i = 0; i < (width - 1) * (heigh - 1); i++)
		L2F50_Dat16(fillColor);

	LCD_CS_1; 		// deselect display
}


void L2F50_Draw_Circle_Fill(unsigned int Xpos, unsigned int Ypos,
		unsigned int Radius, unsigned int contourColor, unsigned int fillColor)
{
	if(Xpos >= DISP_W) Xpos = DISP_W;
	if(Ypos >= DISP_W) Ypos = DISP_H;

	int32_t   D;	// Decision Variable
	uint32_t  CurX;	// Current X Value
	uint32_t  CurY;	// Current Y Value

	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY)
	{
		if(CurY > 0)
		{
			L2F50_Draw_Line(Xpos - CurX, Ypos + CurY, Xpos - CurX, Ypos + CurY - 2*CurY, fillColor);
			L2F50_Draw_Line(Xpos + CurX, Ypos + CurY, Xpos + CurX, Ypos + CurY - 2*CurY, fillColor);
		}
		if(CurX > 0)
		{
			L2F50_Draw_Line(Xpos - CurY, Ypos + CurX, Xpos - CurY, Ypos + CurX - 2*CurX, fillColor);
			L2F50_Draw_Line(Xpos + CurY, Ypos + CurX, Xpos + CurY, Ypos + CurX - 2*CurX, fillColor);
		}

		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}

	L2F50_Draw_Circle(Xpos, Ypos, Radius, contourColor);
}

void L2F50_Draw_Circle(unsigned int Xpos, unsigned int Ypos,
					unsigned int Radius, unsigned int color)
{
	if(Xpos >= DISP_W) Xpos = DISP_W;
	if(Ypos >= DISP_W) Ypos = DISP_H;

	int32_t   D;	// Decision Variable
	uint32_t  CurX;	// Current X Value
	uint32_t  CurY;	// Current Y Value

	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY)
	{
		L2F50_Put_Pixel(Xpos + CurX, Ypos + CurY, color);
		L2F50_Put_Pixel(Xpos + CurX, Ypos - CurY, color);
		L2F50_Put_Pixel(Xpos - CurX, Ypos + CurY, color);
		L2F50_Put_Pixel(Xpos - CurX, Ypos - CurY, color);

		L2F50_Put_Pixel(Xpos + CurY, Ypos + CurX, color);
		L2F50_Put_Pixel(Xpos + CurY, Ypos - CurX, color);
		L2F50_Put_Pixel(Xpos - CurY, Ypos + CurX, color);
		L2F50_Put_Pixel(Xpos - CurY, Ypos - CurX, color);

		if(D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}



//....and numbers with chars
void L2F50_Put_Char(unsigned int x, unsigned int y, char data, const char *font, unsigned int textColor, unsigned int fillColor)
{
	LCD_CS_0;
	unsigned int font_h = font[1];
	unsigned int font_w = font[0];
	unsigned int bytes8_h = 1;
	if(font_h % 8) bytes8_h += 1;
	unsigned int pointer = 3 + (data - 32) * (font_w);

	unsigned int col, row;
	unsigned int shift = 0;

	//pointer += 2;
	for (col = 0; col < font_w; col++)
	{
		L2F50_SetWindow(x + col, y, x + col, y + font_h);
		L2F50_Cmd(RAMWR);
		for(row = 0; row < font_h ; row++)
		{
			if( (font[pointer] >> shift) & 1 )
				L2F50_Dat16(textColor);
			else
				L2F50_Dat16(fillColor);

			shift++;

			if (shift >= 8)
			{
				shift = 0;
				pointer++;
			}
		}
		shift = 0;
		if(bytes8_h > 1) pointer += 1;
	}
	CHAR_POS = font_w + font[2];
	LCD_CS_1;
}


void L2F50_Put_String(unsigned int x,unsigned int y, char *s, const char *font, unsigned int textColor, unsigned int fillColor)
{
	unsigned int k = 0;
	while (*s)
	{
		L2F50_Put_Char(x+k, y, *s, font, textColor, fillColor);
		k += CHAR_POS;
		s++;
	}
}


void L2F50_Put_Num(unsigned int x, unsigned int y, long value,
					const char *font, unsigned int textColor, unsigned int fillColor)
{
	char *s= itoa(value);
	L2F50_Put_String(x, y, s, font, textColor, fillColor);
}


char* itoa(signed long value) {   	//не работает с отрицательными и дробными
	static char buf[32] = {0};
	int i = 30;

	for(; value && i ; --i, value /= 10)
		buf[i] = "0123456789abcdef"[value % 10];

	return &buf[i+1];
}

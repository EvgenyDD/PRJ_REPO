#include <stm32f4xx.h>
#include "stm32f4xx_spi.h"
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <misc.h>
#include <stm32f4xx_syscfg.h>
#include "stm32f4xx_rng.h"

#include "l2f50.h"
#include "font.h"


//Насколько понял SPI на 25Mhz (с учетом прескалера) экран тянет, скорость заливки - фантастика :))))

static __IO uint32_t TimingDelay;

void L2F50_Init(void);
void SPI1_Init (void);
void SysTick_Handler (void);
void Delay_mS(uint32_t nTime);

void SysTick_Handler (void)
{
	TimingDelay--;
}

void Delay_mS(uint32_t nTime)
{
	TimingDelay = nTime;
	while(TimingDelay);
}


int main(void)
{
	SystemInit();

	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	  /* RNG Peripheral enable */
RNG_Cmd(ENABLE);

	SysTick_Config(SystemCoreClock/10000);
	RCC_ClocksTypeDef RCC_ClockFreq;
	RCC_GetClocksFreq(&RCC_ClockFreq);
	SPI1_Init();

	L2F50_Init();
	L2F50_Fill_Screen(WHITE);

	//Настраиваем ногу (PA0) с кнопкой на вход
	GPIO_InitTypeDef PORT;
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);
	PORT.GPIO_Pin = GPIO_Pin_0;
	PORT.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOA, &PORT);

int old=0, new=0;
signed long v = 200;
char vb = '0';
goto x;
//double color[9]={BLACK, WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, ORANGE};
while(1)
{
	if ((GPIOA->IDR & 0x01)==0x01) {
		Delay_mS(800);
		new++;
		if(new == 9) new = 0;

	}

	if(new != old)
	{
		//L2F50_Fill_Screen(color[new]);
		old = new;

					/*
					unsigned int i;

					LCD_CS_0; // select display
					L2F50_SetWindow(10,10,100,100);

					unsigned int color = RED;
					L2F50_Cmd(RAMWR);
					for (i=0; i<DISP_W*DISP_H; i++)
					{
						//if(i%2 == 0)
					//L2F50_Dat16((unsigned int)RNG_GetRandomNumber());

						L2F50_Dat16((unsigned int)RNG_GetRandomNumber());
					}

					LCD_CS_1; // deselect display
					*/
		/*L2F50_Draw_Line(0,0,175,132, RED);
		L2F50_Draw_Line(0,132,175,0, RED);
		//L2F50_Draw_Rect_Fill(40,20,120,100,WHITE,RED);
		L2F50_Put_Pixel(5,0,RED);
		L2F50_Draw_Rect(20,10,180, 100,RED);
		L2F50_Draw_Line(65,130,220,5,BLUE);
		L2F50_Draw_Circle_Fill(50,60,30,BLUE,MAGENTA);
		L2F50_Draw_Circle_Fill(100,80,3,YELLOW,ORANGE);
		L2F50_Draw_Circle_Fill(100,100,1,YELLOW,RED);
		L2F50_Draw_Circle_Fill(85,100,2,YELLOW,RED);
		L2F50_Draw_Circle_Fill(120,70,12,RED,GREEN);
		//L2F50_Draw_Rect_Fill(50,60,30,30,WHITE,YELLOW);
		L2F50_Draw_Rect(10,10,150,110,GREEN);


		*/
		x:
		L2F50_Fill_Screen(BLACK);
		L2F50_Put_Char(16,100,'a', fontRus, RED, YELLOW);
		L2F50_Put_Char(26,100,'B', fontRus, RED, YELLOW);
		L2F50_Put_Char(36,100,'c', fontRus, RED, YELLOW);
		L2F50_Put_Char(46,100,'D', fontRus, RED, YELLOW);
		L2F50_Put_Char(56,100,'e', fontRus, RED, YELLOW);
		L2F50_Put_Char(66,120,'0', font16_26, RED, YELLOW);
		L2F50_Put_Char(76,140,'1', font16_26, RED, YELLOW);
		//L2F50_Put_Char(86,100,'H', font_PGM2, RED, YELLOW);
		//L2F50_Put_Char(96,100,'i', font_PGM2, RED, YELLOW);
		//L2F50_Put_Char(106,100,'9', font_PGM, RED, YELLOW);
		L2F50_Put_String(15,10,"HELLO, L2F50, ", fontRus, YELLOW, BLUE);


		//L2F50_Put_String(15,25,"azaza", font8_14, BLACK, YELLOW);

		char *s= itoa(v);

			L2F50_Put_String(40,35,s, fontRus, BLACK, YELLOW);

		v *= 4;

		while(1)
		{
			v++;
			vb++;
			//v *= -1;
			Delay_mS(500);
			L2F50_Put_Num(15,35,v, font4_6, YELLOW, BLUE);
			L2F50_Put_Num(20,50,v/5, font8_14, YELLOW, BLACK);
			L2F50_Put_Char(45,35,vb,font16_26,YELLOW, RED);
			L2F50_Put_Char(15,30,0x9B,fontRus,YELLOW,BLACK);
		}
/*
		while(1)
		{
			L2F50_Draw_Line((unsigned int)RNG_GetRandomNumber()%170, (unsigned int)RNG_GetRandomNumber()%130, (unsigned int)RNG_GetRandomNumber()%170,
					(unsigned int)RNG_GetRandomNumber()%130, (unsigned int)RNG_GetRandomNumber());
			L2F50_Draw_Circle_Fill((unsigned int)RNG_GetRandomNumber()%160+10, (unsigned int)RNG_GetRandomNumber()%110+10, 20,
								 (unsigned int)RNG_GetRandomNumber(), (unsigned int)RNG_GetRandomNumber()/2+0xFF);
			Delay_mS(20);
		}
*/
		//L2F50_Draw_Rect_Fill(50,10,1,100,RNG_GetRandomNumber());
		//L2F50_Draw_Line(20,40,120,100,RNG_GetRandomNumber());
	}
	//L2F50_Fill_Screen(1);
}
}




void SPI1_Init (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	/* Peripheral Clock Enable -------------------------------------------------*/
	/* Enable the SPI clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	/* Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* Assign SPI pins*/
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1); // SCK
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1); // MISO
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1); // MOSI

	/* Configure GPIO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //GPIO_Mode_IN,GPIO_Mode_OUT,GPIO_Mode_AF,GPIO_Mode_AN
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//GPIO_Speed_2MHz Low speed,GPIO_Speed_25MHz, GPIO_Speed_50MHz, GPIO_Speed_100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//GPIO_OType_PP,GPIO_OType_OD
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//GPIO_PuPd_NOPULL, GPIO_PuPd_UP, GPIO_PuPd_DOWN
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 |GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Disabling SPI1 for configure */
	SPI_I2S_DeInit(SPI1);

	/* SPI1 Configure */
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

	/* Starting SPI */
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
}

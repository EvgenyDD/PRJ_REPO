/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "string.h"

#include "display.h"

#include <stdbool.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t pointer = 0;
static uint8_t customPointer = 0;
static uint8_t customWritedChars[8];


/* Extern variables ----------------------------------------------------------*/
extern const char RusPage[171][8];
extern volatile uint16_t backlightTimer;


/* Private function prototypes -----------------------------------------------*/
#define Delay(x) do{ for(uint32_t i=0; i<(x)*10000; i++) asm("nop"); } while(0);



/*******************************************************************************
 * Function Name  : ClearBuffer
 * Description    : Clear buffer with value
 *******************************************************************************/
void ClearBuffer(uint8_t *s, uint16_t len, char value)
{
	for(uint16_t i = 0; i < len; i++)
		s[i] = value;
}



/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
 * Function Name  : DisplayInit
 * Description    : Initialize display
 *******************************************************************************/
void DisplayInit()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD,
		ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13
		| GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_ResetBits(D_WR_PORT, D_WR);
	GPIO_ResetBits(D_EN_PORT, D_EN);

	// KS0066u defines
	// execution times in ns
#define LCD_TIME_US_SET_MODE       (39)
#define LCD_TIME_US_SET_DATA       (43)
#define LCD_TIME_US_CLEAR_DISPLAY  (1530)
#define LCD_TIME_US_CURSOR_HOME    (1530)
#define LCD_TIME_US_ENABLE         (1)
#define LCD_TIME_MS_RESET          (1)
#define LCD_TIME_MS_STARTUP        (30)

	/* Init sequence */
	Delay(35);

	// Set 8 bit, 1 line and 5x7 character mode
	DisplayWriteCom(LCD_SET_FUNCTION | LCD_FUNCTION_8BIT | LCD_FUNCTION_2LINE | LCD_FUNCTION_5X8);
	Delay(40);

	// Enable display, no cursor or blinking
	DisplayWriteCom(LCD_SET_DISPLAY | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINKING_OFF);
	Delay(40);

	// Clear display
	DisplayWriteCom(LCD_CLEAR_DISPLAY);
	Delay(2);

	// Increase pointer and no display shift
	DisplayWriteCom(LCD_SET_ENTRY | LCD_ENTRY_INCREASE | LCD_ENTRY_NOSHIFT);
	Delay(25);	//sleep_us(LCD_TIME_US_SET_MODE);

	DisplayWriteCom(LCD_CURSOR_HOME);
}


/*******************************************************************************
 * Function Name  : DisplayClear
 * Description    : Clear Display and reset pointers to custom data
 *******************************************************************************/
void DisplayClear()
{
	DisplayWriteCom(LCD_CLEAR_DISPLAY);
	Delay(2);
	pointer = customPointer = 0;
	ClearBuffer(customWritedChars, 8, 0);
}


/*******************************************************************************
 * Function Name  : DisplayWriteData
 * Description    : Write data to display
 *******************************************************************************/
void DisplayWriteData(uint8_t data)
{
	D_DATA_OUT(data);
	D_RS_DATA;
	D_EN_HIGH;
	for(uint8_t i = 0; i < 200; i++)
		asm("nop");
	D_EN_LOW;
	for(uint8_t i = 0; i < 200; i++)
		asm("nop");
}


/*******************************************************************************
 * Function Name  : DisplayWriteCom
 * Description    : Write command to display
 *******************************************************************************/
void DisplayWriteCom(uint8_t data)
{
	D_DATA_OUT(data);
	D_RS_COM;
	D_EN_HIGH;
	for(uint8_t i = 0; i < 200; i++)
		asm("nop");
	D_EN_LOW;
	for(uint8_t i = 0; i < 200; i++)
		asm("nop");
}


/*******************************************************************************
 * Function Name  : DisplayChar
 * Description    : Display one character on display and increment cursor
 *******************************************************************************/
void DisplayChar(uint8_t t)
{
	DisplaySetCursor(pointer);

	if(t < 128 && t != '\\')
		DisplayWriteData(t);
	else
		/* The char is not in the Display ROM */
		DisplayWriteCustomChar(t);

	if(++pointer > 9)
	{
		DisplayWriteCom(LCD_CURSOR_HOME);
		pointer = 0;
	}
}


/*******************************************************************************
 * Function Name  : DisplayWriteCustomChar
 * Description    : Write specific char to display CGRAM and display it
 *******************************************************************************/
void DisplayWriteCustomChar(uint8_t t)
{
	const uint8_t customChars[11][8] =
	{
	{ 0, 4, 2, 31, 2, 4, 0, 0 }, //Arr right = 0xf0
		{ 0, 4, 8, 31, 8, 4, 0, 0 }, //Arr left
		{ 4, 14, 21, 4, 4, 0, 0, 0 }, //arr up
		{ 0, 0, 0, 4, 4, 21, 14, 4 }, //arr down
		{ 8, 20, 20, 8, 5, 6, 5, 5 }, //ok
		{ 14, 17, 17, 31, 27, 27, 31, 0 }, //lock close
		{ 14, 16, 16, 31, 27, 27, 31, 0 }, //lock open
		{ 17, 18, 23, 9, 19, 4, 7, 0 }, //1/2
		{ 31, 17, 31, 4, 4, 6, 7, 4 }, //lock sign
		{ 24, 24, 24, 24, 24, 24, 24, 0 }, //one line sign 		//0xf9
		{ 27, 27, 27, 27, 27, 27, 27, 0 } }; //two lines sign 	//0xfA

	char s = IsCharInEngPage(t);

	if(s)
		DisplayWriteData(s);
	else
	{
		/* If symbols is already in memory - print it */
		if(customPointer)
			for(uint8_t i = 0; i < 8; i++)
				if(customWritedChars[i] == t)
				{
					DisplayWriteData(i);
					return;
				}

		/* Print strange symbol if no memory left for custom characters */
		if(customPointer >= 8)
		{
			DisplayWriteData('\\'); //
			return;
		}

		/* Write char to CGRAM */
		DisplayWriteCom(LCD_SET_CGADR + customPointer * 8);

		if(t < 0xF0)
			for(uint8_t i = 0; i < 8; i++)
			{
				DisplayWriteData(RusPage[t - (t > 223 ? 69 : 21)][i] & 0x1F);
			}
		else
			for(uint8_t i = 0; i < 8; i++)
			{
				DisplayWriteData(customChars[t - 0xF0][i] & 0x1F);
			}

		customWritedChars[customPointer] = t;

		/* ..and display it */
		DisplaySetCursor(pointer);
		DisplayWriteData(customPointer);

		customPointer++;
	}
}


/*******************************************************************************
 * Function Name  : IsCharInEngPage
 * Description    : Some chars can be replaced by English characters
 *******************************************************************************/
char IsCharInEngPage(uint8_t t)
{
	char q = 0;

	switch(t)
	{
	case '\x80'/*À*/:
		q = 'A';
		break;
	case '\x82'/*Â*/:
		q = 'B';
		break;
	case '\x85'/*Å*/:
		q = 'E';
		break;
	case '\x8a'/*Ê*/:
		q = 'K';
		break;
	case '\x8c'/*Ì*/:
		q = 'M';
		break;
	case '\x8d'/*Í*/:
		q = 'H';
		break;
	case '\x8e'/*Î*/:
		q = 'O';
		break;
	case '\x90'/*Ð*/:
		q = 'P';
		break;
	case '\x91'/*Ñ*/:
		q = 'C';
		break;
	case '\x92'/*Ò*/:
		q = 'T';
		break;
	case '\x95'/*Õ*/:
		q = 'X';
		break;

	case '\xa0'/*à*/:
		q = 'a';
		break;
	case '\xa5'/*å*/:
		q = 'e';
		break;
	case '\xae'/*î*/:
		q = 'o';
		break;
	case '\xe0'/*ð*/:
		q = 'p';
		break;
	case '\xe1'/*ñ*/:
		q = 'c';
		break;
	case '\xe5'/*x*/:
		q = 'x';
		break;
	}

	return q;
}


/*******************************************************************************
 * Function Name  : DisplaySetCursor
 * Description    : Set cursor to specific position (0 is the first, 9 is the last)
 *******************************************************************************/
void DisplaySetCursor(uint8_t position)
{
	DisplayWriteCom(LCD_SET_DDADR + (position > 4 ? (0x40 + position - 5) : (position)));
}


/*******************************************************************************
 * Function Name  : DisplayCursorBlink
 * Description    : Set blinking cursor at position
 *******************************************************************************/
void DisplayCursorBlink(uint8_t pos)
{
	DisplaySetCursor(pos);
	DisplayWriteCom(0x0F);
}


/*******************************************************************************
 * Function Name  : DisplayString
 * Description    : Display whole string on display (max 10 symbols)
 *******************************************************************************/
void DisplayString(uint8_t * s)
{
	if(DisplayCheckIfTheSameContent(s) == true)
		return;

	DisplayClear();

	for(uint8_t i = 0; i < 10 && *s != '\0' && *s != '\n'; i++)
	{
		DisplayChar(*s++);

		/* Overflow */
		if(pointer == 0 && *s != '\0' && *s != '\n')
		{
			DisplaySetCursor(9);
			//DisplayWriteData(0x7E); //Arrow
			return;
		}
	}
}


/*******************************************************************************
 * Function Name  : DisplayStringRelative
 * Description    : Display whole string on display NOT from beginning (max 10 symbols)
 *******************************************************************************/
void DisplayStringRelative(uint8_t * s, uint8_t pos)
{
	if(DisplayCheckIfTheSameContent(s) == true)
		return;

	//DisplayClear();
	pointer = pos;

	for(uint8_t i = 0; i < 10 && *s != '\0' && *s != '\n'; i++)
	{
		DisplayChar(*s);
		s++;

		/* Overflow */
		if(pointer == 0 && *s != '\0' && *s != '\n')
		{
			DisplaySetCursor(9);
			DisplayWriteData(0x7E); //Arrow
			return;
		}
	}
}


/*******************************************************************************
 * Function Name  : DisplayStringRelative
 * Description    : Display whole string on the center of display
 *******************************************************************************/
void DisplayStringCentered(uint8_t * s)
{
	if(DisplayCheckIfTheSameContent(s) == true)
		return;

	DisplayClear();

	uint8_t len = strlen(s);

	if(len < 10)
		pointer = (10 - len) / 2;

	for(uint8_t i = 0; i < 10 && *s != '\0' && *s != '\n'; i++)
	{
		DisplayChar(*s++);

		/* Overflow */
		if(pointer == 0 && *s != '\0' && *s != '\n')
		{
			DisplaySetCursor(9);
			DisplayWriteData(0x7E); //Arrow
			return;
		}
	}
}


/*******************************************************************************
 * Function Name  : DisplayNum
 * Description    : Display Number
 *******************************************************************************/
void DisplayNum(int32_t num)
{
	uint8_t d[25];
	itoa_(num, d);
	DisplayString(d);
}


/*******************************************************************************
 * Function Name  : DisplayNum
 * Description    : Display Number at specific position (0 is first)
 *******************************************************************************/
void DisplayNumAtPos(int32_t num, uint8_t pos)
{
	if(pos > 9)
		return;

	uint8_t d[25];
	itoa_(num, d);
	DisplayStringRelative(d, pos);
}


/*******************************************************************************
 * Function Name  : DisplayNumWDesc
 * Description    : Display String + Number
 *******************************************************************************/
void DisplayNumWDesc(uint8_t *s, int32_t num)
{
	uint8_t o[strlen(s) + 15], d[25];
	o[0] = '\0';
	strcat_(o, s);
	itoa_(num, d);
	strcat_(o, d);
	DisplayString(o);
}


/*******************************************************************************
 * Function Name  : DisplayNumWDesc
 * Description    : Turn on backlight
 *******************************************************************************/
void DisplayBacklightOn()
{
	GPIO_SetBits(GPIOA, GPIO_Pin_0);
	backlightTimer = 0;
}


/*******************************************************************************
 * Function Name  : DisplayBacklightOff
 * Description    : Turn off backlight
 *******************************************************************************/
void DisplayBacklightOff()
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);
}


/*******************************************************************************
 * Function Name  : DIsplayBacklightSetState
 * Description    : Set Backlight state to ON/OFF
 *******************************************************************************/
void DisplayBacklightSetState(bool state)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_0, state);
}


/*******************************************************************************
 * Function Name  : DisplayCheckIfTheSameContent
 * Description    : return true if the previous contents are equal
 * 				   to current display content
 *******************************************************************************/
uint8_t DisplayCheckIfTheSameContent(uint8_t* s)
{
	bool equal = true;
	static char prev[10];

	for(uint8_t i = 0; i < 10; i++)
		if(prev[i] != s[i])
			equal = false;

	if(equal == false)
		for(uint8_t i = 0; i < 10; i++)
			prev[i] = s[i];

	return equal;
}


/*******************************************************************************
 * Function Name  : DisplayCursorOn
 * Description    : Turn on flashing cursor
 *******************************************************************************/
void DisplayCursorOn()
{
	DisplayWriteCom(0x0D);
}


/*******************************************************************************
 * Function Name  : DisplayCursorOff
 * Description    : Turn off flashing cursor
 *******************************************************************************/
void DisplayCursorOff()
{
	DisplayWriteCom(0x0C);
}

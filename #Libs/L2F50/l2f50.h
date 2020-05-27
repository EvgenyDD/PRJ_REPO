#ifndef L2F50_H
#define L2F50_H



//*  GPIO Operations  *//
#define LCD_CS_1 GPIO_SetBits(GPIOA, GPIO_Pin_3)
#define LCD_CS_0 GPIO_ResetBits(GPIOA, GPIO_Pin_3)
#define LCD_RS_1 GPIO_SetBits(GPIOA, GPIO_Pin_1)
#define LCD_RS_0 GPIO_ResetBits(GPIOA, GPIO_Pin_1)
#define LCD_RST_1 GPIO_SetBits(GPIOC, GPIO_Pin_3)
#define LCD_RST_0 GPIO_ResetBits(GPIOC, GPIO_Pin_3)
#define LCD_LED_1 GPIO_SetBits(GPIOC, GPIO_Pin_1)
#define LCD_LED_0 GPIO_ResetBits(GPIOC, GPIO_Pin_1)


#define DISP_W 176
#define DISP_H 132

//*  Display Registers  *//
#define DATCTL 0xBC // Data Control (data handling in RAM)
#define DISCTL 0xCA // Display Control

#define GCP64 0xCB // pulse set for 64 gray scale
#define GCP16 0xCC // pulse set for 16 gray scale
#define GSSET 0xCD // set for gray scales

#define OSSEL 0xD0 // Oscillator select
#define ASCSET 0xAA // area scroll setting
#define SCSTART 0xAB // scroll start setting

#define DISON 0xAF // Display ON (no parameter)
#define DISOFF 0xAE // Display OFF (no parameter)
#define DISINV 0xA7 // Display Invert (no parameter)
#define DISNOR 0xA6 // Display Normal (no parameter)

#define SLPIN 0x95 // Display Sleep (no parameter)
#define SLPOUT 0x94 // Display out of sleep (no parameter)
#define RAMWR 0x5C // Display Memory write
#define PTLIN 0xA8 // partial screen write

#define SD_CSET 0x15 // column address setting
#define SD_PSET 0x75 // page address setting


//*  Colors  *//
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define ORANGE 0xEAAA

//*  Display Functions  *//
void L2F50_Cmd(unsigned char);
void L2F50_Dat8(unsigned char);
void L2F50_Dat16(unsigned int);
void L2F50_CSpulse(void);
void L2F50_HW_Configure (void);
void L2F50_Init(void);
void L2F50_SetWindow (unsigned int,unsigned int,unsigned int,unsigned int);

void L2F50_Fill_Screen(unsigned int color);
void L2F50_Put_Pixel(unsigned int, unsigned int, int);
void L2F50_Put_Char(unsigned int, unsigned int, char, const char*, unsigned int, unsigned int);
void L2F50_Put_String(unsigned int,unsigned int,char*,const char*, unsigned int textColor, unsigned int fillColor);
void L2F50_Put_Num(unsigned int,unsigned int,long,const char*, unsigned int, unsigned int);

char* itoa(signed long);

void L2F50_Draw_Line(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
void L2F50_Draw_Rect(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
void L2F50_Draw_Rect_Fill(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
void L2F50_Draw_Circle(unsigned int, unsigned int, unsigned int, unsigned int);
void L2F50_Draw_Circle_Fill(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);


#endif

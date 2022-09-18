#ifndef LCD_LIB_H
#define LCD_LIB_H

#include <ioavr.h>
#include <intrinsics.h>

//порт к которому подключена шина данных ЖКД
#define PORT_DATA PORTA
#define PIN_DATA  PINA
#define DDRX_DATA DDRA

//порт к которому подключены управляющие выводы ЖКД
#define PORT_SIG PORTA
#define PIN_SIG  PINA
#define DDRX_SIG DDRA

//Номера выводов к которым подключены управляющие выводы ЖКД 
#define RS 0
#define RW 2
#define EN 1

#define F_CPU 20000000

#define CHECK_FLAG_BF
#define BUS_4BIT
//******************************************************************************
//макросы




//прототипы функций
void LCD_Init(void);//инициализация портов и жкд
void LCD_WriteData(unsigned char data); //выводит байт данных на жкд
void LCD_WriteCom(unsigned char data); //посылает команду жкд
void LCD_SendStringFlash(unsigned char __flash *str);
void LCD_SendString(char *str);
void BCD_2IntLcd(unsigned int value);
void BCD_4IntLcd(unsigned int value);
void LCD_Goto(char x, char y);

void LCD_SetInitDisplay();
void ds1307_GetALL();
void LCD_DataSent();
void EpromCopy();
void Buttons();
void SetAwakePWM(unsigned int, unsigned int);
void TimeComp();
#endif
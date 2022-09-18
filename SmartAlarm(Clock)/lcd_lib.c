#include "lcd_lib.h"

//макросы для работы с битами
#define ClearBit(reg, bit)       reg &= (~(1<<(bit)))
#define SetBit(reg, bit)         reg |= (1<<(bit))	
#define FLAG_BF 7

//макросы для задержек
#define _delay_us(us) 	__delay_cycles((F_CPU / 1000000) * (us));
#define _delay_ms(ms) 	__delay_cycles((F_CPU / 1000) * (ms));

//общая функция
#pragma inline = forced
void LCD_CommonFunc(unsigned char data)
{
#ifdef BUS_4BIT  
  unsigned char tmp; 
  tmp  = PORT_DATA & 0x0f;
  tmp |= (data & 0xf0);

  PORT_DATA = tmp;		//вывод старшей тетрады 
  SetBit(PORT_SIG, EN);	        
  _delay_us(2);
  ClearBit(PORT_SIG, EN);	

  data = __swap_nibbles(data);
  tmp  = PORT_DATA & 0x0f;
  tmp |= (data & 0xf0);
 
  PORT_DATA = tmp;		//вывод младшей тетрады 
  SetBit(PORT_SIG, EN);	        
  _delay_us(2);
  ClearBit(PORT_SIG, EN);	 
#else 
  PORT_DATA = data;		//вывод данных на шину индикатора 
  SetBit(PORT_SIG, EN);	        //установка E в 1
  _delay_us(2);
  ClearBit(PORT_SIG, EN);	//установка E в 0 - записывающий фронт
#endif
}

//функция ожидания готовности lcd
#pragma inline = forced
void LCD_Wait(void)
{
#ifdef CHECK_FLAG_BF
  #ifdef BUS_4BIT
  
  unsigned char data;
  DDRX_DATA &= 0x0f;            //конфигурируем порт на вход
  PORT_DATA |= 0xf0;	        //включаем pull-up резисторы
  SetBit(PORT_SIG, RW);         //RW в 1 чтение из lcd
  ClearBit(PORT_SIG, RS);	//RS в 0 команды
  do{
    SetBit(PORT_SIG, EN);	
    _delay_us(2);
    data = PIN_DATA & 0xf0;      //чтение данных с порта
    ClearBit(PORT_SIG, EN);
    data = __swap_nibbles(data);
    SetBit(PORT_SIG, EN);	
    _delay_us(2);
    data |= PIN_DATA & 0xf0;      //чтение данных с порта
    ClearBit(PORT_SIG, EN);
    data = __swap_nibbles(data);
  }while((data & (1<<FLAG_BF))!= 0 );
  ClearBit(PORT_SIG, RW);
  DDRX_DATA |= 0xf0; 
  
  #else
  unsigned char data;
  DDRX_DATA = 0;                //конфигурируем порт на вход
  PORT_DATA = 0xff;	        //включаем pull-up резисторы
  SetBit(PORT_SIG, RW);         //RW в 1 чтение из lcd
  ClearBit(PORT_SIG, RS);	//RS в 0 команды
  do{
    SetBit(PORT_SIG, EN);	
    _delay_us(2);
    data = PIN_DATA;            //чтение данных с порта
    ClearBit(PORT_SIG, EN);	
  }while((data & (1<<FLAG_BF))!= 0 );
  ClearBit(PORT_SIG, RW);
  DDRX_DATA = 0xff; 
  #endif    
#else
  _delay_us(40);
#endif  
}

//функция записи команды 
void LCD_WriteCom(unsigned char data)
{
  LCD_Wait();
  ClearBit(PORT_SIG, RS);	//установка RS в 0 - команды
  LCD_CommonFunc(data);
}

//функция записи данных
void LCD_WriteData(unsigned char data)
{
  LCD_Wait();
  SetBit(PORT_SIG, RS);	        //установка RS в 1 - данные
  LCD_CommonFunc(data);
}

//функция инициализации
void LCD_Init(void)
{
#ifdef BUS_4BIT
  DDRX_DATA |= 0xf0;
  PORT_DATA |= 0xf0; 
  DDRX_SIG |= (1<<RW)|(1<<RS)|(1<<EN);
  PORT_SIG |= (1<<RW)|(1<<RS)|(1<<EN);
  ClearBit(PORT_SIG, RW);
  _delay_ms(40);
  LCD_WriteCom(0x28); //4-ех разрядная шина, 2 строки
#else
  DDRX_DATA |= 0xff;
  PORT_DATA |= 0xff;
  DDRX_SIG |= (1<<RW)|(1<<RS)|(1<<EN);
  PORT_SIG |= (1<<RW)|(1<<RS)|(1<<EN);
  ClearBit(PORT_SIG, RW);
  _delay_ms(40);
  LCD_WriteCom(0x38); //0b00111000 - 8 разрядная шина, 2 строки
#endif
  LCD_WriteCom(0xC);  //0b00001100 - дисплей, не курсор, мерцание выключены
  LCD_WriteCom(0x1);  //0b00000001 - очистка дисплея
  _delay_ms(2);
  LCD_WriteCom(0x6);  //0b00000110 - курсор движется вправо, сдвига нет
}

//функция вывода строки из флэш памяти
void LCD_SendStringFlash(unsigned char __flash *str)
{
  unsigned char data;			
  while (*str)
  {
    data = *str++;
    LCD_WriteData(data);
  }
}

//функция вывда строки из ОЗУ
void LCD_SendString(char *str)
{
  unsigned char data;
  while (*str)
  {
    data = *str++;
    LCD_WriteData(data);
  }
}

void BCD_2IntLcd(unsigned int value)
{
  static unsigned char buf[5];
  
  buf[0] = (unsigned char)((value % 10) + 48);
  value = value/10;
  buf[1] = (unsigned char)((value % 10) + 48);
 // value = value/10;
 /* buf[2] = (unsigned char)((value % 10) + 48);
  value = value/10;
  buf[3] = (unsigned char)((value % 10) + 48);
  value = value/10; 
  buf[4] = (unsigned char)(value + 48);*/
 
  LCD_WriteData(buf[1]);
  LCD_WriteData(buf[0]); 
}

void BCD_4IntLcd(unsigned int value)
{
  static unsigned char buf[5];
  
  buf[0] = (unsigned char)((value % 10) + 48);
  value = value/10;
  buf[1] = (unsigned char)((value % 10) + 48);
  value = value/10;
  buf[2] = (unsigned char)((value % 10) + 48);
  value = value/10;
  buf[3] = (unsigned char)((value % 10) + 48);
  /*value = value/10; 
  buf[4] = (unsigned char)(value + 48);*/
 
  LCD_WriteData(buf[3]); 
  LCD_WriteData(buf[2]); 
  LCD_WriteData(buf[1]);
  LCD_WriteData(buf[0]); 
}

/*позиционирование курсора*/
void LCD_Goto(char x, char y) {
  if(y < 2) LCD_WriteCom(((((y)& 1)*0x40)+((x)))|128); 
  if (y==2) LCD_WriteCom((((((y)-1)& 1)*0x14)+((x)))|128); 
  if (y==3) LCD_WriteCom((((((y)-2)& 1)*0x54)+((x)))|128); 
}
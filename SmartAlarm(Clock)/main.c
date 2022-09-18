#include <ioavr.h>
#include <stdio.h>
#include "lcd_lib.h"
#include "i2c.h"
#include "ds1307.h"

static unsigned char flag1Hz, modeSet=0, flagAwake[2]={0,0}; 
   __flash unsigned char aa[] = "abc - 3 string";
   __flash unsigned char bb[] = "cde - 4 string";
   //
   __eeprom __no_init unsigned char EEPROM[8];//={20,20,10,50,20,20,1,0};
   
typedef struct{
  unsigned char second, minute, hour;
  unsigned char date, month, day;
  unsigned int year;
}time;
time rtc; 

typedef struct{
  unsigned char minute, hour, onOff, len;
}alarming;
alarming alarm[2];

unsigned char bell[1]={237}; unsigned char null[1]={238};
__flash unsigned char JAN[] = "ЗЅіapД";  __flash unsigned char FEB[] = "дeіpa»Д"; //month of year
__flash unsigned char MAR[] = "јapї";    __flash unsigned char APR[] = "aѕpe»Д";
__flash unsigned char MAY[] = "јa№";     __flash unsigned char JUN[] = "ёЖЅД";
__flash unsigned char JUL[] = "ёЖ»Д";    __flash unsigned char AUG[] = "aіґycї";
__flash unsigned char SEP[] = "ceЅїЗІpД";__flash unsigned char OCT[] = "oєїЗІpД";
__flash unsigned char NOV[] = "ЅoЗІpД";  __flash unsigned char DEC[] = "гeєaІpД";
  unsigned char Date[4]; //Формат: Date[0]=день недели, Date[1]=Дата, Date[2]=Число, Date[3]=Год
  unsigned char Time[3]; //Формат: Time[0]=ss, Time[1]=mm, Time[2]=hh
  
  __flash unsigned char bright[18]={1,1,2,2,3,3,4,5,6,7,8,9,10,20,100,250, 255};

  
//
//*****************************************************************************1
void main(void)
{
  EpromCopy();
  __delay_cycles(400000);
  PORTD_Bit2=1;
  DDRB_Bit0=1; //LED
  DDRD_Bit7=1; //pwm
  
  PORTB_Bit1=1; PORTB_Bit5=1; PORTD_Bit1=1;
  ds1307_Init();
  LCD_Init(); 
  LCD_SetInitDisplay();
  OCR2=0;
  
  __enable_interrupt();
/*
  Date[0]=3;  //Пятница
  Date[1]=6; //13-е
  Date[2]=3;  //Апреля
  Date[3]=13; //2012
  
  Time[0]=16;
  Time[1]=42;
  Time[2]=15;   //12:11:10
  
  ds1307_SetTime(Time); //Устанавливаем время
  ds1307_SetDate(Date); //Устанавливаем дату
  */
  
 
 /*
  LCD_Goto(5,1);
  LCD_SendStringFlash(textBus);
  LCD_Goto(0,0);
  LCD_SendStringFlash(textBF);  
*/

while(1) 
{
  LCD_DataSent();
  Buttons();
  TimeComp();
  
}
}
//*****************************************************************************9


void LCD_SetInitDisplay()  //setting initial "char" data
{
  LCD_Goto(8,0);
  LCD_SendString(":");
  LCD_Goto(11,0);
  LCD_SendString(":");
  LCD_Goto(0,2);
  LCD_SendString(" yг1:");
  LCD_Goto(0,3);
  LCD_SendString(" yг2:");  
  LCD_Goto(15,2);
  LCD_SendString("јёЅ");
  LCD_Goto(15,3);
  LCD_SendString("јёЅ");
  LCD_Goto(8,2);
  LCD_SendString(":");
  LCD_Goto(8,3);
  LCD_SendString(":");
  
  OCR2=255;
  
  //разрешаем прерывания
  GICR |= (1<<INT0)|(1<<INT1)|(1<<INT2);
  //условия срабатывания
  MCUCR |= (1<<ISC01)|(0<<ISC00)|(1<<ISC11)|(0<<ISC10);
  
  TCCR2 |= (1<<COM21)|(0<<WGM21)|(1<<WGM20)|(1<<CS21)|(0<<CS22);
}

void LCD_DataSent() //send numeric data
{
 if(flag1Hz | modeSet)
 {
   ds1307_GetALL();
  LCD_Goto(6,0);
    BCD_2IntLcd(rtc.hour);
  LCD_Goto(9,0);
    BCD_2IntLcd(rtc.minute);
  LCD_Goto(12,0);
    BCD_2IntLcd(rtc.second);
  
  LCD_Goto(2,1);
    BCD_2IntLcd(rtc.date);
  LCD_Goto(5,1);
  switch(rtc.month){
    case(1):
      LCD_SendStringFlash(JAN);
      break;
    case(2):
      LCD_SendStringFlash(FEB);
      break;
    case(3):
      LCD_SendStringFlash(MAR);
      break;
    case(4):
      LCD_SendStringFlash(APR);
      break;
    case(5):
      LCD_SendStringFlash(MAY);
      break;
    case(6):
      LCD_SendStringFlash(JUN);
      break;
    case(7):
      LCD_SendStringFlash(JUL);
      break;
    case(8):
      LCD_SendStringFlash(AUG);
      break;
    case(9):
      LCD_SendStringFlash(SEP);
      break;
    case(10):
      LCD_SendStringFlash(OCT);
      break;
    case(11):
      LCD_SendStringFlash(NOV);
      break;
    case(12):
      LCD_SendStringFlash(DEC);
      break;       
  }
  
  LCD_Goto(12,1);
    BCD_4IntLcd(rtc.year); 
  LCD_Goto(6,2);
    BCD_2IntLcd(alarm[0].hour); 
  LCD_Goto(6,3);
    BCD_2IntLcd(alarm[1].hour);
  LCD_Goto(9,2);
    BCD_2IntLcd(alarm[0].minute); 
  LCD_Goto(9,3);
    BCD_2IntLcd(alarm[1].minute);
  LCD_Goto(13,2);
    BCD_2IntLcd(alarm[0].len); 
  LCD_Goto(13,3);
    BCD_2IntLcd(alarm[1].len);
  
  LCD_Goto(17,0);
  switch(rtc.day){
    case(1):
      LCD_SendString("ЁoЅ");
      break;
    case(2):
      LCD_SendString("Bїp");
      break;
    case(3):
      LCD_SendString("Cpг");
      break;
    case(4):
      LCD_SendString("«їі");
      break;
    case(5):
      LCD_SendString("ЁїЅ");
      break;
    case(6):
      LCD_SendString("CyІ");
      break;
    case(7):
      LCD_SendString("Bcє");
      break;
  }

  LCD_Goto(19,2);  
    if(alarm[0].onOff) LCD_WriteData(*bell);
    else LCD_WriteData(*null);
  LCD_Goto(19,3);
    if(alarm[1].onOff) LCD_WriteData(*bell);
    else LCD_WriteData(*null);
    
    LCD_Goto(0,0); 
    BCD_2IntLcd(modeSet);
    
   

  flag1Hz=0;
 }
}

void ds1307_GetALL()  //pick up data from RTC IC
{
  ds1307_GetTime(Time); //чтение времени
  ds1307_GetDate(Date); //чтение даты
  rtc.hour=Time[2]; rtc.minute=Time[1]; rtc.second=Time[0];
  rtc.date=Date[1]; rtc.month=Date[2]; rtc.year =2000+Date[3];
  rtc.day=Date[0];
}

void EpromCopy()
{
  alarm[0].hour=EEPROM[0];
  alarm[1].hour=EEPROM[1];
  alarm[0].minute=EEPROM[2];
  alarm[1].minute=EEPROM[3];
  alarm[0].len=EEPROM[4];
  alarm[1].len=EEPROM[5];
  alarm[0].onOff=EEPROM[6];
  alarm[1].onOff=EEPROM[7];
}



#pragma vector = INT0_vect
__interrupt void ds1307_Int()
{
  flag1Hz=1;  
  PORTB_Bit0 = ~PORTB_Bit0;
  
}



void Buttons()
{
  if(PINB_Bit1==0){
    __delay_cycles(4000000);
    modeSet++;
    if(modeSet==13) modeSet=0;
  }
  
  if(PINB_Bit5==0)
  {
    //__delay_cycles(4000000);
    if(!modeSet){
      if(alarm[0].onOff==0) {alarm[0].onOff=1; EEPROM[6]=1;}
      else                  {alarm[0].onOff=0; EEPROM[6]=0;}
      flagAwake[0]=0; OCR2=0;
      __delay_cycles(14000000);
    }
    if(modeSet){
      switch(modeSet){
        case(1): Time[2]--; ds1307_SetTime(Time); break; //hour
        case(2): Time[1]--; ds1307_SetTime(Time); break; //minute
        
        case(3): Date[1]--; ds1307_SetDate(Time); break; //number of month
        case(4): Date[2]--; ds1307_SetDate(Time); break; //month
        case(5): Date[3]--; ds1307_SetDate(Time); break; //year
        case(6): Date[0]--; ds1307_SetDate(Time); break; //week day
        
        case(7): alarm[0].hour--; EEPROM[0]--; break;
        case(8): alarm[0].minute--; EEPROM[2]--; break;
        case(9): alarm[0].len--; EEPROM[4]--; break;
        case(10): alarm[1].hour--; EEPROM[1]--; break;
        case(11): alarm[1].minute--; EEPROM[3]--; break;
        case(12): alarm[1].len--; EEPROM[5]--; break;
      }
      __delay_cycles(5000000);
    }
  }
  
  if(PIND_Bit1==0)
  {
    //__delay_cycles(4000000);
    if(!modeSet){
      if(alarm[1].onOff==0) {alarm[1].onOff=1; EEPROM[7]=1;}
      else                  {alarm[1].onOff=0; EEPROM[7]=0;}
      flagAwake[1]=0; OCR2=0;
      __delay_cycles(14000000);
    }
    if(modeSet){
      switch(modeSet){
        case(1): Time[2]++; ds1307_SetTime(Time); break; //hour
        case(2): Time[1]++; ds1307_SetTime(Time); break; //minute
        
        case(3): Date[1]++; ds1307_SetDate(Time); break; //number of month
        case(4): Date[2]++; ds1307_SetDate(Time); break; //month
        case(5): Date[3]++; ds1307_SetDate(Time); break; //year
        case(6): Date[0]++; ds1307_SetDate(Time); break; //week day
        
        case(7): alarm[0].hour++; EEPROM[0]++; break;
        case(8): alarm[0].minute++; EEPROM[2]++; break;
        case(9): alarm[0].len++; EEPROM[4]++; break;
        case(10): alarm[1].hour++; EEPROM[1]++; break;
        case(11): alarm[1].minute++; EEPROM[3]++; break;
        case(12): alarm[1].len++; EEPROM[5]++; break;
      }
      __delay_cycles(5000000);
    }
  }
}


void TimeComp()
{
  __disable_interrupt();
  static unsigned char bufer[2]={0,0};
  
  static unsigned long int time[3];
  time[0] = 60*rtc.hour + rtc.minute;
  time[1] = 60*alarm[0].hour + alarm[0].minute - alarm[0].len-1;
  time[2] = 60*alarm[1].hour + alarm[1].minute - alarm[1].len-1;
  
  if(((time[0]>time[1]) & ((time[0]-time[1])<=alarm[0].len))& alarm[0].onOff) {bufer[0]=(unsigned char)(time[0]-time[1]);flagAwake[0]=1;}
  else bufer[0]=0;
  if(((time[0]>time[2]) & ((time[0]-time[2])<=alarm[1].len))& alarm[1].onOff) {bufer[1]=(unsigned char)(time[0]-time[2]);flagAwake[1]=1;}
  else bufer[1]=0;

  
  SetAwakePWM(bufer[0], bufer[1]);
  __enable_interrupt();
}

void SetAwakePWM(unsigned int remain1, unsigned int remain2)
{
  if(remain1){
    OCR2 = bright[(unsigned char)((remain1 * 16) / alarm[0].len)];
  }
  else{
    if(flagAwake[0]==1) OCR2=255;
  }
    
  if(remain2){
    OCR2 = bright[(unsigned char)((remain2 * 16) / alarm[1].len)];
  }
  else{
    if(flagAwake[1]==1) OCR2=255;
  }
}
  

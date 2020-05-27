#include "HT1621.h"

#define FREQ 20000000UL
#define delay_ms(ms) __delay_cycles((FREQ/1000)*(ms))
/////////////////////////////////////////////////////////////////////////////////
void main() 
{ 
  DDRA = (1 << PB0) | (1 << PB2) | (1 << PB4); 	
  PORTA = (0 << PB0) | (0 << PB2) | (0 << PB4);
  SendCmd(BIAS); //setup bias and working period 
  SendCmd(SYSEN); //start system oscillator 
  SendCmd(LCDON); //switch on LCD bias generator 
  SendCmd(TONE2K);

do{  
  for(int p=0; p<1000; p++){
    int y=p;
    y=p/100;
   WriteDigit_1621(1,y,1);
    y=p%100;
    y=y/10;
   WriteDigit_1621(2,y,0); 
    y=p%10;
   WriteDigit_1621(3,y,1); 
   WriteDigit_1621(4,0,0);
    delay_ms(80);
      if(p%10==0){ SendCmd(TONEON); delay_ms(1);}
      else SendCmd(TONEOFF);
   }

  Clear_1621();
  PORTC_Bit7=1; 
  delay_ms(700);
  SendCmd(TONEON);
  PORTC_Bit7=0;
  delay_ms(50);
  SendCmd(TONEOFF);
}while(1);
}
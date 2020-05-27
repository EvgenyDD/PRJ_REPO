/*
 * LCDS.c
 *
 * Created: 20.01.2016 16:03:18
 * Author: Evgeny
 */

#include <io.h>
#include <delay.h>
#include <stdlib.h>
#define F_CPU 8000000UL


volatile unsigned char value = 0; 
int i;   
int numFlash = 0;       
int interval = 0;    
volatile unsigned int curPotValue;
       
void MeasureBattery()
{
    ADMUX = 1;  
    delay_ms(1);  
    
    ADCSRA |= (1<<ADSC);          
    while (ADCSRA & (1<<ADSC))
    
    value = ADCL;
}

unsigned char MeasurePot()
{
    ADMUX = 2;        
    delay_ms(1);  

    ADCSRA |= (1<<ADSC);          
    while (ADCSRA & (1<<ADSC))
    
    return ADCL;
}

void main( void )
{
        // Main Clock source: Calibrated Internal 8 MHz Osc.
    CCP=0xd8;
    CLKMSR=(0<<CLKMS1) | (0<<CLKMS0);
    // Clock Prescaler division factor: 1
    CCP=0xd8;
    CLKPSR=(0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0);
    // internal_clk = 8 MHz 
    
    
    // pulse period 20mS, lenght 1 - 1.5(middle) - 2mS 
    // 8 /64 timer counter prescaler /187 timer value = 668.44Hz (1.49mS)
    
    //WGM3:0 = 0101 -> Fast PWM 8 bit
    TCCR0A = (1<<COM0A1) | (0<<COM0A0) | (0<<WGM01) | (1<<WGM00);  
    TCCR0B = (0<<WGM03) | (1<<WGM02) | (1<<CS01) | (0<<CS00); 
    
     // Enable ADC, presc 1:8 for 125kHz ADC-clock
    ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);   
    ADCSRB = 0;
    DIDR0 = (1<<ADC2D) | (1<<ADC1D);     
   
    
    DDRB = 1<<PORTB0;           //timer out
    //PUEB = 1<<PUEB1;     
   
    //delay_ms(200);   
                     
    for(i=0; i<40; i++) MeasureBattery();   
                                           
    
    if(value<= 208)                  numFlash = 1; //<3.55V
    else if(value>208 && value<=214) numFlash = 2; //3.55-3.65V
    else if(value>214 && value<=220) numFlash = 3; //3.65-3.75V
    else if(value>220 && value<=226) numFlash = 4; //3.75-3.85V
    else if(value>226 && value<=232) numFlash = 5; //3.85-3.95V
    else if(value>232 && value<=238) numFlash = 6; //3.95-4.05V
    else if(value>238 && value<=244) numFlash = 7; //4.05-4.15V
    else if(value>244)               numFlash = 8; //>4.15V 
                        
    
    for(i=0; i<numFlash; i++)
    {           
        OCR0A = 30;
        delay_ms(330);   
        OCR0A = 2; 
        delay_ms(330);
    }
                  
    for(;;)
    {          
        delay_ms(50);
        for(i=0; i<40; i++) MeasurePot();  
        curPotValue = 0;    
        for(i=0; i<16; i++) curPotValue += MeasurePot(); 
        OCR0A = curPotValue>>4;
               
        if(++interval == (15*20))
        {              
            delay_ms(10); 
            for(i=0; i<80; i++) MeasureBattery();    
            
            if(value < 200 && value > 80)
            {
                for(i=0; i<3; i++)
                {           
                    OCR0A = curPotValue>10?curPotValue:20;
                    delay_ms(330);   
                    OCR0A = 2; 
                    delay_ms(330);
                }     
            } 
            
            //OCR0A = value;
            //delay_ms(1000);
            interval = 0;
        }
    }                    
}


//
//// Timer 0 overflow interrupt service routine
//interrupt [TIM0_OVF] void timer0_ovf_isr()
//{
//    static unsigned char int_count = 0;     //to make the servo signal
//    // 1mS =180, 1.5mS=143, 2mS=105, 2.1=100
//    
//    if(int_count++ >= 5)
//    {
//        int_count = 0;  
//        if(timer !=0) 
//        {  
//            magic = 130;
//        }
//        else 
//        {              
//            if(makePress == 1)
//                magic = 10;
//            else
//                magic = 130;
//        }
//        OCR0A = magic;       
//    }
//    else
//    {
//        OCR0A = 255;            //low level all period (inverse compare out)
//    } 
//     
//    if(timer) timer--;
//} 
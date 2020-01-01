/*
 * Box.c
 *
 * Created: 01.09.2015 23:03:18
 * Author: Evgeny
 */

#include <io.h>
#include <delay.h>
#include <stdlib.h>
#define F_CPU 8000000UL


//PB1 - INT0 - BTN
//PB2 - T0   - Servo

unsigned long timer = 0;
unsigned char magic = 130;
unsigned char lastState = 1;
unsigned int y;
char makePress = 0;

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
    
    //WGM3:0 = 0101 -> Fast PWM 10 bit
    TCCR0A = 1<<COM0A1 | 1<<COM0A0 | 1<<WGM00;  
    TCCR0B = 1<<WGM02 | 1<<CS01 | 1<<CS00;
    OCR0A  = 187;
    TIMSK0 = 1<<TOIE0;          //TOIE0 counter overflow interrupt      
    
    DDRB = 1<<PORTB0;           //timer out
    PUEB = 1<<PUEB1;     
   
    #asm("sei");  
    delay_ms(200);
                  
    for(;;)
    {      
        delay_ms(10);
        //button handler
        if(!(PINB & (1<<1)))   //0
        {            
            if(lastState == 1)
            {
                lastState = 0;    
                if(rand()%3 <2)      
                {
                    y =(rand()%3)*1000;
                    y+=(rand()%10)*100;
                    y+=(rand()%10)*10;
                    timer = 200+(long)y;
                }
                else
                    timer = 2;  
                makePress = 1;
            }  
            else
            {
                
            }  
        }    
        else  //1
        {
            if(lastState == 0)
            {
                lastState = 1;
            }  
            else
            {
                if(timer == 0 && makePress ==1)
                    makePress = 0;
            } 
        }
    }                    
}

// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr()
{
    static unsigned char int_count = 0;     //to make the servo signal
    // 1mS =180, 1.5mS=143, 2mS=105, 2.1=100
    
    if(int_count++ >= 5)
    {
        int_count = 0;  
        if(timer !=0) 
        {  
            magic = 130;
        }
        else 
        {              
            if(makePress == 1)
                magic = 10;
            else
                magic = 130;
        }
        OCR0A = magic;       
    }
    else
    {
        OCR0A = 255;            //low level all period (inverse compare out)
    } 
     
    if(timer) timer--;
} 
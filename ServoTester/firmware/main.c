#include "iotiny13.h"
#include <intrinsics.h>

//****************************************
//     This is simple ServoTester
//     with a button, a LED and a Pot
//        
//          3 MODE work:
//      1. The pot changes the servo position
//         from 1000uS to 2000uS
//         (LED is constantly light)
//      2. Center servo position
//         1500uS
//         (LED is fast flashing)
//      3. Sweep mode
//         the speed of sweep is controlled by the pot
//         (LED is slow flashing)
//          
//     Made by E.Dolgalev in 10.2013
//****************************************

// fuse default (0x6a)


unsigned char mode = 3;         //servotester mode
unsigned char direction = 1;    //sweep direction
unsigned char OCR_val = 143;    //OCR current value

//******************************************************************************
#pragma vector=TIM0_OVF_vect
__interrupt void my_interrupt_handler(void)
{ 

    static unsigned char int_count = 0;     //to make the servo signal
    static unsigned char i = 0;             //for led flashing
    // 1mS =180, 1.5mS=143, 2mS=105, 2.1=100
    
    if(int_count++ >= 5)
    {
      
        int_count = 0;
          
        switch(mode)
        {
            
        case 0: //position by potentiometer
                //light LED
          
            OCR0A = 180 - ADCH * 5/17;          
            PORTB |= (1<<PB4);
            
            break;
            
        //***********************
        case 1: //middle position
                //fast flashing LED        
            
            OCR0A = 143;       
            if(i++ > 1)
            {
              PORTB ^= (1<<PB4);
              i=0;
            }
            
            break;
            
        //***********************
        case 2: //sweep (freq by potentiometer)
                //slow flashing LED
            
            if(direction)
            {         //turn CCW
                  OCR_val += (ADCH/25 + 1);
                  
                  if(OCR_val > 180)
                  {
                      direction = 0;
                      OCR_val = 180;
                  }
            }
            else
            {         //turn CW
                  OCR_val -= (ADCH/25 + 1);
                  
                  if(OCR_val < 105)
                  {
                      direction = 1;
                      OCR_val = 105;
                  }
            }
            
            OCR0A = OCR_val;
            
            if(i++ > 30) 
            {
              PORTB ^= (1<<PB4);
              i=0;
            }
            
            break;
            
        }
        
    }
    else
    {
        
        OCR0A = 255;            //low level all period (inverse compare out)
        ADCSRA |= 1<<ADSC;      //ADC single conversion start
    
    }
}

int main()
{
    // internal_clk = 9.6 MHz
    // pulse period 20mS, lenght 1 - 1.5(middle) - 2mS 
    // default divider = 8 (9.6/8=1.2)
    // 9.6 /2 master clock prescaler=4.8 /64 timer counter prescaler /150 timer value = 500Hz (2mS)
   
    CLKPR = 1<<CLKPCE;        // divider change enable
    CLKPR = 1;                // 0=divider - 1, 1=divider 2 (page 26 manual PDF)
    
    //WGM2:0 = 3 -> Fast PWM 0xFF TOP MAX
    TCCR0A = 1<<COM0A1 | 1<<COM0A0 | 1<<COM0B1 | 1<<COM0B0 | 1<<WGM01 | 1<<WGM00;  
    OCR0A  = 105;
    //OCR0B  = 105;                //for test purpose only
    TIMSK0 = 1<<TOIE0;          //TOIE0 counter overflow interrupt
    TCCR0B = 3;                 //3=timer prescaler 64, 1=1
      
    ADMUX   = 1<<ADLAR | 1<<MUX0; //ADC init REF_VCC(ADMUX=0), left_just(ADLAR=1), chan ADC1=PB2(MUX1..0=01)
    ADCSRA  = 1<<ADEN | 1<<ADPS2 | 1<<ADPS1;//ADC on (ADEN=1), 4.8/64(prescaler)=75kHz(ADPS2:0=110)
    DIDR0   = 1<<ADC1D;           //ADC digital input on PB2 disable
    ADCSRA |= 1<<ADSC;          //start single conversion
                                //ADC only ADCH read 
    
    DDRB |=  1<<PB0 | 1<<PB4;    //timers out ON  
    PORTB |= 1<<PB1;    
    SREG |=  1<<7;               //interrupt enable
    
    
    for(;;)
    {
        //button handler
        if(!(PINB & (1<<PB1))) 
        {
            if(mode++ >= 2)
                mode = 0;
            
            __delay_cycles(4000000); //debounce
            
            if(mode == 2)
                OCR0A = 143;
        }
    }                    
}
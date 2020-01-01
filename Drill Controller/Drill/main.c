#include "all.h"
//#include "PID.h"

//#include <string.h>
//#include <stdio.h>

#include "PID.h"


volatile uint16_t sliderValue; 
uint16_t currentSpeed=0;
uint8_t failFlag=0;

extern signed int DGain;
extern signed int PGain;
/*******************************************************************************
* Function Name  : Init
* Description    : Initialization
*******************************************************************************/
void Init()
{
//GPIO
    DDRA = 0xF0;
    DDRB = 0x04;
    
	PORTA = 0x08;
    PORTB = 0x03;
    
//ADC
    ADMUX = (1<<MUX1);//(1<<REFS0)|
    ADCSRA = (1<<ADEN)|(0<<ADSC)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
    ADCSRB = (1<<ADLAR);
    DIDR0 = 0;

//Timer
    TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(0<<COM1A0)|(0<<COM1B0)|(0<<WGM11)|(1<<WGM10);
    TCCR1B = (0<<WGM13)|(1<<WGM12)|(1<<CS12)|(0<<CS11)|(0<<CS10); 

	TCCR0B = (1<<CS02)|(1<<CS01)|(1<<CS00);
    
//WatchDog
   // WDTCSR = (1<<WDE)|(1<<WDP2);
       
}

static uint8_t failCounter=0;
		static uint16_t counter=0;	
		static uint16_t counter2=0; 

/*******************************************************************************
* Function Name  : main
* Description    : ADC Interrupt Handler
*******************************************************************************/
int main()
{
    Init();
    DriveReset(); 
	DriveRotate(0);  

	sei();	//Enable interrupts
    ADCSRA |= (1<<ADSC); //start ADC
    
    while(1)
    {
		wdt_reset();


//#define PID_ALG
/***************************************************************************/
#ifdef PID_ALG
		

		//PORTA ^= (1<<7);

		/*pwm -	hz   - 	per 6.2 hz value = 162*x/1000
		10		10.4 	1.68
		12		40		6.48
		15		70		11.34
		20		110		17.82
		30		208		33.6
		50		308
		100		400		64.8
		150		430
		250		430		69.66
		max  	454		73.54
		*/

		if(++counter >= 166) //1000/6 = 
		{
			counter=0;

			/* 6.2Hz */
			currentSpeed = TCNT0*3+TCNT0/2; //3.5
			TCNT0 = 0;	
		}

/* sliderValue = 10 min
   				= 255 max

32 - slider

				15 = 90hz
				40 = סממעג 232 דצ = 32slider
				
   after norm: currentSpeed = [3 257]; 					
*/
		

	
//	currentSpeed = 30;
		DriveRotate(DrivePID(currentSpeed, sliderValue));
		
/*		if(++counter2 >= 10) //1000/counter2 = freqPID
		{
			counter2=0;
			//DriveRotate(DrivePID(currentSpeed, sliderValue));
		} 
*/
		/* SCAN FREQ = 1 kHz */
		_delay_loop_2(1800);//3900-500   1920 - 1000

#else
		if(BitIsReset(PINB, 1))
		{
			DriveRotate(sliderValue);  
			PORTA |= (1<<7);
		}
		else if(BitIsReset(PINB, 0))
		{
			DriveRotate(sliderValue/2);
		}
		else
		{
			DriveRotate(0);

			static uint16_t counter0=0;
			if(++counter0 >= 300)
			{
				counter0 = 0;
				PORTA ^= (1<<7);
			}
		}
		
		_delay_loop_2(1800);//3900-500    1000-1920
#endif	  
		ADCSRA |= (1<<ADSC);
		
/****************************************************************************/

    }    
}


/*******************************************************************************
* Function Name  : ADC_vect
* Description    : ADC Interrupt Handler
*******************************************************************************/
ISR(ADC_vect)
{
    static volatile unsigned int temp;
    static volatile unsigned char i;
    
    temp += ADCH;
    
	/*sliderValue = (uint8_t)(ADCH << 8);
    sliderValue = ADCL;*/

    if(++i >= 64)
    {
        i = 0;
        sliderValue = (uint16_t)(temp >> 6);
        temp = 0;
    }   
}

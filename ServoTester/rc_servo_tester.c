#define ENABLE_BIT_DEFINITIONS
#include "iotiny13.h"
//SERVOTESTER V 1.0
// 1 PB5 (PCINT5/RESET/ADC0/dW) 
// 2 PB3 (PCINT3/CLKI/ADC3)            button_sweep  - sweep on trigger
// 3 PB4 (PCINT4/ADC2)                 button_middle - pulse width 1.5mS
// 4 gnd
// 5 PB0 (MOSI/AIN0/OC0A/PCINT0)       pulse_out
// 6 PB1 (MISO/AIN1/OC0B/INT0/PCINT1)
// 7 PB2 (SCK/ADC1/T0/PCINT2)          r_var (2k2, gnd, vcc) - pulse width or sweep frequency
// 8 vcc
// fuse default (0x6a)
#define MIDDLE 4
#define SWEEP 3
#define RESET 5

#define WIDTH_MODE   0
#define SWEEP_MODE   2

#define DEB_LIMIT 5

#pragma vector=TIM0_OVF_vect
__interrupt void my_interrupt_handler(void)
{ 
static unsigned char int_count=0, button_debounce=0, sweep_state=WIDTH_MODE;
static char sweep_direct=0;
static int sweep_count;


static unsigned char impulse = 255-150, tmp; 
// 1mS =180, 1.5mS=143, 2mS=105

  if(int_count++ >=5)
  {
    OCR0A = impulse;
    int_count = 0;
    impulse=ADCH;//only high 8 bits (left justify mode)

    if(!(PINB & (1<<SWEEP)))
    {
      if(button_debounce == DEB_LIMIT)
      {
        if(sweep_state == WIDTH_MODE)
        {
          sweep_state = SWEEP_MODE;
          sweep_count = 0;
        }
        else
          sweep_state = WIDTH_MODE;
        
        button_debounce = 0;
      }
    }
    else if(button_debounce < DEB_LIMIT)
      button_debounce++;
        
  
    if(!(PINB & (1<<MIDDLE)))
      impulse = 255-150/2-150/4; //1.5mS
    else
    {
      if(sweep_state == WIDTH_MODE)
      {
        impulse >>=1;
        tmp = impulse>>1;
        impulse -= tmp;
        tmp >>=2;
        impulse += tmp;
        impulse += 105-1;
      }
      else
      {
        if(sweep_direct == 0)
        {
          sweep_count += ((int)impulse)<<3;
          if(sweep_count > 0x3fff)
          {
            sweep_count = 0x3fff;
            sweep_direct = 1;
          }
        }
        else
       {
          sweep_count -= ((int)impulse)<<3;
          if(sweep_count < 0)
          {
            sweep_count = 0;
            sweep_direct = 0;
          }
        }
        impulse = sweep_count>>6;
        impulse >>=1;
        tmp = impulse>>1;
        impulse -= tmp;
        tmp >>=2;
        impulse += tmp;
        impulse += 105-1;
      }
    }
    
  }
  else
  {
    OCR0A = 255; //low level all period (inverse compare out)
    ADCSRA |= 1<<ADSC;//ADC single conversion start
  }
}

int main()
{
 // internal_clk = 9.6 MHz
// pulse period 20mS, lenght 1 - 1.5(middle) - 2mS 
  // default divider = 8 (9.6/8=1.2)
  // 9.6 /2 master clock prescaler=4.8 /64 timer counter prescaler /150 timer value = 500Hz (2mS)
  CLKPR = 1<<CLKPCE;// divider change enable
  CLKPR = 1;// 0=divider - 1, 1=divider 2 (page 26 manual PDF)

  
  TCCR0A = 1<<COM0A1 | 1<<COM0A0 | 1<<COM0B1 | 1<<COM0B0 | 1<<WGM01 | 1<<WGM00;//WGM2:0 = 3 -> Fast PWM 0xFF TOP MAX  
  OCR0A = 105;
  OCR0B = 105;//for test purpose only
  TIMSK0 = 1<<TOIE0;//TOIE0 counter overflow interrupt
  TCCR0B = 3;//3=timer prescaler 64, 1=1
    
  ADMUX = 1<<ADLAR | 1<<MUX0;//ADC init REF_VCC(ADMUX=0), left_just(ADLAR=1), chan ADC1=PB2(MUX1..0=01)
  ADCSRA = 1<<ADEN | 1<<ADPS2 | 1<<ADPS1;//ADC on (ADEN=1), 4.8/64(prescaler)=75kHz(ADPS2:0=110)
  DIDR0 = 1<<ADC1D;//ADC digital input on PB2 disable
  ADCSRA |= 1<<ADSC;//start single conversion
  //ADC only ADCH read 

 DDRB |=  1<<PB0 | 1<<PB1;//timers out ON  
  PORTB |= 1<<MIDDLE | 1<<SWEEP | 1<<RESET;//pullup enable on button and reset

 SREG |=  1<<7; //interrupt enable


 for(;;)
 {
 }
}
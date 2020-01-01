#include <stdlib.h>
#include <ioavr.h>
#include <intrinsics.h>

#define FREQ 8000000UL
#define _delay_us(us)          __delay_cycles((FREQ/1000000)*(us))
#define _delay_ms(ms)          __delay_cycles((FREQ/1000)*(ms))

#define DEBOUNCE 1
#define FLASH 10

#define BitIsSet(reg, bit)        ((reg & (1<<bit)) != 0)
#define BitIsClear(reg, bit)      ((reg & (1<<bit)) == 0)
#define ClearBit(reg, bit)        reg &= (~(1<<(bit))) 
#define SetBit(reg, bit)          reg |= (1<<(bit))  

unsigned char dispMode = 3;
unsigned int debounce = 0, fail=0;
volatile unsigned int adc=0; 
unsigned char colorL[6]={0,0,0,0,0,0};  
unsigned char colorR[6]={0,0,0,0,0,0};  
unsigned char smoothVar=0;
unsigned char flash=FLASH;

void Smooth( unsigned char load );
static void HSVtoRGB( unsigned char *r, unsigned char *g, unsigned char *b, unsigned int i_hue );
/*
OCR1B

D4  D3  D2  D0
G   R   B   Y
C5  C4  C3  C2

*/

#define GREEN   (1<<PC5)
#define RED     (1<<PC4)
#define BLUE    (1<<PC3)
#define YELLOW  (1<<PC2)

volatile unsigned char waitTimer=0;     //таймер паузы
volatile unsigned char smoothTimer=0;    //таймер времени плавного перехода RGB
volatile unsigned char outR[4], outG[4], outB[4];          //текущий цвет
unsigned char targetR[4], targetG[4], targetB[4];          //целевой цвет
unsigned char reachR = 0,  reachG = 0,  reachB = 0;        //флаги равности текущего и целевого цвета
unsigned char speedR = 0,  speedG = 0,  speedB = 0;        //++ или -- чтоб достичь целевого цвета
unsigned char R = 0, G = 0, B = 60;


void Leds(void)
{
    PORTC &= ~(GREEN|RED|BLUE|YELLOW);
    switch(dispMode)
    {
    case 0: 
        PORTC |= GREEN|RED|BLUE|YELLOW;
        break;
    case 1:
        PORTC |= GREEN;
        break;
    case 2:             //police
        PORTC |= RED;
        break;  
    case 14:            //strob
        PORTC |= RED;
        PORTC |= GREEN;
        break;                 
    case 3:             //smooth change
        PORTC |= BLUE;
        break; 
    case 13:            //smooth change + blank
        PORTC |= BLUE|YELLOW;
        break;
        
    case 4:         //1color mode
        PORTC |= RED;
        PORTC |= YELLOW;
        break; 
    case 5:
        PORTC |= GREEN;
        PORTC |= YELLOW;
        break;  
    case 6:
        PORTC |= BLUE;
        PORTC |= YELLOW;
        break; 
        
    case 7:         //2color mode
        PORTC |= RED;
        PORTC |= GREEN;
        PORTC |= YELLOW;
        break; 
    case 8:
        PORTC |= GREEN;
        PORTC |= BLUE;
        PORTC |= YELLOW;
        break;  
    case 9:
        PORTC |= BLUE;
        PORTC |= RED;
        PORTC |= YELLOW;
        break; 
        
    case 10:
        PORTC |= BLUE;
        PORTC |= RED;
        PORTC |= GREEN;
        PORTC |= YELLOW;
        break;  
    
    } 
}


void Buttons(void)
{
    if(dispMode == 2 && !PIND_Bit3) {while(!PIND_Bit3);dispMode = 1; debounce = DEBOUNCE; return;}
    if(dispMode == 1 && !PIND_Bit3) {while(!PIND_Bit3);dispMode = 14; debounce = DEBOUNCE; return;}
    if(dispMode == 1 && !PIND_Bit4) {while(!PIND_Bit4);dispMode = 11; debounce = DEBOUNCE; return;}
    if(dispMode == 3 && !PIND_Bit2) {while(!PIND_Bit2);dispMode = 13;waitTimer=0;
        smoothVar=0;
        smoothTimer=0; debounce = DEBOUNCE; return;}
    
    
    if(dispMode == 9 && !PIND_Bit0) {while(!PIND_Bit0);dispMode = 10; debounce = DEBOUNCE; return;}
    if(dispMode == 8 && !PIND_Bit0) {while(!PIND_Bit0);dispMode = 9; debounce = DEBOUNCE; return;}
    if(dispMode == 7 && !PIND_Bit0) {while(!PIND_Bit0);dispMode = 8; debounce = DEBOUNCE; return;}
    if(dispMode == 6 && !PIND_Bit0) {while(!PIND_Bit0);dispMode = 7; debounce = DEBOUNCE; return;}
  
    if(dispMode == 5 && !PIND_Bit0) {while(!PIND_Bit0);dispMode = 6; debounce = DEBOUNCE; return;}
    if(dispMode == 4 && !PIND_Bit0) {while(!PIND_Bit0);dispMode = 5; debounce = DEBOUNCE; return;}
    
    
    if(!PIND_Bit4) 
    {
        while(!PIND_Bit4){} 
        debounce = DEBOUNCE;

        dispMode = 12;         
    }
    
    if(!PIND_Bit3) 
    {
        while(!PIND_Bit3){}
        debounce = DEBOUNCE;
  
        dispMode = 2;      
    }
    
    if(!PIND_Bit2) 
    {
        while(!PIND_Bit2){}
        debounce = DEBOUNCE; 
        
        waitTimer=0;
        smoothVar=0;
        smoothTimer=0;
 
        dispMode = 3;
    }
    
    if(!PIND_Bit0) 
    {
        while(!PIND_Bit0);
        debounce = DEBOUNCE;
        
        dispMode = 4;         
    }
 
    
    //Battery Section Indication
    if(adc>=751) OCR1B = 0;
    else if((adc<750) && (adc >680)) OCR1B = (unsigned char)(2630-3.5*ADC);  
    else if(adc<=680)
    {
        flash--;
        if(!flash)
        {
            flash = FLASH;
            if(OCR1B>20) {OCR1B = 0;}
            else {OCR1B = 250;}  
        }
    } 
    //\\ Battery Section Indication
}

 
//‘ункци€ проверки контрольной суммы
unsigned char CalcCRC (unsigned char* buf, unsigned char k)
{
   char j=0;
   unsigned char i;
   for (i=0;i<6;i++){
     if(buf[i] > 253) 
       j+= 253;
     else
       j += buf[i];
   }
   return (j);
}
 

// Put a char (with adress mean)
void putchar(unsigned char data) 
{ 
    if(data >=254) data = 253;
    
    while (!(UCSRA & (1<<UDRE)));
    UDR = data;   
}


// Put a char
void putcharC(unsigned char data) 
{   
    while (!(UCSRA & (1<<UDRE)));
    UDR = data;   
}


//UART Handler
void UART(void)
{
    char k=0;
    
    putcharC(254);
    k=0;
    while(k<6)
        putchar(colorL[k++]);
    putchar(CalcCRC(colorL,6));
    
    _delay_us(100);
    
    putcharC(255);
    k = 0;
    while(k<6)
        putchar(colorR[k++]); 
    putchar(CalcCRC(colorR,6));
    
    _delay_us(100);
    
}


//COLOR BOTH CHANNELS
void setColor(unsigned char R, unsigned char G, unsigned char B)
{
    colorL[0]=R;  colorR[0]=R;
    colorL[1]=G;  colorR[1]=G;
    colorL[2]=B;  colorR[2]=B;
    colorL[3]=R;  colorR[3]=R;
    colorL[4]=G;  colorR[4]=G;
    colorL[5]=B;  colorR[5]=B;  
    UART();
    UART();  
}

//COLOR - LEFT CHANNEL
void setColorL(unsigned char R, unsigned char G, unsigned char B)
{
    colorL[0]=R; 
    colorL[3]=R;
    colorL[1]=G;
    colorL[4]=G;
    colorL[2]=B;
    colorL[5]=B;  
    UART();
    UART();       
}

//COLOR - RIGHT CHANNEL
void setColorR(unsigned char R, unsigned char G, unsigned char B)
{
    colorR[0]=R;
    colorR[3]=R;
    colorR[1]=G;
    colorR[4]=G;
    colorR[2]=B;
    colorR[5]=B;  
    UART();
    UART();      
}

//RESET COLORS
void resetColor(void)
{ 
    for(unsigned char i=0; i<6; i++)
    {
        colorL[i]=0;
        colorR[i]=0;
    }
    
    UART(); 
    UART(); 
}



// PROGRAMMS PROGRAMMS PROGRAMMS PROGRAMMS PROGRAMMS PROGRAMMS 

void pr1(void)      //Ѕ„Ѕ   
{        
    colorL[3]=255; 
    colorL[4]=200;
    colorL[5]=200;
    colorL[0]=255; 
    colorL[1]=0;
    colorL[2]=0;
    
    colorR[3]=255; 
    colorR[4]=0;
    colorR[5]=0;
    colorR[0]=255; 
    colorR[1]=200;
    colorR[2]=200;
    UART();     
}



void pr_Strb_Police(void)      //Police lights
{
    setColor(255,0,0);
    resetColor();
    setColor(255,0,0);
    resetColor();
    setColor(255,0,0);
    resetColor();
    
    _delay_ms(250);
    
    setColor(0,0,255);
    resetColor();
    setColor(0,0,255);
    resetColor();
    setColor(0,0,255);
    resetColor();
    
    _delay_ms(250);
}


void pr_Strb_1(void)      //Strob lights
{
    if(!waitTimer)
    {  
        static unsigned char modeBlank;
        
        if(modeBlank)
        {
            modeBlank = 0;
            HSVtoRGB(&R, &G, &B , ((rand()+TCNT0) % 360));
            setColor(R,G,B);
            
            waitTimer = 2;
        }
        else
        {
            modeBlank = 1;
            setColor(0,0,0);
            
            waitTimer = ((rand()+TCNT0)%6+2);
        }
    }
}




void pr3(void)      //Smooth random color change
{
    if(!waitTimer && !smoothVar)
    {
        HSVtoRGB(&R, &G, &B , ((rand()+TCNT0) % 360));
        
        //R=0;G=255;B=20;
        for(unsigned int bit=0; bit<4; bit++){
            targetR[bit]=R; targetG[bit]=G; targetB[bit]=B;
            if(outR[bit] > targetR[bit]) ClearBit(speedR, bit); else SetBit(speedR, bit);
            if(outG[bit] > targetG[bit]) ClearBit(speedG, bit); else SetBit(speedG, bit); 
            if(outB[bit] > targetB[bit]) ClearBit(speedB, bit); else SetBit(speedB, bit);
        }
        smoothVar = 1;//rand() % 30;
        Smooth(smoothVar);  
    }
    
    if(reachR==0x0F && reachG==0x0F && reachB==0x0F && !waitTimer) //???????? ??? ????????? ?????
    {
        /*do{ 
           waitTimer = (rand() % 3492); 
        }while (waitTimer < 1000); */
        waitTimer = (rand() % 100+10); 
        smoothVar = 0;
    }
    
    if(!waitTimer) Smooth(smoothVar);
}


void pr4(void)      //Smooth random color change + BLANK pause
{
  static unsigned char modeBlank;
  
    if(!waitTimer && !smoothVar)
    {
        if(modeBlank == 0)
        {
            HSVtoRGB(&R, &G, &B , ((rand() + TCNT0) % 360));
            modeBlank = 1;   
        }
        else 
        {
            R = 0;
            G = 0;
            B = 0;
            modeBlank = 0;
        }
        
        //R=0;G=255;B=20;
        for(unsigned int bit=0; bit<4; bit++){
            targetR[bit]=R; targetG[bit]=G; targetB[bit]=B;
            if(outR[bit] > targetR[bit]) ClearBit(speedR, bit); else SetBit(speedR, bit);
            if(outG[bit] > targetG[bit]) ClearBit(speedG, bit); else SetBit(speedG, bit); 
            if(outB[bit] > targetB[bit]) ClearBit(speedB, bit); else SetBit(speedB, bit);
        }
        smoothVar = 1;//rand() % 5;
        Smooth(smoothVar);  
    }
    
    if(reachR==0x0F && reachG==0x0F && reachB==0x0F && !waitTimer) //???????? ??? ????????? ?????
    {
       // do{ 
           waitTimer = (rand() % 100+10); 
        //}while (waitTimer < 1000); 
        smoothVar = 0;
    }
    
    if(!waitTimer) Smooth(smoothVar);
}
//\\ PROGRAMMS PROGRAMMS PROGRAMMS PROGRAMMS PROGRAMMS PROGRAMMS 




/*--------------*/
/*     MAIN     */
/*--------------*/
int main( void )
{
    
    //PORT init
    DDRB = 0x04;  DDRC = 0x3C;  DDRD = 0x02;
    PORTB = 0x00; PORTC = 0x00; PORTD = 0x1D; 
    
    //PWM                
    TCCR1A |= (1<<COM1A1)|(1<<COM1B1)|(0<<COM1A0)|(0<<COM1B0)|(1<<WGM10)|(0<<WGM11); //PWM phase correct 8-bit
    TCCR1B |= (1<<WGM12)|(0<<WGM13)|(0<<CS12)|(1<<CS11)|(0<<CS10); // using prescaler of 64 with a clock frequency of 1Mhz
    TCCR0  |= 0x05;
    TIMSK  |= 0x01;
    
    //ADC init
    //ацп - Avcc, выравнивание влево
    ADMUX = (0<<REFS1)|(1<<REFS0)|(0<<ADLAR) | (0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(1<<MUX0);
    //ADC enabled, f=FREQ/128, ADC_int enabled 
    ADCSR = (1<<ADEN)|(1<<ADSC)|(0<<ADFR)|(1<<ADIE) | (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); 
    
    //UART
    UBRRH = 0;
    UBRRL = 51; //скорость обмена 9600 бод
    //разр. прерыв при приеме, разр приема, разр передачи.
    UCSRB = (0<<RXCIE)|(1<<RXEN)|(1<<TXEN);  
    //обращаемс€ к регистру UCSRS, размер слова Ц 8 бит
    UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);

    __enable_interrupt();
    
    
        
    while(1)
    {
        //Display1();
        //Display2();
        //GetVal();
        //UART();
        
       // OCR1B = 0;
        
        switch(dispMode)
        {
        case 1:
            pr1();
            break;
        case 2:
            pr_Strb_Police();
            break;
        case 3:
            pr3();
            break;
        case 4:
            setColor(255,0,0);      //RED
            break;
        case 5:
            setColor(0,255,0);      //GREEN
            break;
        case 6:
            setColor(0,0,255);      //BLUE
            break;
        case 7:
            setColor(255,100,0);    //Yellow
            break;
        case 8:
            setColor(0,150,255);    //CYAN
            break;
        case 9:
            setColor(255,0,100);    //Magenta
            break;
        case 10:
            setColor(255,100,100);  //White
            break;
        case 12:
            setColor(0,0,0);        //No Color
            break;
        case 13:
            pr4();
            break;
        case 14:
            pr_Strb_1();
            break;
        }  
        if(waitTimer > 120) waitTimer = 25;
    }
}




//++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*INTERRUPTS*/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma vector = TIMER0_OVF_vect                            
__interrupt void D(void)  
{
    __disable_interrupt();
       
    
    Leds();
    
    if(debounce)
        debounce --;
    else
        Buttons();
    
    
    __enable_interrupt();
    
    
    
    if(smoothTimer) smoothTimer--;
    if(waitTimer) waitTimer--;
}
 

#pragma vector = ADC_vect                             
__interrupt void measure(void)  
{  
    unsigned int temp = 0;    
        
    temp = ADCL;
    temp |= (ADCH<<8); 
    adc = temp;

    ADCSR |= (1<<ADSC);
}



//*METHODS

void Smooth( unsigned char load )
{
          
    if( smoothTimer == 0 )
    {
          for(unsigned char x=0; x<4; x++)
          {
               
                if(outR[x] == targetR[x]) SetBit(reachR, x); else ClearBit(reachR, x);
                if(outG[x] == targetG[x]) SetBit(reachG, x); else ClearBit(reachG, x);
                if(outB[x] == targetB[x]) SetBit(reachB, x); else ClearBit(reachB, x);
                
                if(BitIsClear(reachR, x)) //если цвет не достигнут на "х" канале
                {
                    if(BitIsSet(speedR, x)) outR[x]++; //++
                       else outR[x]--; //--
                       
                    if ((BitIsSet(speedR, x)) && ( outR[x] >= targetR[x] )) 
                        { outR[x] = targetR[x]; SetBit(reachR,x); }  //цвет достигнут 
                    if ((BitIsClear(speedR, x)) && ( outR[x] <= targetR[x] )) 
                        { outR[x] = targetR[x]; SetBit(reachR,x); }  //цвет достигнут 
                }
                if(BitIsClear(reachG, x)) //если цвет не достигнут на "х" канале
                {
                    if(BitIsSet(speedG, x)) outG[x]++; //++
                       else outG[x]--; //--
                       
                    if ((BitIsSet(speedG, x)) && ( outG[x] >= targetG[x] ))
                        { outG[x] = targetG[x]; SetBit(reachG,x); }  //цвет достигнут 
                    if ((BitIsClear(speedG, x)) && ( outG[x] <= targetG[x] )) 
                        { outG[x] = targetG[x]; SetBit(reachG,x); }  //цвет достигнут 
                }
                if(BitIsClear(reachB, x)) //если цвет не достигнут на "х" канале
                {
                    if(BitIsSet(speedB, x)) outB[x]++; //++
                       else outB[x]--; //--
                       
                    if ((BitIsSet(speedB, x)) && ( outB[x] >= targetB[x] ))
                        { outB[x] = targetB[x]; SetBit(reachB,x); }  //цвет достигнут 
                    if ((BitIsClear(speedB, x)) && ( outB[x] <= targetB[x] )) 
                        { outB[x] = targetB[x]; SetBit(reachB,x); }  //цвет достигнут 
                }
                switch(x)
                {
                case 0:
                colorL[0]=outR[x];
                colorL[1]=outG[x];
                colorL[2]=outB[x];
                break;
                case 1:
                colorL[3]=outR[x];
                colorL[4]=outG[x];
                colorL[5]=outB[x];
                break;
                case 2:
                colorR[0]=outR[x];
                colorR[1]=outG[x];
                colorR[2]=outB[x];
                break;
                case 3:
                colorR[3]=outR[x];
                colorR[4]=outG[x];
                colorR[5]=outB[x];
                break;
                }
                
          }
          smoothTimer = load;
    }
    UART();
}


static void HSVtoRGB( unsigned char *r, unsigned char *g, unsigned char *b, unsigned int i_hue )
{
    unsigned int ii;
    double fr, hue;
    unsigned char c2, c3;
    
    while (i_hue >= 360) i_hue -= 360;
    
    hue = i_hue;
    ii = (int)(hue /= 60.0);
    fr = hue - ii;
    c2 = (unsigned char)(255 - 255 * fr);
    c3 = (unsigned char)(255 * fr);
    
    switch (ii)
    { 
        case 0: *r = 255; *g = c3;  *b = 0;   break;
        case 1: *r = c2;  *g = 200; *b = 0;   break;
        case 2: *r = 0;   *g = 200; *b = c3;  break;
        case 3: *r = 0;   *g = c2;  *b = 200; break;
        case 4: *r = c3;  *g = 0;   *b = 200; break;
        case 5: *r = 255; *g = 0;   *b = c2;  break;
    }
}
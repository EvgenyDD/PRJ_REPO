#include <stdlib.h>
#include <ioavr.h>
#include <intrinsics.h>

#include "mma7455.h"
#include "debug.h"
#include "string.h"

#define _delay_us(us)          __delay_cycles((FREQ/1000000)*(us))
#define _delay_ms(ms)          __delay_cycles((FREQ/1000)*(ms))

#define RIGHT PORTD_Bit5
#define LEFT PORTD_Bit6

signed int x, y, z;

volatile unsigned int color[6] =
{ 0, 0, 0, 0, 0, 0 };
/* буфер-----------------------------------------------------------*/
//#define RxBfrSz 15           //размер буфера
volatile unsigned char RxBfr[30]; //кольцевой (циклический) буфер
//unsigned char RxBfrTail = 0;  //"указатель" хвоста буфера
//unsigned char RxBfrHead = 0;  //"указатель" головы буфера
volatile unsigned char RxBfrCntr = 0; //счетчик символов

void Display1(void)
{
	OCR1A = 0;
	OCR1B = 0;
	OCR2 = 0;
	_delay_us(300);

	int yy = y > 0 ? y : -y;
	yy *= 3;
	RIGHT = 0;
#if 1  
	OCR1A = color[0];
	OCR1B = color[1];
	OCR2 = color[2];
#else
	OCR1A = y * 3; //(color[0]-yy)>0?(color[0]-yy):0;
	OCR1B = 0; //(color[1]-yy)>0?(color[1]-yy):0;
	OCR2 = 0; //(color[2]-yy)>0?(color[2]-yy):0;
#endif
	LEFT = 1;

	_delay_ms(5);
}
void Display2(void)
{
	OCR1A = 0;
	OCR1B = 0;
	OCR2 = 0;
	_delay_us(300);

	LEFT = 0;
	OCR1A = color[3];
	OCR1B = color[4];
	OCR2 = color[5];
	RIGHT = 1;

	_delay_ms(5);
}

//Процедура очистки буфера
void FlushBuf(void)
{
	RxBfrCntr = 0;
}

//Весь свет в 0
void Erase(void)
{
	for (unsigned char i = 0; i < 6; i++)
		color[i] = 0;
}

//Процедура пополнения буфера символом
void PutCharBuf(unsigned char sym)
{
	RxBfr[RxBfrCntr] = sym;
	RxBfrCntr++; //инкрементируем счетчик символов
}

//Функция проверки контрольной суммы
unsigned char CalcCRC(unsigned char* buf, unsigned char k)
{
	char j = 0;

	for (char i = 1; i < 7; i++)
		j += buf[i];

	if (j >= 254)
		j = 253;
	return (j);

}

//Функция обработки команды
void GetVal()
{
	if (RxBfrCntr >= 7)
	{
		if (1) //(CalcCRC (RxBfr,6) == RxBfr[7])
		{
			color[0] = RxBfr[1];
			color[1] = RxBfr[2];
			color[2] = RxBfr[3];
			color[3] = RxBfr[4];
			color[4] = RxBfr[5];
			color[5] = RxBfr[6];
			//FlushBuf();
			RxBfrCntr = 0;
		}
		else
		{
			RxBfrCntr = 0;
			//FlushBuf();
		}
	}
}

void TEST(void)
{
	static unsigned char cnt = 255;
	static unsigned char num = 0;

	cnt--;
	if (cnt == 0)
	{
		cnt = 255;

		switch (num)
		{
		case 0:
			color[0] = 0xff;
			color[1] = 0;
			color[2] = 0;
			color[3] = 0;
			color[4] = 0;
			color[5] = 0;
			break;
		case 1:
			color[0] = 0;
			color[1] = 0xff;
			color[2] = 0;
			color[3] = 0;
			color[4] = 0;
			color[5] = 0;
			break;
		case 2:
			color[0] = 0;
			color[1] = 0;
			color[2] = 255;
			color[3] = 0;
			color[4] = 0;
			color[5] = 0;
			break;
		case 3:
			color[0] = 0;
			color[1] = 0;
			color[2] = 0;
			color[3] = 0xff;
			color[4] = 0;
			color[5] = 0;
			break;
		case 4:
			color[0] = 0;
			color[1] = 0;
			color[2] = 0;
			color[3] = 0;
			color[4] = 0xff;
			color[5] = 0;
			break;
		case 5:
			color[0] = 0;
			color[1] = 0;
			color[2] = 0;
			color[3] = 0;
			color[4] = 0;
			color[5] = 255;
			break;
		}

		if (num++ == 6)
			num = 0;
	}

}

void SendAccel(int x, int y, int z)
{
	char tttt[15] = "X=", yy[7];
	itoa_(x, yy);
	strcat_(tttt, yy);
	strcat_(tttt, "Y=");
	itoa_(y, yy);
	strcat_(tttt, yy);
	strcat_(tttt, "Z=");
	itoa_(z, yy);
	strcat_(tttt, yy);
	DebugSendString(tttt);
}

int main(void)
{
    _delay_ms(5);
    x = y = z = 0;

    //PORT init
    DDRB = 0x0E;
    DDRC = 0x00;
    DDRD = 0x60;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x60;
    //PWM
    TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (0 << COM1A0) | (0 << COM1B0)
                    | (1 << WGM10) | (0 << WGM11); //PWM phase correct 8-bit
    TCCR1B |= (1 << WGM12) | (0 << WGM13) | (0 << CS12) | (0 << CS11)
                    | (1 << CS10); // using prescaler of 64 with a clock frequency of 1Mhz
    TCCR2 |= (1 << WGM20) | (1 << WGM21) | (0 << COM20) | (1 << COM21)
                    | (1 << CS20);
    /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     TCCR0 |= (1<<WGM00)|(1<<WGM01)|(0<<COM00)|(1<<COM01)|(1<<CS00);
     TCCR1A |= (1<<COM1A1)|(1<<COM1B1)|(0<<COM1A0)|(0<<COM1B0)|(1<<WGM10)|(0<<WGM11);
     TCCR1B |= (1<<WGM12)|(0<<WGM13)|(0<<CS12)|(0<<CS11)|(1<<CS10);
     */

    //UART
    UBRRH = 0;
    UBRRL = 71; //скорость обмена 9600 бод
    //разр. прерыв при приеме, разр приема, разр передачи.
    UCSRB = (1 << RXCIE) | (1 << RXEN) | (1 << TXEN);
    //обращаемся к регистру UCSRS, размер слова – 8 бит
    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);

    //MMA7455_init((1 << MMA7455_GLVL0) | (1 << MMA7455_MODE0));

    __enable_interrupt();

    unsigned char i;

    //begin test
#define CYCLEDEC 8

    for (i = 0; i < 255 - CYCLEDEC; i += CYCLEDEC)
    {
            color[0] = i;
            color[3] = i;
            //_delay_ms(1);
            Display1();
            Display2();
    }
    for (i = 255; i > CYCLEDEC; i -= CYCLEDEC)
    {
            color[0] = i;
            color[3] = i;
            //_delay_ms(1);
            Display1();
            Display2();
    }
    for (i = 0; i < 255 - CYCLEDEC; i += CYCLEDEC)
    {
            color[1] = i;
            color[4] = i;
            //_delay_ms(1);
            Display1();
            Display2();
    }
    for (i = 255; i > CYCLEDEC; i -= CYCLEDEC)
    {
            color[1] = i;
            color[4] = i;
            //_delay_ms(1);
            Display1();
            Display2();
    }
    for (i = 0; i < 255 - CYCLEDEC; i += CYCLEDEC)
    {
            color[2] = i;
            color[5] = i;
            //_delay_ms(1);
            Display1();
            Display2();
    }
    for (i = 255; i > CYCLEDEC; i -= CYCLEDEC)
    {
            color[2] = i;
            color[5] = i;
            //_delay_ms(1);
            Display1();
            Display2();
    }

    Erase();
    //end of begin test

    //MAIN CYCLE
    while (1)
    {
//      MMA7455_xyz(&x, &y, &z); // get the accelerometer values.            
//      
//      if(y < -45) color[5] = 0;
//      else
//        color[5] = (45-abs(y))*6;
//     // color[5] = 3*(80-abs(y)) ;
//      color[0] = color[1] = color[3] = color[4] = color[2] = 0;
        Display1();
        Display2();
        GetVal();

        
      /*
        static uint8_t val = 0;
        if (++val == 80)
        {
            SendAccel(x, y, z);
            val = 0;
        }
		*/
        //_delay_ms(100);

        //  TEST();
    }
}

//прием символа по usart`у в буфер
#pragma vector=USART_RXC_vect
__interrupt void usart_rxc_my(void)
{
	unsigned char data;
	data = UDR;
	// помещаем принятый символ в буфер
	if (RxBfrCntr)
	{ //в буфере есть 1 и более символов
		if (RxBfrCntr < 8)
		{
                    PutCharBuf(data);
		}
	}
	else
	{ //буфер пустует
		if (data == 255)
	        {
			PutCharBuf(data);
		}
	}

}


#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>



#define BitSet(p,m) 			((p) |= (1<<(m)))
#define BitReset(p,m) 			((p) &= ~(1<<(m)))
#define BitFlip(p,m) 			((p) ^= (1<<(m)))
#define BitWrite(c,p,m) 		((c) ? BitSet(p,m) : BitReset(p,m))
#define BitIsSet(reg, bit) 		(((reg) & (1<<(bit))) != 0)
#define BitIsReset(reg, bit) 	(((reg) & (1<<(bit))) == 0)

#define ButtonState1 	BitIsReset(PINB, 1)
#define ButtonStateFw 	BitIsReset(PINA, 1)
#define ButtonStateUsb 	BitIsReset(PINA, 0)
#define ButtonState4 	BitIsReset(PIND, 2)

#define LED_FW(state) 	BitWrite(state, PORTB, 4)
#define LED_USB(state) 	BitWrite(state, PORTB, 3)

#define LED_1(state) 	BitWrite(state, PORTB, 5)
#define LED_2(state) 	BitWrite(state, PORTB, 2)
#define LED_3(state) 	BitWrite(state, PORTB, 0)
#define LED_4(state) 	BitWrite(state, PORTD, 6)

#define LEDS_ON			do{LED_1(1);LED_2(1);LED_3(1);LED_4(1);}while(0);
#define LEDS_OFF		do{LED_1(0);LED_2(0);LED_3(0);LED_4(0);}while(0);

#define RelayFw(state)	BitWrite(state, PORTD, 5)
#define RelayUsb(state)	BitWrite(state, PORTD, 4)

uint8_t switchState;

enum{STATE_OFF, STATE_USB, STATE_FW};


#define DELAY_SW	((1000)/2)
#define DELAY_RLX	((1000)/2)
#define DELAY_PWRUP	((4000)/2)

#define wdt_reset() __asm__ __volatile__ ("wdr")

void RunningRight()
{
	LEDS_OFF;
	LED_1(1);
	_delay_ms(DELAY_PWRUP/4);
	wdt_reset();
	LED_2(1);
	_delay_ms(DELAY_PWRUP/4);
	wdt_reset();
	LED_3(1);
	_delay_ms(DELAY_PWRUP/4);
	wdt_reset();
	LED_4(1);
	_delay_ms(DELAY_PWRUP/4);
	wdt_reset();
	LEDS_OFF;
}

void RunningLeft()
{
	LEDS_ON;
	_delay_ms(DELAY_PWRUP/4);
	wdt_reset();
	LED_4(0);
	_delay_ms(DELAY_PWRUP/4);
	wdt_reset();
	LED_3(0);
	_delay_ms(DELAY_PWRUP/4);
	wdt_reset();
	LED_2(0);
	_delay_ms(DELAY_PWRUP/4);
	wdt_reset();
	LED_1(0);
}


static void HandlePressI()
{
	if(switchState == STATE_OFF)
	{
		LED_USB(1);
		RelayUsb(1);
		switchState = STATE_USB;
	}
	else if(switchState == STATE_USB)
	{
		LED_USB(0);
		RelayUsb(0);
		switchState = STATE_OFF;
	}
	else if(switchState == STATE_FW)
	{
		LED_FW(0);
		RelayFw(0);

		RunningRight();

		LED_USB(1);
		RelayUsb(1);
		switchState = STATE_USB;
	}
}

static void HandlePressII()
{
	if(switchState == STATE_OFF)
	{
		LED_FW(1);
		RelayFw(1);
		switchState = STATE_FW;
	}
	else if(switchState == STATE_FW)
	{
		LED_FW(0);
		RelayFw(0);
		switchState = STATE_OFF;
	}
	else if(switchState == STATE_USB)
	{
		LED_USB(0);
		RelayUsb(0);

		RunningLeft();

		LED_FW(1);
		RelayFw(1);
		switchState = STATE_FW;
	}
}


int main(void)
{
	DDRA = 0x00;
	DDRB = 0x3D;
	DDRD = 0x78;

	PORTA = 0x03;
	PORTB = 0x02;
	PORTD = 0x04;

	//reset watchdog
	wdt_reset();
	//set up WDT interrupt
	WDTCSR = (1<<WDCE)|(1<<WDE);
	//Start watchdog timer with 4s prescaller
	WDTCSR = (1<<WDE)|(1<<WDP0)|(1<<WDP2);

	RelayUsb(0);
	RelayFw(0);

	LEDS_OFF;
	LED_FW(0);
	LED_USB(0);

	switchState = STATE_OFF;

	for(;;)
	{
		if(ButtonState1)
		{			
			HandlePressII();			

			//wait till button release
			while(ButtonState1)
			{
				LED_1(1);
				_delay_ms(100);
				wdt_reset();
			}
			LED_1(0);
			_delay_ms(100);
		}
		else if(ButtonStateFw)	//FW Button was pressed
		{
			HandlePressII();	

			//wait till button release
			while(ButtonStateFw)
			{
				LED_2(1);
				_delay_ms(100);
				wdt_reset();
			}
			LED_2(0);
		}
		else if(ButtonStateUsb)   //USB Button was pressed
		{
			HandlePressI();	

			//wait till button release
			while(ButtonStateUsb)
			{
				LED_3(1);
				_delay_ms(100);
				wdt_reset();
			}
			LED_3(0);
		}
		else if(ButtonState4)
		{
			HandlePressI();

			//wait till button release
			while(ButtonState4)
			{
				LED_4(1);
				_delay_ms(100);
				wdt_reset();
			}
			LED_4(0);
		}

		_delay_ms(20);
		wdt_reset();
	}

	return 0;
}

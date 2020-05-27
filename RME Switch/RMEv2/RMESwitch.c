#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>


#define BitSet(p,m) 			((p) |= (1<<(m)))
#define BitReset(p,m) 			((p) &= ~(1<<(m)))
#define BitFlip(p,m) 			((p) ^= (1<<(m)))
#define BitWrite(c,p,m) 		((c) ? BitSet(p,m) : BitReset(p,m))
#define BitIsSet(reg, bit) 		(((reg) & (1<<(bit))) != 0)
#define BitIsReset(reg, bit) 	(((reg) & (1<<(bit))) == 0)

#define ButtonStateOn 	BitIsReset(PINB, 1)
#define ButtonStateFw 	BitIsReset(PINA, 1)
#define ButtonStateUsb 	BitIsReset(PINA, 0)
#define ButtonStateRst 	BitIsReset(PIND, 2)

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
#define RelayPwr(state)	BitWrite(state, PORTD, 3)

uint8_t powerState, switchState, OnOffState;


#define DELAY_SW	((1000)/2)
#define DELAY_RLX	((1000)/2)
#define DELAY_PWRUP	((1000)/2)



void RunningRight()
{
	LEDS_OFF;
	LED_1(1);
	_delay_ms(DELAY_PWRUP/4);
	LED_2(1);
	_delay_ms(DELAY_PWRUP/4);
	LED_3(1);
	_delay_ms(DELAY_PWRUP/4);
	LED_4(1);
	_delay_ms(DELAY_PWRUP/4);
	LEDS_OFF;
}

void RunningLeft()
{
	LEDS_ON;
	_delay_ms(DELAY_PWRUP/4);
	LED_4(0);
	_delay_ms(DELAY_PWRUP/4);
	LED_3(0);
	_delay_ms(DELAY_PWRUP/4);
	LED_2(0);
	_delay_ms(DELAY_PWRUP/4);
	LED_1(0);
}

void PowerOn()
{
	powerState = eeprom_read_byte(0)?1:0;
	switchState = eeprom_read_byte(1);
	if(switchState > 2) //
		switchState = 0;
	OnOffState = 1;

	if(switchState == 1)
	{
		RelayFw(1);
		LED_FW(1);
	}
	else if(switchState == 2)
	{
		RelayUsb(1);
		LED_USB(1);
	}

	if(powerState)
	{
		RelayPwr(1);
	}	

	LED_1(0);
	LED_4(powerState);

	eeprom_write_byte(2, OnOffState);
}

void PowerOff()
{
	OnOffState = 0;

	RelayPwr(0);
	RelayFw(0);
	RelayUsb(0);
	LED_USB(0);
	LED_FW(0);

	LED_1(1);
	LED_4(0);

	eeprom_write_byte(2, OnOffState);		
}



int main(void)
{
	DDRA = 0x00;
	DDRB = 0x3D;
	DDRD = 0x78;

	PORTA = 0x03;
	PORTB = 0x02;
	PORTD = 0x04;

	powerState = eeprom_read_byte(0)?1:0;
	switchState = eeprom_read_byte(1);
	if(switchState > 2) //
		switchState = 0;
	OnOffState = eeprom_read_byte(2)?1:0;

	RelayPwr(0);
	RelayUsb(0);
	RelayFw(0);
	LEDS_OFF;
	LED_FW(0);
	LED_USB(0);

	//INIT
	if(OnOffState)
	{
		if(switchState == 1)
		{
			RelayFw(1);
			LED_FW(1);
		}
		else if(switchState == 2)
		{
			RelayUsb(1);
			LED_USB(1);
		}

		if(powerState)
		{
			RelayPwr(1);
			LED_4(1);
		}
	}	

	for(;;)
	{
		if(ButtonStateOn)
		{
			
			if(OnOffState) //was ON
				PowerOff();
			else if(powerState || switchState)
				PowerOn();
			
			//wait till button release
			while(ButtonStateOn)
				_delay_ms(100);
			_delay_ms(100);
		}
#define NUM_FLASH	4
		else if(ButtonStateFw)	//FW Button was pressed
		{
			if(OnOffState == 0)
			{
				switchState = 1;
				OnOffState = 1;

				eeprom_write_byte(1, switchState);	
				eeprom_write_byte(2, OnOffState);
				PowerOn();
			}
			else if(switchState == 1 && OnOffState) //FW was
			{ //the same state - wrong state
				switchState = 0;
				LED_FW(0);
				RelayFw(0);
				eeprom_write_byte(1, switchState);

				if(powerState == 0)
				{
					OnOffState = 0;
					eeprom_write_byte(2, OnOffState);
				}

				LED_1(!OnOffState);
			}
			else if(switchState == 2 && OnOffState)
			{ //another state - change switch
				RelayUsb(0);
				LED_USB(0);
				for(uint8_t i=0; i<NUM_FLASH; i++)
				{
					LEDS_ON;
					LED_FW(1);
					_delay_ms(DELAY_SW/2/NUM_FLASH);
					LEDS_OFF;
					LED_FW(0);
					_delay_ms(DELAY_SW/2/NUM_FLASH);
				}

				RelayFw(1);
				switchState = 1;
				OnOffState = 1;
				eeprom_write_byte(1, switchState);	
				eeprom_write_byte(2, OnOffState);	
				LED_FW(1);	
			}
			else
			{
				RelayFw(1);
				switchState = 1;
				OnOffState = 1;
				LED_1(!OnOffState);

				eeprom_write_byte(1, switchState);	
				eeprom_write_byte(2, OnOffState);	
				LED_FW(1);	
			}

			//wait till button release
			while(ButtonStateFw)
				_delay_ms(100);
			_delay_ms(100);
		}
		else if(ButtonStateUsb)   //USB Button was pressed
		{
			if(OnOffState == 0)
			{
				switchState = 2;
				OnOffState = 1;

				eeprom_write_byte(1, switchState);	
				eeprom_write_byte(2, OnOffState);	
				PowerOn();
			}
			else if(switchState==2 && OnOffState)
			{ //the same state - USB was
				LED_USB(0);
				RelayUsb(0);
				switchState = 0;
				eeprom_write_byte(1, switchState);	

				if(powerState == 0)
				{
					OnOffState = 0;
					eeprom_write_byte(2, OnOffState);
				}

				LED_1(!OnOffState);

			}
			else if(switchState==1 && OnOffState)
			{ //another state
				RelayFw(0);
				LED_FW(0);
				for(uint8_t i=0; i<NUM_FLASH; i++)
				{
					LEDS_ON;
					LED_USB(1);
					_delay_ms(DELAY_SW/2/NUM_FLASH);
					LEDS_OFF;
					LED_USB(0);
					_delay_ms(DELAY_SW/2/NUM_FLASH);
				}

				RelayUsb(1);

				switchState = 2;
				OnOffState = 1;

				eeprom_write_byte(1, switchState);	
				eeprom_write_byte(2, OnOffState);	
				LED_USB(1);	
			}
			else
			{
				RelayUsb(1);

				switchState = 2;
				OnOffState = 1;
				LED_1(!OnOffState);

				eeprom_write_byte(1, switchState);	
				eeprom_write_byte(2, OnOffState);	
				LED_USB(1);	
			}

BtnUSBEnd:
			//wait till button release
			while(ButtonStateUsb)
				_delay_ms(100);
			_delay_ms(100);
		}
		else if(ButtonStateRst)
		{
			if(powerState == 0)
			{
				if(OnOffState==0)
				{
					OnOffState = 1;
					eeprom_write_byte(2, OnOffState);
				}
				powerState = 1;
			}
			else if(powerState == 1 && OnOffState == 0)
			{
				OnOffState = 1;
				eeprom_write_byte(2, OnOffState);
			}
			else
			{
				powerState = 0;
				if(switchState == 0)
				{
					OnOffState = 0;
					eeprom_write_byte(2, OnOffState);
				}
			}

			RelayPwr(powerState);
			eeprom_write_byte(0, powerState);		


			LED_1(!OnOffState);
			LED_4(powerState);

			//wait till button release
			while(ButtonStateRst)
				_delay_ms(100);
			_delay_ms(100);
		}

		LED_1(!OnOffState);
		LED_4(powerState && OnOffState);

		_delay_ms(20);
	}

	return 0;
}

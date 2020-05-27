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

uint8_t powerState, switchState;


#define DELAY_SW	((2000)/2)
#define DELAY_RLX	((2500)/2)
#define DELAY_PWRUP	((6000)/2)

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

void RunningRelax()
{
	LEDS_OFF;
	LED_1(1);
	LED_3(1);
	_delay_ms(DELAY_RLX/4);
	LED_2(1);
	LED_4(1);
	LED_1(0);
	LED_3(0);
	_delay_ms(DELAY_RLX/4);
	LED_2(0);
	LED_4(0);
	LED_1(1);
	LED_3(1);
	_delay_ms(DELAY_RLX/4);
	LED_2(1);
	LED_4(1);
	LED_1(0);
	LED_3(0);
	_delay_ms(DELAY_RLX/4);
	LEDS_OFF;
}

void PowerOn()
{
	if(switchState)
	{
		RelayFw(1);
		LED_FW(1);
	}
	else
	{
		RelayUsb(1);
		LED_USB(1);
	}

	RunningRight();
	RelayPwr(1);

	powerState = 1;
	eeprom_write_byte(0, powerState);		
}

void PowerOff()
{
	RelayPwr(0);
	RunningLeft();
	RelayFw(0);
	RelayUsb(0);
	LED_USB(0);
	LED_FW(0);
	LED_1(1);

	powerState = 0;
	eeprom_write_byte(0, powerState);		
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
	switchState = eeprom_read_byte(1)?1:0;

	RelayPwr(0);
	RelayUsb(0);
	RelayFw(0);
	LEDS_OFF;
	LED_FW(0);
	LED_USB(0);

	//INIT
	if(powerState==0) //config = OFF mode
	{
		RelayPwr(0);
		RelayUsb(/*!switchState*/0);
		RelayFw(/*switchState*/0);
		LED_FW(0);
		LED_USB(0);
		LED_1(1);
	}
	else			//config = ON mode
	{
		RelayFw(switchState);
		RelayUsb(!switchState);
		LED_FW(switchState);
		LED_USB(!switchState);
		RunningRight();
		RelayPwr(powerState);
	}

	for(;;)
	{
		if(ButtonStateOn)
		{
			if(powerState) //was ON
				PowerOff();
			else 		//was OFF
				PowerOn();
			

			//wait till button release
			while(ButtonStateOn)
				_delay_ms(10);
		}
#define NUM_FLASH	4
		else if(ButtonStateFw)	//FW Button was pressed
		{
			if(powerState == 0)
			{
				switchState = 1;

				PowerOn();
				goto BtnFWEnd;
			}

			if(switchState) //FW was
			{ //the same state - wrong state
//				LED_USB(0);
//				LED_FW(1);
				for(uint8_t i=0; i<NUM_FLASH; i++)
				{
					LEDS_ON;
					_delay_ms(DELAY_SW/2/NUM_FLASH);
					LEDS_OFF;
					_delay_ms(DELAY_SW/2/NUM_FLASH);
				}
			}
			else
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
				eeprom_write_byte(1, switchState);	
				LED_FW(1);	
			}

BtnFWEnd:
			//wait till button release
			while(ButtonStateFw)
				_delay_ms(10);
		}
		else if(ButtonStateUsb)   //USB Button was pressed
		{
			if(powerState == 0)
			{
				switchState = 0;

				PowerOn();
				goto BtnUSBEnd;
			}

			if(switchState==0)
			{ //the same state - USB was
//				LED_USB(1);
//				LED_FW(0);
				for(uint8_t i=0; i<NUM_FLASH; i++)
				{
					LEDS_ON;
					_delay_ms(DELAY_SW/2/NUM_FLASH);
					LEDS_OFF;
					_delay_ms(DELAY_SW/2/NUM_FLASH);
				}
			}
			else
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
				switchState = 0;
				eeprom_write_byte(1, switchState);	
				LED_USB(1);	
			}

BtnUSBEnd:
			//wait till button release
			while(ButtonStateUsb)
				_delay_ms(10);
		}
		else if(ButtonStateRst)
		{
			if(powerState == 0)
			{
				switchState = 1;

				PowerOn();
				goto BtnRSTEnd;
			}
			RelayPwr(0);

			RunningLeft();

			RelayFw(0);
			RelayUsb(0);

			RunningRelax();

			if(switchState)
				RelayFw(1);
			else
				RelayUsb(1);

			RunningRight();

			RelayPwr(1);
BtnRSTEnd:
			//wait till button release
			while(ButtonStateRst)
				_delay_ms(10);
		}

		_delay_ms(20);
	}

	return 0;
}

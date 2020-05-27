#include <avr/io.h>
#include <stdio.h>

#include "hw_setup.h"
#include "avr_serial.h"
#include "utils.h"
#include "PPM_Output.h"
#include "HID_Joystick.h"

int main()
{
	// the PPM_HID jumber
	ClrBit(DDRD, PPM_HID_JUMPER);	// init for input
	SetBit(PORTD, PPM_HID_JUMPER);	// pull-up resistor
	
	// the POWER LED
	SetBit(DDRC, POWER_LED);	// init for output
	SetBit(PORTC, POWER_LED);	// turn the LED on
	
	// init the serial interface for debugging with printf
	//init_serial();

	// are we running in PPM output or HID Joystick mode?
	if (PIND & _BV(PPM_HID_JUMPER))
		HID_Joystick();
	else
		PPM_Output();
	
	return 0;
}

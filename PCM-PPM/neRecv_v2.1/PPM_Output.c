#include <avr/io.h>
#include <avr/interrupt.h>

#include "hw_setup.h"
#include "PPM_Output.h"
#include "neRX.h"
#include "utils.h"

// for 0.667 clock duration
#define us2clock(us)		(((us)*3)/2)

// durations of signals
#define FRAME_US		22000
#define PULSE_US		300

// maps the 0-127 range into microsecond durations 0-1000
#define TX2DUR(tx)		((tx*63)/8)

// number of output channels
#define NUM_CHANNELS	4

// number of elements in the durations array
#define MAX_DURS		((NUM_CHANNELS*2) + 2)

volatile uint16_t SignalDurs[MAX_DURS];
volatile uint8_t DurCnt = 0;

// Calculates the durations of the high/low signals of a PPM frame
void CalcDurations()
{
	// calc the signal durations
	uint16_t remain = FRAME_US, dur;
	uint8_t channel, ndx = 0;
	uint8_t* pChannel = &neSticks.pitch;
	for (channel = 0; channel < NUM_CHANNELS; channel++)
	{
		// the pulse
		SignalDurs[ndx++] = PULSE_US;
		remain -= PULSE_US;

		// calc the duration for this channel
		dur = 1000 - PULSE_US + TX2DUR(pChannel[channel]);
		SignalDurs[ndx++] = dur;
		remain -= dur;
	}

	// do the remaining pulse
	SignalDurs[ndx++] = PULSE_US;
	// do the gap to make the frame FRAME_US long
	SignalDurs[ndx] = remain - PULSE_US;
}

ISR(TIMER1_COMPA_vect) 
{ 
	TogBit(PORTD, PPM_OUT);	// toggle output

	// go to next duration
	DurCnt++;
	if (DurCnt == MAX_DURS)
		DurCnt = 0;
	
	// set new overflow value
	OCR1A = us2clock(SignalDurs[DurCnt]);
}

// Processes the input from the RX and creates PPM output
void PPM_Output()
{
	// setup the PPM out pin
	SetBit(DDRD, PPM_OUT);
	
#ifdef NEGATIVE_PPM
	ClrBit(PORTD, PPM_OUT);	// set the PPM low - this way we get negative PPM
#else
	SetBit(PORTD, PPM_OUT);	// set the PPM high - this way we get positive PPM
#endif	

	RX_Init();
	
	// the default positions
	neSticks.pitch		= 0;
	neSticks.rudder		= 0x40;
	neSticks.elevator	= 0x40;
	neSticks.aileron	= 0x40;

	CalcDurations();
	
	// setup the timer we're using for PPM
	SetBit(TCCR1B, WGM12);	// configure for CTC mode
	OCR1A = us2clock(SignalDurs[0]);	// default overflow

	SetBit(TIMSK1, OCIE1A);	// enable CTC interrupt
	
	sei();
	
	// start the PPM timer with prescaler 8
	// that gives us a 0.667us resolution with a 12MHz crystal
	SetBit(TCCR1B, CS11);	

	uint8_t need_recalc = 0;
	for (;;)
	{
		if (RX_ReadSticks())
			need_recalc = 1;
		
		// have all the signals been read by the ISR?
		if (DurCnt == MAX_DURS - 1  	// have we sent the last sample?
				&&  need_recalc			// do we have new data?
				&&  (OCR1A - TCNT1) > us2clock(50))		// do we have enough time to recalculate?
		{
			CalcDurations();
			
			need_recalc = 0;
		}
	}
}

#pragma once

typedef struct
{
	// stick position. the range of these is 0 - 0x7f
	uint8_t	pitch;
	uint8_t	rudder;
	uint8_t	elevator;
	uint8_t	aileron;
	
	/*
	// this is the extra data that the TX sends out
	// we don't need this, so it's commented out
	uint8_t		command;	// 0x55 for sticks data
	
	uint16_t	tx_id;		// id of the TX
							// Merlin 90: 0xA0FF
							// SoloPro :  0xF04A
	*/
	
} neSticks_t;

neSticks_t neSticks;

void RX_Init();
char RX_ReadSticks();

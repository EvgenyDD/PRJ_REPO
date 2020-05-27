#include <avr/io.h>
#include <stdio.h>
#include <string.h>

#include "hw_setup.h"
#include "neRX.h"
#include "utils.h"
#include "nRF24L01.h"

uint8_t is_bound;
uint16_t tx_id;

void RX_Init()
{
	nRF_Init();
	
	// init the TX signal LED
	SetBit(DDRC, TX_SIGNAL_LED);
	ClrBit(PORTC, TX_SIGNAL_LED);

	// init the 'binding' data
	is_bound = 0;
	
	// init the LED timer
	// prescaler 1024; overflow duration = 21.84ms
	SetVal(TCCR0B, _BV(CS02) | _BV(CS00));
	
	// receiver address
	const uint8_t RxAddr[] = {0x34, 0x43, 0x10, 0x10, 0x01};
	
	// init the receiver
	nRF_FlushRX();
	nRF_WriteAddrReg(RX_ADDR_P0, RxAddr, 5);
	nRF_WriteReg(RF_SETUP, vRF_DR_250KBPS);	// data rate
	nRF_WriteReg(RX_PW_P0, 7);				// RX payload length
	nRF_WriteReg(EN_RXADDR, vERX_P0);		// enable address
	nRF_WriteReg(RF_CH, 30);				// receiver channel
	nRF_WriteReg(STATUS, vRX_DR | vTX_DS | vMAX_RT);	// reset the IRQ flags
	nRF_WriteReg(CONFIG, vEN_CRC | vCRCO | vPWR_UP | vPRIM_RX);

	// power up the receiver
	nRF_CE_hi;
}

char RX_ReadSticks()
{
	// check if IRQ is low
	if ((PIND & _BV(IRQ_P)) == 0)
	{
		uint8_t num_ok_packets = 0;
		
		do {
			// read 7 bytes into RAM starting at nRF_data[1]
			nRF_ReadRxPayload(7);

			// do we have a sticks position packet?
			if (nRF_data[5] == 0x55)
			{
				// do we know the TX ID we will listen to?
				if (is_bound)
				{
					// is this packet from the TX we're bound to?
					if (tx_id == *((uint16_t*) (nRF_data + 6)))
					{
						// copy to buffer
						memcpy(&neSticks, nRF_data + 1, sizeof(neSticks));
						
						// we have read another packet
						num_ok_packets++;
					}
					
				} else {
				
					// do the 'binding'
					tx_id = *((uint16_t*) (nRF_data + 6));

					is_bound = 1;
					
					/*
					printf("bound to: %04X\n", tx_id);

					printf("packet: %02x %02x %02x %02x %02x %02x %02x\n", 
								nRF_data[1],
								nRF_data[2],
								nRF_data[3],
								nRF_data[4],
								nRF_data[5],
								nRF_data[6],
								nRF_data[7]);
								*/
				}
			}

			// reset the RX_DR
			nRF_WriteReg(STATUS, vRX_DR);
			
		} while (HAS_RX_PAYLOAD(*nRF_data));
		
		// handle the TX LED
		if (num_ok_packets)
		{
			// turn the TX LED on
			SetBit(PORTC, TX_SIGNAL_LED);

			// reset the LED counter
			TCNT0 = 0;

			// reset overflow bit
			TIFR0 = _BV(TOV0);
		}
		
		return num_ok_packets;
	}

	// do we have an LED timer overflow?
	if (TIFR0 & _BV(TOV0))
	{
		// TX LED goes off
		ClrBit(PORTC, TX_SIGNAL_LED);

		// reset overflow bit
		TIFR0 = _BV(TOV0);
	}

	return 0;
}

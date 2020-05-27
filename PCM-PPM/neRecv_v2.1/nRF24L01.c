#include <avr/io.h>

#include "nRF24L01.h"
#include "utils.h"

uint8_t nRF_data[MAX_COMMAND_LENGTH];

void nRF_Init()
{
	ClrBit(DDRD, IRQ_P);	// input pin
	SetBit(PORTD, IRQ_P);	// enable pull-up
	
	// set the CSN and CE as output pins
	SetVal(DDRB, _BV(CSN_P) | _BV(CE_P));

	// set these to default states
    nRF_CE_lo;
    nRF_CSN_hi;
    
	// define relevant pins as output
    SetVal(DDRB, _BV(MOSI_P) | _BV(SCK_P) | _BV(SS_P));	// init SPI pins
	
	// enable SPI in Master Mode with SCK = CK/4
	SetVal(SPCR, _BV(SPE) | _BV(MSTR));

	volatile char IOReg;
	IOReg = SPSR;		// clear SPIF bit in SPSR
	IOReg = SPDR;
}

uint8_t nRF_ShiftCommand(uint8_t cmd_len)
{
	nRF_CSN_lo;

	uint8_t* pData = nRF_data;
	while (cmd_len > 0)
	{
		// set character to send the nRF through mosi
		SPDR = *pData;
		
		// wait until char is sent
		loop_until_bit_is_set(SPSR, SPIF);

		// remember the byte received from nRF through miso
		*pData = SPDR;
		
		++pData;
		--cmd_len;
	}
	
	nRF_CSN_hi;

	// return status register
	return *nRF_data;
}

uint8_t nRF_ShiftByte(uint8_t one_byte)
{
	nRF_CSN_lo;

	// set character to send the nRF through mosi
	SPDR = one_byte;
	
	// wait until char is sent
	loop_until_bit_is_set(SPSR, SPIF);

	// remember the byte received from nRF through miso
	*nRF_data = SPDR;
	
	nRF_CSN_hi;

	// return the STATUS register
	return *nRF_data;
}

uint8_t nRF_ReadReg(enum nRFRegister_e reg)
{
	nRF_data[0] = R_REGISTER | reg;
	
	return nRF_ShiftCommand(2);
}

uint8_t nRF_WriteReg(enum nRFRegister_e reg, uint8_t val)
{
	nRF_data[0] = W_REGISTER | reg;
	nRF_data[1] = val;
	
	return nRF_ShiftCommand(2);
}

uint8_t nRF_WriteAddrReg(enum nRFRegister_e reg, const uint8_t* addr, uint8_t addr_len)
{
	nRF_data[0] = W_REGISTER | reg;
	
	// set the data to be sent
	int c;
	for (c = 0; c < addr_len; c++)
		nRF_data[1 + c] = addr[c];
	
	return nRF_ShiftCommand(addr_len + 1);
}

uint8_t nRF_ReadAddrReg(enum nRFRegister_e reg, uint8_t addr_len)
{
	nRF_data[0] = R_REGISTER | reg;
	
	return nRF_ShiftCommand(addr_len + 1);
}

uint8_t nRF_ReadRxPayload(uint8_t num_bytes)
{
	nRF_data[0] = R_RX_PAYLOAD;
	
	return nRF_ShiftCommand(num_bytes + 1);
}

uint8_t nRF_WriteTxPayload(const uint8_t* payload, uint8_t num_bytes)
{
	nRF_data[0] = W_TX_PAYLOAD;
	
	// set the data to be sent
	int c;
	for (c = 0; c < num_bytes; c++)
		nRF_data[1 + c] = payload[c];
		
	return nRF_ShiftCommand(num_bytes + 1);
}

uint8_t nRF_FlushTX()
{
	return nRF_ShiftByte(FLUSH_TX);
}

uint8_t nRF_FlushRX()
{
	return nRF_ShiftByte(FLUSH_RX);
}
	
uint8_t nRF_NOP()
{
	return nRF_ShiftByte(NOP);
}

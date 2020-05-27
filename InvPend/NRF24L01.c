/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "NRF24L01.h"
//#include "spi.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint8_t TxMode;		// Flag which denotes transmitting mode
uint8_t Payload;


uint8_t MyAddress[] = {
	0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E
    
};

/* Receiver address */
uint8_t TxAddress[] = {
    0xE7,
    0xE7,
    0xE7,
    0xE7,
    0xE7
};

#define NRF24L01_CLEAR_INTERRUPTS	NRF24L01_WriteBit(7, 4, 1); \
									NRF24L01_WriteBit(7, 5, 1); \
									NRF24L01_WriteBit(7, 6, 1)


void NRF24L01_WriteBit(uint8_t reg, uint8_t bit, uint8_t value)
{
	uint8_t tmp = NRF24L01_ReadRegister(reg);
	if(value != 0)
		tmp |= 1 << bit;
	else
		tmp &= ~(1 << bit);

	NRF24L01_WriteRegister(reg, tmp);
}


/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void delay_ms(uint32_t nTime);


/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input		 :
*******************************************************************************/

/*******************************************************************************
* Function Name  : 
* Description    : 
*******************************************************************************/
void NRF24L01_Init() 
// Initializes pins ans interrupt to communicate with the MiRF module
// Should be called in the early initializing phase at startup.
{

    NRF24L01_CE_L;
    NRF24L01_CSN_H;

}


void NRF24L01_Config(uint8_t channel, uint8_t payload) 
// Sets the important registers in the MiRF module and powers the module
// in receiving mode
{
    // Set RF channel
	if(channel > 125) channel = 125;
    NRF24L01_WriteRegister(RF_CH, channel);

    // Set length of incoming payload 
	if(payload > 32) payload = 32;
	Payload = payload;
    NRF24L01_WriteRegister(RX_PW_P0, payload);

	// Set RADDR and TADDR as the transmit address since we also enable auto acknowledgement
	//NRF24L01_SetRxAdr(MyAddress);
	//NRF24L01_SetTxAdr(TxAddress);

	// Enable RX_ADDR_P0 address matching
	NRF24L01_WriteRegister(EN_RXADDR, 1<<ERX_P0);

    NRF24L01_PowerUpRx();     // Power up in receiving mode
    NRF24L01_CE_H;     // Listening for pakets
	delay_ms(1);
}

void NRF24L01_SetRF(NRF24L01_DataRate_t DataRate, NRF24L01_OutputPower_t OutPwr)
{
	uint8_t tmp = 0;
	
	if (DataRate == NRF24L01_DataRate_2M) {
		tmp |= 1 << NRF24L01_RF_DR_HIGH;
	} else if (DataRate == NRF24L01_DataRate_250k) {
		tmp |= 1 << NRF24L01_RF_DR_LOW;
	}
	//If 1Mbps, all bits set to 0
	
	if (OutPwr == NRF24L01_OutputPower_0dBm) {
		tmp |= 3 << NRF24L01_RF_PWR;
	} else if (OutPwr == NRF24L01_OutputPower_M6dBm) {
		tmp |= 2 << NRF24L01_RF_PWR;
	} else if (OutPwr == NRF24L01_OutputPower_M12dBm) {
		tmp |= 1 << NRF24L01_RF_PWR;
	}
	
	NRF24L01_WriteRegister(RF_SETUP, tmp);
}



void NRF24L01_SetRxAdr(uint8_t *adr) 
// Sets the receiving address
{
    NRF24L01_CE_L;
    NRF24L01_WriteRegisters(RX_ADDR_P0, adr, 5);
    NRF24L01_CE_H;
}


void NRF24L01_SetTxAdr(uint8_t *adr)
// Sets the transmitting address
{
	NRF24L01_CE_L;
    NRF24L01_WriteRegisters(TX_ADDR, adr, 5);
	NRF24L01_CE_H;
}

uint8_t NRF24L01_ReadRegister(uint8_t reg)
// Reads an byte from the  the MiRF registers.
{
    NRF24L01_CSN_L;
    SPI_Send(R_REGISTER | (REGISTER_MASK & reg));
    uint8_t value = SPI_Send(NOP);
    NRF24L01_CSN_H;
	
	return value;
}

void NRF24L01_WriteRegister(uint8_t reg, uint8_t value)
// Clocks only one byte into the given MiRF register
{
    NRF24L01_CSN_L;
    SPI_Send(W_REGISTER | (REGISTER_MASK & reg));
    SPI_Send(value);
    NRF24L01_CSN_H;
}

void NRF24L01_WriteRegisters(uint8_t reg, uint8_t *value, uint8_t len) 
// Writes an array of bytes into inte the MiRF registers.
{
    NRF24L01_CSN_L;
    SPI_Send(W_REGISTER | (REGISTER_MASK & reg));
    SPI_WriteMass(value,len);
    NRF24L01_CSN_H;
}








/*
SIGNAL(INT1_vect) 
{

#if 0
	PORTD |= (1<<5);
    // If still in transmitting mode then finish transmission 
    if (TxMode) 
	{
    
        // Read MiRF status 
        NRF24L01_CSN_L;                                
        SPI_Send(NOP);          // Read status register
        NRF24L01_CSN_H;                              

        NRF24L01_CE_L;                             // Deactivate transreceiver
        RX_POWERUP;                            		// Power up in receiving mode
        NRF24L01_CE_H;                             // Listening for pakets
		_delay_us(300);
        TxMode = 0;                                // Set to receiving mode

        NRF24L01_WriteRegister(STATUS,(1<<TX_DS)|(1<<MAX_RT)); // Reset status register
    }
	PORTD &= ~(1<<5);
#endif

}
*/



uint8_t NRF24L01_IsDataReady() 
// Checks if data is available for reading
{
    NRF24L01_CSN_L;                                
    uint8_t status = SPI_Send(NOP);          // Read status register
    NRF24L01_CSN_H;                               
    return status & (1<<RX_DR);
}

void NRF24L01_GetData(uint8_t *data) 
// Reads Payload bytes into data array
{
    NRF24L01_CSN_L;     

    SPI_Send( R_RX_PAYLOAD );            // Send cmd to read rx payload
    SPI_WrRdMass(data,data,Payload); // Read payload

    NRF24L01_CSN_H;                               
    NRF24L01_WriteRegister(STATUS,(1<<RX_DR));   // Reset status register
}




void NRF24L01_PowerUpRx()
{
	NRF24L01_CSN_L;                    
    SPI_Send( FLUSH_RX );     // Write cmd to flush tx fifo
    NRF24L01_CSN_H;  

	NRF24L01_CLEAR_INTERRUPTS;
	NRF24L01_CE_L;			
	NRF24L01_WriteRegister(CONFIG, NRF24L01_CONFIG | ( (1<<PWR_UP) | (1<<PRIM_RX) ) );
	NRF24L01_CE_H;
	delay_ms(2);
}



void NRF24L01_PowerUpTx()
{
	NRF24L01_CLEAR_INTERRUPTS;
    NRF24L01_WriteRegister(CONFIG, NRF24L01_CONFIG | (0<<PRIM_RX) | (1<<PWR_UP));
	delay_ms(2);
}




void NRF24L01_Send(uint8_t *value, uint8_t len) 
// Sends a data package to the default address. Be sure to send the correct
// amount of bytes as configured as payload on the receiver.
{
    NRF24L01_CE_L;

	NRF24L01_PowerUpTx();
    
    NRF24L01_CSN_L;                    
    SPI_Send( FLUSH_TX );     // Write cmd to flush tx fifo
    NRF24L01_CSN_H;                    
    
    NRF24L01_CSN_L;                    
    SPI_Send( W_TX_PAYLOAD ); // Write cmd to write payload
    SPI_WriteMass(value, len);   // Write payload
    NRF24L01_CSN_H;                    
    
    NRF24L01_CE_H;                     // Start transmission
}



NRF24L01_Transmit_Status_t NRF24L01_GetTransmissionStatus()
{

	NRF24L01_CSN_L;
	uint8_t status = SPI_Send(NOP);
	NRF24L01_CSN_H;

	if(BitIsSet(status, TX_DS))
		return NRF24L01_Transmit_Status_Ok;
	else if(BitIsSet(status, MAX_RT))
		return NRF24L01_Transmit_Status_Lost;
	
	return NRF24L01_Transmit_Status_Sending;
}










void SPI_WrRdMass(uint8_t *dataout, uint8_t *datain, uint8_t len)
// Shift full array through target device
{
	for(uint8_t i=0; i<len; i++) 
	{
		datain[i] = SPI_Send(dataout[i]);
	    /* SPDR = dataout[i];
	     while((SPSR & (1<<SPIF))==0);
	     datain[i] = SPDR;*/
	}
}

void SPI_WriteMass(uint8_t *dataout, uint8_t len)
// Shift full array to target device without receiving any byte
{
	for(uint8_t i=0; i<len; i++) 
	{
		SPI_Send(dataout[i]);
	     /*SPDR = dataout[i];
	     while((SPSR & (1<<SPIF))==0);*/
	}
}

uint8_t SPI_Send(uint8_t data)
// Clocks only one byte to target device and returns the received one
{
	/* Fill output buffer with data */
		SPI1->DR = data;
		/* Wait for transmission to complete */
		while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
		/* Wait for received data to complete */
		while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
		/* Wait for SPI to be ready */
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY));
		/* Return data from buffer */
		return SPI1->DR;
}


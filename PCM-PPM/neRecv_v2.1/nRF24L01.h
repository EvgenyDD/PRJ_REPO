#pragma once

// nRF (MiRF module) pins
#define CE_P	PB0
#define CSN_P	PB1
#define SS_P	PB2
#define MOSI_P	PB3
#define MISO_P	PB4
#define SCK_P	PB5
#define IRQ_P	PD7

// definitions for selecting and enabling SparkFun module
#define nRF_CSN_hi     PORTB |=  _BV(CSN_P)
#define nRF_CSN_lo     PORTB &= ~_BV(CSN_P)
#define nRF_CE_hi      PORTB |=  _BV(CE_P)
#define nRF_CE_lo      PORTB &= ~_BV(CE_P)

///////////////////////
// the registers
///////////////////////
enum nRFRegister_e
{
	CONFIG		= 0x00,
	EN_AA		= 0x01,
	EN_RXADDR	= 0x02,
	SETUP_AW	= 0x03,
	SETUP_RETR	= 0x04,
	RF_CH		= 0x05,
	RF_SETUP	= 0x06,
	STATUS		= 0x07,
	OBSERVE_TX	= 0x08,
	CD			= 0x09,
	RX_ADDR_P0	= 0x0A,
	RX_ADDR_P1	= 0x0B,
	RX_ADDR_P2	= 0x0C,
	RX_ADDR_P3	= 0x0D,
	RX_ADDR_P4	= 0x0E,
	RX_ADDR_P5	= 0x0F,
	TX_ADDR		= 0x10,
	RX_PW_P0	= 0x11,
	RX_PW_P1	= 0x12,
	RX_PW_P2	= 0x13,
	RX_PW_P3	= 0x14,
	RX_PW_P4	= 0x15,
	RX_PW_P5	= 0x16,
	FIFO_STATUS	= 0x17,

	DYNPD		= 0x1C,
	FEATURE		= 0x1D,

	undefined_reg = 0xff,
};

///////////////////////
// register bits
///////////////////////

// CONFIG
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0

// EN_AA
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0

// EN_RXADDR
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0

// RF_SETUP
#define CONT_WAVE   7
#define RF_DR_LOW   5
#define PLL_LOCK    4
#define RF_DR_HIGH  3
#define RF_PWR_HIGH 2
#define RF_PWR_LOW  1
#define LNA_HCURR   0	// obsolete in nRF24L01+

// STATUS
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define TX_FULL     0

// FIFO_STATUS
#define TX_REUSE        6
#define FIFO_TX_FULL    5
#define TX_EMPTY        4
#define RX_FULL         1
#define RX_EMPTY        0

///////////////////////
// register bit values
///////////////////////

// CONFIG
#define vMASK_RX_DR		_BV(MASK_RX_DR)
#define vMASK_TX_DS		_BV(MASK_TX_DS)
#define vMASK_MAX_RT	_BV(MASK_MAX_RT)
#define vEN_CRC			_BV(EN_CRC)
#define vCRCO			_BV(CRCO)
#define vPWR_UP			_BV(PWR_UP)
#define vPRIM_RX		_BV(PRIM_RX)

// EN_AA
#define vENAA_P5		_BV(ENAA_P5)
#define vENAA_P4		_BV(ENAA_P4)
#define vENAA_P3		_BV(ENAA_P3)
#define vENAA_P2		_BV(ENAA_P2)
#define vENAA_P1		_BV(ENAA_P1)
#define vENAA_P0		_BV(ENAA_P0)

// EN_RXADDR
#define vERX_P5			_BV(ERX_P5)
#define vERX_P4			_BV(ERX_P4)
#define vERX_P3			_BV(ERX_P3)
#define vERX_P2			_BV(ERX_P2)
#define vERX_P1			_BV(ERX_P1)
#define vERX_P0			_BV(ERX_P0)

// SETUP_AW -- address widths in bytes
#define vAW_3			1
#define vAW_4			2
#define vAW_5			3

// RF_SETUP
#define vCONT_WAVE		_BV(CONT_WAVE)
#define vRF_DR_LOW		_BV(RF_DR_LOW)
#define vPLL_LOCK		_BV(PLL_LOCK)
#define vRF_DR_HIGH		_BV(RF_DR_HIGH)
#define vRF_PWR_HIGH	_BV(RF_PWR_HIGH)
#define vRF_PWR_LOW		_BV(RF_PWR_LOW)
#define vLNA_HCURR		_BV(LNA_HCURR)	// obsolete in nRF24L01+

#define vRF_DR_1MBPS	0
#define vRF_DR_2MBPS	_BV(RF_DR_HIGH)
#define vRF_DR_250KBPS	_BV(RF_DR_LOW)

#define vRF_PWR_M18DBM	0b000
#define vRF_PWR_M12DBM	0b010
#define vRF_PWR_M6DBM	0b100
#define vRF_PWR_0DBM	0b110

// STATUS
#define vRX_DR			_BV(RX_DR)
#define vTX_DS			_BV(TX_DS)
#define vMAX_RT			_BV(MAX_RT)
#define vTX_FULL		_BV(TX_FULL)

#define RX_P_NO(stat)	((stat >> 1) & 7)
#define HAS_RX_PAYLOAD(stat)	((stat & 0b1110) < 0b1100)

// FIFO_STATUS
#define vTX_REUSE		_BV(TX_REUSE)
#define vTX_FULL		_BV(TX_FULL)
#define vTX_EMPTY		_BV(TX_EMPTY)
#define vRX_FULL		_BV(RX_FULL)
#define vRX_EMPTY		_BV(RX_EMPTY)


// commands
#define R_REGISTER			0b00000000
#define W_REGISTER			0b00100000
#define R_RX_PAYLOAD		0b01100001
#define W_TX_PAYLOAD		0b10100000
#define FLUSH_TX			0b11100001
#define FLUSH_RX			0b11100010
#define REUSE_TX_PL			0b11100011 
#define R_RX_PL_WID			0b01100000
#define W_ACK_PAYLOAD		0b10101000
#define W_TX_PAYLOAD_NOACK	0b10110000
#define NOP					0b11111111

#define MAX_COMMAND_LENGTH	33

// This buffer is used to send and receive data through SPI to the nRF24L01+ chip.
// It is used by the nRF commands to prepare the command to be sent over SPI and also
// receives the data sent by the chip. After each of the command functions
// this buffer will contain the value of STATUS register in nRF_data[0] and
// any data the command read from the chip.
uint8_t nRF_data[MAX_COMMAND_LENGTH];

void nRF_Init();

// All the command functions return the value of the STATUS register.
// The nRF_Read* commands store the data from the chip starting from nRF_data[1]

// read/write the single-byte registers
uint8_t nRF_WriteReg(enum nRFRegister_e reg, uint8_t val);
uint8_t nRF_ReadReg(enum nRFRegister_e reg);

// read/write the address registers
uint8_t nRF_WriteAddrReg(enum nRFRegister_e reg, const uint8_t* addr, uint8_t addr_len);
uint8_t nRF_ReadAddrReg(enum nRFRegister_e reg, uint8_t addr_len);

// reads the RX payload (max num_bytes == 32)
uint8_t nRF_ReadRxPayload(uint8_t num_bytes);

// writes the TX payload (max num_bytes == 32)
uint8_t nRF_WriteTxPayload(const uint8_t* payload, uint8_t num_bytes);

// the flush commands
uint8_t nRF_FlushTX();
uint8_t nRF_FlushRX();

// the NOP command
uint8_t nRF_NOP();

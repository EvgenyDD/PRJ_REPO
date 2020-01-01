// MMA7455 Accelerometer

#include <stdlib.h>
#include <ioavr.h>
#include <intrinsics.h>

#define TWI_FREQ 100000

#include "mma7455.h"
#define TWI_PORT PORTC
#define TWI_DDR  DDRC
#define TWI_SCL  5
#define TWI_SDA  4

// --------------------------------------------------------
// MMA7455_init
//
// Initialize the MMA7455.
// Set also the offset, assuming that the accelerometer is
// in flat horizontal position.
//
// Important notes about the offset:
// The sensor has internal registers to set an offset.
// But the offset could also be calculated by software.
// This function uses the internal offset registers
// of the sensor.
// That turned out to be bad idea, since setting the
// offset alters the actual offset of the sensor.
// A second offset calculation had to be implemented
// to fine tune the offset.
// Using software variables for the offset would be
// much better.
//
// The offset is influenced by the slightest vibration
// (like a computer on the table).
//
void MMA7455_init(uint8_t mode)
{
	/* setup TWI and pull-up resistors */
	TWI_PORT |= 1 << TWI_SCL | 1 << TWI_SDA;
	TWI_DDR &= ~(1 << TWI_SCL | 1 << TWI_SDA);

#define F_I2C 50000UL
#define TWBR_VALUE (((FREQ)/(F_I2C)-16)/2)

#if ((TWBR_VALUE > 255) || (TWBR_VALUE == 0))
#error "TWBR value is not correct"
#endif

	TWBR = TWBR_VALUE;
	TWSR = 0;

	MMA7455_Wr(MMA7455_MCTL, mode);
}

// The values are with integers as 64 per 'g'.
void MMA7455_xyz(int *pX, int *pY, int *pZ)
{
  static signed int XX, YY, ZZ = 0;
  static uint8_t avg = 0;
	uint8_t data[6];

	MMA7455_RdMulti6(data, MMA7455_XOUTL);
	/* data[0] = MMA7455_Rd(MMA7455_XOUTL);
	 data[1] = MMA7455_Rd(MMA7455_XOUTH);
	 data[2] = MMA7455_Rd(MMA7455_YOUTL);
	 data[3] = MMA7455_Rd(MMA7455_YOUTH);
	 data[4] = MMA7455_Rd(MMA7455_ZOUTL);
	 data[5] = MMA7455_Rd(MMA7455_ZOUTH);*/

	// The output is 10-bits and could be negative.
	// To use the output as a 16-bit signed integer,
	// the sign bit (bit 9) is extended for the 16 bits.
	if (data[1] & 0x02) // Bit 9 is sign bit.
		data[1] |= 0xFC; // Stretch bit 9 over other bits.
	if (data[3] & 0x02)
		data[3] |= 0xFC;
	if (data[5] & 0x02)
		data[5] |= 0xFC;

        if(++avg>=8)
        {
			avg = 0;
			// The result is the g-force in units of 64 per 'g'.
			*pX = XX/8;//;
			*pY = YY/8;//;
			*pZ = ZZ/8;//;   
			XX=YY=ZZ=0;
        }
		else
		{
			 XX +=(data[1] << 8) | data[0];
			 YY +=(data[3] << 8) | data[2];
			 ZZ +=(data[5] << 8) | data[4];
		}
}

uint8_t MMA7455_Rd(uint8_t reg)
{
	uint8_t data;

	/*формируем состояние СТАРТ*/
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;

	/*выдаем на шину пакет SLA-R*/
	TWDR = (MMA7455_I2C_ADDRESS << 1) | 0;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;

	/*выдаем на шину адрес*/
	TWDR = reg;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;

	/*формируем состояние СТАРТ*/
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;

	/*выдаем на шину пакет SLA-R*/
	TWDR = (MMA7455_I2C_ADDRESS << 1) | 1;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;

	/*считываем данные*/
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;
	data = TWDR;

	/*формируем состояние СТОП*/
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);

	return data;
}

void MMA7455_RdMulti6(uint8_t * buf, uint8_t reg)
{
	/*формируем состояние СТАРТ*/
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;

	/*выдаем на шину пакет SLA-R*/
	TWDR = (MMA7455_I2C_ADDRESS << 1) | 0;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;

	/*выдаем на шину адрес*/
	TWDR = reg;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;

	/*формируем состояние СТАРТ*/
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;

	/*выдаем на шину пакет SLA-R*/
	TWDR = (MMA7455_I2C_ADDRESS << 1) | 1;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;

	for (uint8_t i = 0; i < 6; i++)
	{
		/*считываем данные*/
		TWDR = 0;
		if (i != 5)
			TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
		else
			TWCR = (1 << TWINT) | (1 << TWEN);
		while (!(TWCR & (1 << TWINT)))
			;
		buf[i] = TWDR;
	}

	/*формируем состояние СТОП*/
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void MMA7455_Wr(uint8_t reg, uint8_t data)
{

	/*формируем состояние СТАРТ*/
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;

	/*выдаемна шину пакет SLA-W*/
	TWDR = (MMA7455_I2C_ADDRESS << 1) | 0;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;

	/*передаем адрес регистра ds1307*/
	TWDR = reg;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;

	/*передаем данные*/
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;

	/*формируем состояние СТОП*/
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

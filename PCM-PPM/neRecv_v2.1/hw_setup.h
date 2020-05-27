#pragma once

// These are the pins of the LEDs and the jumpers that the device uses.
// The SPI and CE/IRQ pin definitions are in nRF24L01.h
// If you change any of the definitions below, you also have to make sure
// that all the pin setups reflect the changes you made - all the DDRx,
// PORTx and PINx definitions!

// LEDs
#define TX_SIGNAL_LED	PC0
#define POWER_LED		PC1
#define BOOTLOAD_LED	PC2

// the PPM output pin
#define PPM_OUT			PD6

// jumpers
#define PPM_HID_JUMPER		PD5
#define BOOTLOAD_JUMPER		PC5

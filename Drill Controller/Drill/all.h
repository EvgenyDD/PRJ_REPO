#ifndef ALL_H
#define ALL_H


typedef unsigned char uint8_t; 
typedef unsigned int uint16_t;


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay_basic.h>
#include <util/delay.h>

#define FREQ 8000000UL
//#define _delay_us(us)          __delay_cycles((FREQ/1000000)*(us))
//#define _delay_ms(ms)          __delay_cycles((FREQ/1000)*(ms))
//#define _delay_s(s)            __delay_cycles((FREQ)*(s))


#define BitIsSet(reg, bit)        ((reg & (1<<(bit))) != 0)
#define BitIsReset(reg, bit)      ((reg & (1<<(bit))) == 0)
#define BitReset(reg, bit)        (reg) &= (~(1<<(bit))) 
#define BitSet(reg, bit)          (reg) |= (1<<(bit))  
#define BitFlip(reg, bit) 		  ((reg) ^= (1<<(bit)))

#endif //ALL_H

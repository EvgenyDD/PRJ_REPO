#include <stdio.h>
#include <avr/io.h>

#include "avr_serial.h"

#define BAUD 57600
#define MYUBRR (((((F_CPU * 10) / (16L * BAUD)) + 5) / 10) - 1)

static int serial_putchar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(serial_putchar, NULL, _FDEV_SETUP_WRITE);

static int serial_putchar(char c, FILE *stream)
{
    if (c == '\n')
		serial_putchar('\r', stream);
  
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
    
    return 0;
}

unsigned char serial_getchar()
{
    while (!(UCSR0A & _BV(RXC0)))
		;

    return UDR0;
}

void init_serial()
{
    // USART Baud rate:
    UBRR0H = MYUBRR >> 8;
    UBRR0L = MYUBRR;
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
    
    stdout = &mystdout; // Required for printf init
}

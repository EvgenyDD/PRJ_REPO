#ifndef MAIN_H
#define MAIN_H

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

void Delay( unsigned int delay);

void delay_ms(unsigned int delay_value);
uint16_t uint16_time_diff(uint16_t now, uint16_t before);

#endif


#ifndef __75HC595_H__
#define __75HC595_H__

void shift_reg_set (uint8_t val);
void shift_reg_set_reverse (uint8_t val);
void set_num(uint8_t val);
uint8_t countdown(uint8_t start, double delay);
uint8_t countup(uint8_t start, double delay);
void blink(uint8_t num, double delay);

#endif
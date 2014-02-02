#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "74HC595.h"
#include "SA23_11YWA.h"

#define BCD7SEG_BRIDGE

#define __DELAY_BACKWARD_COMPATIBLE__
#define F_CPU 1000000UL  // 1 MHz
//#define F_CPU 14.7456E6
#include <util/delay.h>

/*
 * SHCP: Shift clock pulse
 * STCP: Storage clock pulse
 * DS:   Data serial
 */
#ifndef BCD7SEG_BRIDGE
#define DS_PORT	PORTD
#define DS_PIN		0

#define STCP_PORT	PORTD
#define STCP_PIN	2

#define SHCP_PORT	PORTD
#define SHCP_PIN	1

#else

#define DS_PORT	PORTC
#define DS_PIN		5

#define STCP_PORT	PORTC
#define STCP_PIN	3

#define SHCP_PORT	PORTC
#define SHCP_PIN	4
#endif

#define DS_LOW  DS_PORT &= ~_BV(DS_PIN)
#define DS_HIGH DS_PORT |= _BV(DS_PIN)
#define STCP_LOW  STCP_PORT &= ~_BV(STCP_PIN)
#define STCP_HIGH STCP_PORT |= _BV(STCP_PIN)
#define SHCP_LOW  SHCP_PORT &= ~_BV(SHCP_PIN)
#define SHCP_HIGH SHCP_PORT |= _BV(SHCP_PIN)

void shift_reg_set (uint8_t val)					
{ 															
	uint8_t i; 	
												
	SHCP_LOW;									
	STCP_LOW;									
	DS_LOW;	
	
	for (i = 0; i < 8; i++) { 								
		if (val & (1 << i)) { 								
			DS_HIGH; 							
		} else {											
			DS_LOW;							
		}
		SHCP_HIGH;	
 		SHCP_LOW;		
	}													
	STCP_HIGH;	
	STCP_LOW;			
}	

void shift_reg_set_reverse (uint8_t val)					
{ 															
	int8_t i; 	
												
	SHCP_LOW;									
	STCP_LOW;									
	DS_LOW;	

	i = 7;
	
	do {
		if (val & (1 << i)) { 								
			DS_HIGH; 							
		} else {											
			DS_LOW;							
		}
		SHCP_HIGH;	
 		SHCP_LOW;
		i--;		
	} while (i >= 0);
													
	STCP_HIGH;	
	STCP_LOW;			
}	

uint8_t numbers[] = {
	0x08,
	0x6E,
	0x14,
	0x44,
	0x62,
	0x41,
	0x03,
	0x6C,
	0x00,
	0x60,
};

void set_num (uint8_t val)
{
	if (val > 9) {
		return;
	}
	
	shift_reg_set(numbers[val]);
}

uint8_t countdown(uint8_t start, double delay)
{
	int idx	 = start;
	
	while (idx >= 0) {
		shift_reg_set(numbers[idx--]);
		_delay_ms(delay);
	}
	
	if (idx < 0) {
		idx = 0;
	}
	
	return (idx);
}

uint8_t countup(uint8_t start, double delay)
{
	int idx	 = start;
	
	while (idx <= 9) {
		shift_reg_set(numbers[idx++]);
		_delay_ms(delay);
	}
	
	return (idx);
}

void blink(uint8_t num, double delay)
{	
	while (1) {
		shift_reg_set(numbers[num]);
		_delay_ms(delay);
		shift_reg_set(0xFF);
		_delay_ms(delay);
	}
}

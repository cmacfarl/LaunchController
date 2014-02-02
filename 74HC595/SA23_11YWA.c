#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "74HC595.h"
#include "SA23_11YWA.h"

segment_pin_map_t pin_map[] = {
	{ SEGMENT_A, SR_PIN_1 },
	{ SEGMENT_B, SR_PIN_2 },
	{ SEGMENT_C, SR_PIN_3 },
	{ SEGMENT_D, SR_PIN_4 },
	{ SEGMENT_E, SR_PIN_5 },
	{ SEGMENT_F, SR_PIN_6 },
	{ SEGMENT_G, SR_PIN_7 },
};

/*
 * Note that SFR ports are 8 bit registers.
 */
/*
typedef struct avr_segment_pin_map_ {
	led_segment_t segment;
	volatile uint8_t *port;
	uint8_t pin;
} avr_segment_pin_map_t;

avr_segment_pin_map_t avr_pin_map[][] = {
	{	
		{ SEGMENT_A, PORTD, 1 },
		{ SEGMENT_A, PORTD, 1 },
		{ SEGMENT_A, PORTD, 1 },	
		{ SEGMENT_A, PORTD, 1 },	
		{ SEGMENT_A, PORTD, 1 },	
		{ SEGMENT_A, PORTD, 1 },	
		{ SEGMENT_A, PORTD, 1 },	
	},
	{	
		{ SEGMENT_A, PORTC, 5 },	
		{ SEGMENT_B, PORTC, 4 },
		{ SEGMENT_C, PORTC, 3 },	
		{ SEGMENT_D, PORTC, 2 },	
		{ SEGMENT_E, PORTD, 2 },	
		{ SEGMENT_F, PORTD, 1 },	
		{ SEGMENT_G, PORTD, 0 },	
	}	
};
*/

segment_state_t zero[] = {
	{
		SEGMENT_A, true
	},
	{
		SEGMENT_B, true
	},
	{
		SEGMENT_C, true
	},
	{
		SEGMENT_D, true
	},
	{
		SEGMENT_E, true
	},
	{
		SEGMENT_F, true
	},
	{
		SEGMENT_G, false
	},
};

segment_state_t one[] = {
	{
		SEGMENT_A, false
	},
	{
		SEGMENT_B, true
	},
	{
		SEGMENT_C, true
	},
	{
		SEGMENT_D, false
	},
	{
		SEGMENT_E, false
	},
	{
		SEGMENT_F, false
	},
	{
		SEGMENT_G, false
	},
};

segment_state_t two[] = {
	{
		SEGMENT_A, true
	},
	{
		SEGMENT_B, true
	},
	{
		SEGMENT_C, false
	},
	{
		SEGMENT_D, true
	},
	{
		SEGMENT_E, true
	},
	{
		SEGMENT_F, false
	},
	{
		SEGMENT_G, true
	},
};

segment_state_t three[] = {
	{
		SEGMENT_A, true
	},
	{
		SEGMENT_B, true
	},
	{
		SEGMENT_C, true
	},
	{
		SEGMENT_D, true
	},
	{
		SEGMENT_E, false
	},
	{
		SEGMENT_F, false
	},
	{
		SEGMENT_G, true
	},
};

segment_state_t four[] = {
	{
		SEGMENT_A, false
	},
	{
		SEGMENT_B, true
	},
	{
		SEGMENT_C, true
	},
	{
		SEGMENT_D, false
	},
	{
		SEGMENT_E, false
	},
	{
		SEGMENT_F, true
	},
	{
		SEGMENT_G, true
	},
};

segment_state_t five[] = {
	{
		SEGMENT_A, true
	},
	{
		SEGMENT_B, false
	},
	{
		SEGMENT_C, true
	},
	{
		SEGMENT_D, true
	},
	{
		SEGMENT_E, false
	},
	{
		SEGMENT_F, true
	},
	{
		SEGMENT_G, true
	},
};

segment_state_t six[] = {
	{
		SEGMENT_A, false
	},
	{
		SEGMENT_B, false
	},
	{
		SEGMENT_C, true
	},
	{
		SEGMENT_D, true
	},
	{
		SEGMENT_E, true
	},
	{
		SEGMENT_F, true
	},
	{
		SEGMENT_G, true
	},
};

segment_state_t seven[] = {
	{
		SEGMENT_A, true
	},
	{
		SEGMENT_B, true
	},
	{
		SEGMENT_C, true
	},
	{
		SEGMENT_D, false
	},
	{
		SEGMENT_E, false
	},
	{
		SEGMENT_F, false
	},
	{
		SEGMENT_G, false
	},
};

segment_state_t eight[] = {
	{
		SEGMENT_A, true
	},
	{
		SEGMENT_B, true
	},
	{
		SEGMENT_C, true
	},
	{
		SEGMENT_D, true
	},
	{
		SEGMENT_E, true
	},
	{
		SEGMENT_F, true
	},
	{
		SEGMENT_G, true
	},
};

segment_state_t nine[] = {
	{
		SEGMENT_A, true
	},
	{
		SEGMENT_B, true
	},
	{
		SEGMENT_C, true
	},
	{
		SEGMENT_D, false
	},
	{
		SEGMENT_E, false
	},
	{
		SEGMENT_F, true
	},
	{
		SEGMENT_G, true
	},
};

segment_state_t off[] = {
	{
		SEGMENT_A, false
	},
	{
		SEGMENT_B, false
	},
	{
		SEGMENT_C, false
	},
	{
		SEGMENT_D, false
	},
	{
		SEGMENT_E, false
	},
	{
		SEGMENT_F, false
	},
	{
		SEGMENT_G, false
	},
};

static shift_reg_pin_t lookup_shift (led_segment_t segment)
{
	int i;
	
	for (i = 0; i < 8; i++) {
		if (pin_map[i].segment == segment) {
			return (pin_map[i].pin);
		}
	}
}

static uint8_t calc_hexmap (segment_state_t *num) 
{
	int i;
	uint8_t shift;
	uint8_t hexnum;
	
	hexnum = 0;
	
	for (i = 0; i < 7; i++) {
		if (num[i].on) {
			shift = lookup_shift(num[i].segment);
			hexnum = hexnum | (1 << shift);
		}		
	}
	
	return (hexnum);
}
/*
void sa23_11ywa_avr_set (uint8_t hexmap, uint8_t column)
{
	int start_pin;
	
		switch (column) {
			case 0:
				start_pin = COLUMN_ZERO_START_PIN;
				break;
			case 1:
				start_pin = COLUMN_ONE_START_PIN;
				break;
			default:
				return;
		}
		
		
}
*/
void sa23_11ywa_set_num (uint8_t val)
{
	uint8_t hexmap;
	
	switch (val) {
		case 0:
			hexmap = calc_hexmap(zero);
			break;
		case 1:
			hexmap = calc_hexmap(one);
			break;
		case 2:
			hexmap = calc_hexmap(two);
			break;
		case 3:
			hexmap = calc_hexmap(three);
			break;
		case 4:
			hexmap = calc_hexmap(four);
			break;
		case 5:
			hexmap = calc_hexmap(five);
			break;
		case 6:
			hexmap = calc_hexmap(six);
			break;
		case 7:
			hexmap = calc_hexmap(seven);
			break;
		case 8:
			hexmap = calc_hexmap(eight);
			break;
		case 9:
			hexmap = calc_hexmap(nine);
			break;
		default:
			hexmap = calc_hexmap(zero);
	}
	
	shift_reg_set_reverse(hexmap);
}

void sa23_11ywa_set_off (void)
{
	uint8_t hexmap;
	
	hexmap = calc_hexmap(off);
	shift_reg_set_reverse(hexmap);
}


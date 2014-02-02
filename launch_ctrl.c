/*
 * Launch controller for Estes rocket engines.
 */

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "74HC595.h"
#include "debounce.h"

#define F_CPU 1000000UL  // 1 MHz
//#define F_CPU 14.7456E6
#include <util/delay.h>

//Define functions
//======================
void ioinit(void);      	//Initializes IO
//======================

#define TRANSISTOR_PIN 2
#define LAUNCH_PORT_0  2
#define LAUNCH_PORT_1  2
#define LAUNCH_PORT_2  2
#define LAUNCH_PORT_3  2

/*
 * Launch Button
 * Set this pin to INPUT
 */
#define LB_PORT		PORTC
#define LB_PIN		2

/*
 * Launch Control Switches
 * Set these pins to INPUT.
 */
#define LCS_PORT	PORTC
#define LCS0_PIN	3
#define LCS1_PIN	4

/*
 * Launch Control LEDs
 */
#define LC0_PORT	PORTB	
#define LC1_PORT	PORTD
#define LC0_0_PIN	6
#define LC0_1_PIN	7
#define LC1_0_PIN	5
#define LC1_1_PIN	6

#define LC0_LOW  LC0_PORT &= ~_BV(LC0_0_PIN)
#define LC0_HIGH LC0_PORT |= _BV(LC0_0_PIN)
#define LC1_LOW  LC0_PORT &= ~_BV(LC0_1_PIN)
#define LC1_HIGH LC0_PORT |= _BV(LC0_1_PIN)
#define LC2_LOW  LC1_PORT &= ~_BV(LC1_0_PIN)
#define LC2_HIGH LC1_PORT |= _BV(LC1_0_PIN)
#define LC3_LOW  LC1_PORT &= ~_BV(LC1_1_PIN)
#define LC3_HIGH LC1_PORT |= _BV(LC1_1_PIN)

/*
 * Launch transitors
 */
#define LT_PORT		PORTB
#define LT_0_PIN	2
#define LT_1_PIN	3
#define LT_2_PIN	4
#define LT_3_PIN	5

typedef enum launch_cfg_ {
	LC_ALL,
	LC_SLOW_STAGGER,
	LC_FAST_STAGGER,
	LC_TWO_BY_TWO
} launch_cfg_t;

launch_cfg_t g_launch_cfg = LC_ALL;

void delay_ms(uint32_t ms)
{
	for (int i=0; i<ms; i++) {
		_delay_ms(1);
	}		
}

/*
 * 00 - LC_ALL
 * 01 - LC_SLOW_STAGGER
 * 10 - LC_FAST_STAGGER
 * 11 - LC_TWO_BY_TWO
 */
launch_cfg_t get_launch_cfg()
{
	launch_cfg_t lc = 0;
	
	if ((PINC & (1 << PINC2)) == 0) {
		lc = lc | (1 << 1);
	}

	if ((PINC & (1 << PINC1)) == 0) {
		lc = lc | (1);
	}
	return (lc);
}

void launch_cfg_diags (void)
{
	while (1) {
	if ((PINC & (1 << PINC1)) == 0) {
		LC0_HIGH;
		LC1_LOW;
	} else {
	
		LC1_HIGH;
		LC0_LOW;
	}
	delay_ms(500);
	}	
}

void set_launch_cfg_leds(launch_cfg_t lc)
{
	switch (lc) {
		case LC_ALL:
			LC0_HIGH;
			LC1_LOW;
			LC2_LOW;
			LC3_LOW;
			break;
		case LC_SLOW_STAGGER:
			LC1_HIGH;
			LC0_LOW;
			LC2_LOW;
			LC3_LOW;			
			break;
		case LC_FAST_STAGGER:
			LC2_HIGH;
			LC1_LOW;
			LC0_LOW;
			LC3_LOW;
			break;
		case LC_TWO_BY_TWO:
			LC3_HIGH;
			LC1_LOW;
			LC2_LOW;
			LC0_LOW;
			break;
	}
}

void lc_leds_diags (void)
{
	LC0_HIGH;
	LC1_HIGH;
	LC2_HIGH;
	LC3_HIGH;

	while(1) {
		
	}
}

uint8_t segments[] = {
	0x40,
	0x20,
	0x10,
	0x08,
	0x04,
	0x02,
	0x01,
	0x08,
};

void move_snake (void)
{
	static uint8_t idx = 0;
	
	if (idx >= 8) {
		idx = 0;
	}
	
	shift_reg_set(~segments[idx]);
	idx++;
}

void do_startup_blinky (void)
{
	uint32_t ms;

	move_snake();
			
	ms = 1000;
	for (int i = 0; i < 12; i++) {
		LC0_HIGH;
		delay_ms(ms);
		LC0_LOW;
		move_snake();
		LC1_HIGH;
		delay_ms(ms);
		LC1_LOW;
		move_snake();
		LC2_HIGH;
		delay_ms(ms);
		LC2_LOW;
		move_snake();
		LC3_HIGH;
		delay_ms(ms);
		LC3_LOW;
		move_snake();
		ms = ms * 0.66;
	}	
	
	for (int j = 0; j < 3; j++) {
		LC0_HIGH;
		LC1_HIGH;
		LC2_HIGH;
		LC3_HIGH;
		set_num(9);
		_delay_ms(800);
		LC0_LOW;
		LC1_LOW;
		LC2_LOW;
		LC3_LOW;
		shift_reg_set(0xFF);
		_delay_ms(800);
	}	
	
	set_num(9);
}

void port_zero_on (void)
{
	LT_PORT |= _BV(LT_0_PIN);	
}

void port_zero_off (void)
{
	LT_PORT &= ~_BV(LT_0_PIN);	
}

void port_one_on (void)
{
	LT_PORT |= _BV(LT_1_PIN);
}

void port_one_off (void)
{
	LT_PORT &= ~_BV(LT_1_PIN);	
}

void port_two_on (void)
{
	LT_PORT |= _BV(LT_2_PIN);	
}

void port_two_off (void)
{
	LT_PORT &= ~_BV(LT_2_PIN);	
}

void port_three_on (void)
{
	LT_PORT |= _BV(LT_3_PIN);	
}

void port_three_off (void)
{
	LT_PORT &= ~_BV(LT_3_PIN);	
}

void all_off (void)
{
	port_zero_off();
	port_one_off();
	port_two_off();
	port_three_off();	
}
void launch_all (void)
{
	// Send 5v out to transistor for 200ms
	port_zero_on();
	port_one_on();
	port_two_on();
	port_three_on();
//	_delay_ms(2000);
//	all_off();
}

void launch_slow_stagger (void)
{
	port_zero_on();
	delay_ms(2000);
	port_zero_off();
	delay_ms(1000);

	port_one_on();
	delay_ms(2000);
	port_one_off();
	delay_ms(1000);
	
	port_two_on();
	delay_ms(2000);
	port_two_off();
	delay_ms(1000);
	
	port_three_on();
	delay_ms(2000);
	port_three_off();
	delay_ms(1000);
	
	all_off();
}
void launch_fast_stagger (void)
{
	port_zero_on();
	delay_ms(1000);
	port_zero_off();

	port_one_on();
	delay_ms(1000);
	port_one_off();
	
	port_two_on();
	delay_ms(1000);
	port_two_off();
	
	port_three_on();
	delay_ms(1000);
	port_three_off();
	
	all_off();	
}

void launch_two_by_two (void)
{
	port_zero_on();
	port_one_on();
	delay_ms(3000);
	port_two_on();
	port_three_on();
}

void do_launch (void)
{
	switch (g_launch_cfg) {
	case LC_ALL:
		launch_all();
		break;
	case LC_SLOW_STAGGER:
		launch_slow_stagger();
		break;
	case LC_FAST_STAGGER:
		launch_fast_stagger();
		break;
	case LC_TWO_BY_TWO:
		launch_two_by_two();
		break;
	}	
}

static void seven_seg_diags (void)
{
	static int val = 0;
	
	set_num(val++);
	
	while (1) {	

		/*
		 * Is a human pressing the launch button?
		 */		 
		if ((PINC & (1 << PINC3)) == 0) {	
			LC0_HIGH;
			delay_ms(1000);
			LC0_LOW;

			if (val > 10) {
				val = 0;
			}
			set_num(val++);	
		}

		delay_ms(200);
	}
}

void launch_diags(void)
{
	int num;
	
	set_num(0);
	
	for (int i = 0; i < 3; i++) {
		LC0_HIGH;
		delay_ms(1000);
		LC0_LOW;
		delay_ms(500);
	}
	
	num = countdown(9, 900);
		
		
	g_launch_cfg = LC_ALL;
	do_launch();	
	
	while (1) {}
}

static void do_launch_program()
{
	int num = 1;
	launch_cfg_t lc;
	
	/*
	 * Control flow:
	 *    - Do the startup led blinks.
	 *    - Read the launch config switches.
	 *    - Store launch config 
	 *    - Light the launch config leds.
	 *    - Wait for launch button to be pressed
	 *    - Do countdown
	 *    - Activate launch transistors per the launch config.
	 */
		
	do_startup_blinky();
		
	g_launch_cfg = get_launch_cfg();

	set_launch_cfg_leds(g_launch_cfg);
			
	while (1) {
		/*
		 * poll for launch cfg change or launch button press
		 */
		lc = get_launch_cfg();
		set_launch_cfg_leds(lc);
		
		/*
		 * Is a human pressing the launch button?
		 */		 
		if ((PINC & (1 << PINC3)) == 0) {
			break;
		}

		delay_ms(300);
	}
	
	num = countdown(9, 900);

	if (!num) {
		do_launch();
		num = countup(0, 900);
	} else {
		blink(num, 900);
	}
}

int main (void)
{
    ioinit(); //Setup IO pins and defaults

	// seven_seg_diags();
	
	do_launch_program();
	
	// launch_diags();
	
	// launch_cfg_diags();
	
	return (0);
}

void ioinit (void)
{
    /*
	 * 1 = output, 0 = input
	 */
    DDRB = 0b11111111; 
    DDRC = 0b00000000; 
	PORTC = 0xFF;		/* Activate the internal pull up resistors */
    DDRD = 0b11111111; 
}



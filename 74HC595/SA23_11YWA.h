#ifndef __SA23_11YWA_H__
#define __SA23_11YWA_H__

typedef uint8_t bool;
#define true 1
#define false 0

/*
 * 7 Segment LED: Yellow, Common Anode
 *
 * Pin to segment mapping
 *
 * 5V : 1,5
 *
 * A : 7
 * B : 6
 * C : 4
 * D : 3
 * E : 2
 * F : 9
 * G : 10
 *
 * DP : 8
 * 
 * To use with software wire to shift register as follows
 *
 * A : 1, B : 2, C : 3, D : 4, E : 5, F : 6, G : 7, DP : 15
 *
 * LED pin : 74HC pin
 *   7     :    1
 *   6     :    2
 *   4     :    3
 *   3     :    4
 *   2     :    5
 *   9     :    6
 *   10    :    7
 *   8     :    15  (optional for decimal point)
 *
 */

typedef enum {
	SEGMENT_A = 0,
	SEGMENT_B,
	SEGMENT_C,
	SEGMENT_D,
	SEGMENT_E,
	SEGMENT_F,
	SEGMENT_G,
} led_segment_t;

typedef enum {
	SR_PIN_0 = 0,
	SR_PIN_1,
	SR_PIN_2,
	SR_PIN_3,
	SR_PIN_4,
	SR_PIN_5,
	SR_PIN_6,
	SR_PIN_7,
} shift_reg_pin_t;

typedef struct segment_pin_map_ {
	led_segment_t segment;
	shift_reg_pin_t pin;
} segment_pin_map_t;


/*
 * An array of numbers.  Each element of the array is composed
 * of an array of segments specifying whether it should be on or off
 * Note use false for segment OFF.  Software will determine whether or
 * not to send zero or one to that pin of the 7 segment.
 */

typedef struct segment_state_ {
	led_segment_t segment;
	bool on;
} segment_state_t;

void sa23_11ywa_set_num(uint8_t val);

void sa23_11ywa_set_off(void);

#endif
/**
 * Header for the servo_control file. Contains various servo defines like: Position Values, States, Defines and Operations Needed else where
 * created: 3/6/21
 * last updated: 3/9/21
 * author: Stephen Cook
 * Language: C
 */

#ifndef INC_SERVO_CONTROL_H_
#define INC_SERVO_CONTROL_H_

// Start Includes
#include <stdlib.h>

#include "stm32l4xx_hal.h"
#include "opcode.h"
#include "LED.h"


//End Includes

//Start Defines

// Defines Servos
#define SERVO_N (2)		//Number of Servos in use

// POSITION DEFINES
// Math: HCLK = 80Mhz/4 (APB1Prescaler) x2 = 40Mhz <- clock frequency before timer 2 prescaler
// Target Frequency = 50hz. FREQ = CLOCK/(Prescalaer + 1)x(ARR+1)
// 50hz = 40,000,000hz/(79+1)X(9,999+1)  (80-1) gets the Hz to a mutiple of 50.
// ARR 10,000 gives adjusts the frequency down to 50hz while also keeping the duty cycle range accurate enough

// Degree MATHS: 160/5 = 32Degree Each (POS_0=0Deg, POS_1=32Deg, POS_2=64Deg, POS_3=96Deg, POS_4=128, POS_5=160)
// Duty Cycle Maths: Range = 10%-2% = 8%. 8%/5 = 1.6% each. (POS_0=10%, POS_1=8.4%,POS_2=6.8%,POS_3=5.2%,POS_4=3.6%,POS_5=2%)
#define POS_5 (1000)	//POS 5, Extreme Clockwise Position(160 Degrees), 10% duty cycle. 20 * .10 = 2m/s. .10 * 10000(ARR) = 100
#define POS_4 (840)		//POS 4, ~128 Degrees Counter Clockwise POS 0, 8.4% duty cycle. 20 * .084 =  1.68 m/s. 0.084 * 10000 = 840
#define POS_3 (680) 	//POS 3, ~96 Degrees Counter Clockwise POS 0, 6.8% duty cycle. 20 * .068 = 1.36 m/s. 0.068 * 10000 = 680
#define POS_2 (520)		//POS 2, ~64 Degrees Counter Clockwise POS 0, 5.2% duty cycle. 20 * .052 = 1.04 m/s. 0.052 * 10000 = 520
#define POS_1 (360)		//POS 1, ~32 Degrees Counter Clockwise MILLISECONDS_PER_WAIT_VALUEycle. 20 * .036 = 0.72 m/s. 0.036 * 10000 = 36-
#define POS_0 (220) 	//POS 0, Extreme Clockwise Position(0 Degrees), 2% duty Cycle. 20 * .02 =  0.4. .02 * 10000 = 220 (USING 220, cuz 200 over stretches)

// Time defines
// TIM16 and TIM17 tick rate calculation:
// HCLK = 80/16 (APB2 Prescaler) * 2 10Mhz before TIM16 and TIM17 prescaler
// Target frequency is 1kHz (gives tickrate of 1 millisecond)
// 10Mhz/(9,999 +1) = 1 kHz, so TIM16 and TIM17 Prescaler = 10000-1
// Timers configured to have a tick rate of 1 millisecond
#define MILLISECONDS_PER_WAIT_VALUE (100)	//A wait is 1/10 of a second, so 100 milliseconds
#define MILlISECONDS_PER_MOVE (200)			//Milliseconds per servo position movement, 200 milliseconds

//End Defines

//Start Typedefs
//Servo_state, used as a sudo-state machine to indicate the servos state
typedef enum{
	state_at_position,			//Indicates Idle
	state_waiting,				//Indicates Waiting (CMD) or Waiting for move to end
	state_recipe_ended,			//Recipe Ended
	state_param_error,			//Param Error
	state_nested_loop_error,	//Nested Loop Error
	state_user_pause,			//User force pause
	state_total_end				//End of recipe list
}servo_state;


//Structure for the servo. Holds all the information a servo could need
typedef struct {
	// Identifying info
	int id;								//Servo ID
	servo_state current_state;			//State of servo
	int pos;							//Position of the servo, meaning the 0-5 pos
	int duty_cycle; 					//The duty cycle
	TIM_HandleTypeDef duty_timer;		//The timer the Servo uses for the duty cycle
	TIM_HandleTypeDef wait_timer; 					//The timer for tracking wait time

	//recipe info
	int recipe_index;					//The index of the recipe the servo is currently prefoming
	int current_recipe_index;			//The index of the command in the recipe we are doing.

	// info for waits
	uint16_t wait_time;					//The time to wait on wait command
	uint16_t wait_start_time;			//The count at the start of the wait time

	//info for loops
	int looping;						//Indicates we are currently looping (used to prevent nested loops)
	int loops;							//Number of Times we loop
	int loop_start_index;				//Index of the start of loop

	//info for fanning;
	int fanning;						//indicates fanning operation
	int fanning_count;					//the number of fans

}Servo;

//End Typedefs

//Start Functions
// Runs the operation on a servo
void operate(Servo *servo, unsigned char recipes[] );

//Initializes the servo
Servo servo_init(int id, TIM_HandleTypeDef duty_timer, TIM_HandleTypeDef wait_timer);

void pause_servo(Servo *servo);
void continue_servo(Servo *servo);
void right_servo(Servo *servo);
void left_servo(Servo *servo);
void begin_servo(Servo *servo);

//End functions

#endif /* INC_SERVO_CONTROL_H_ */

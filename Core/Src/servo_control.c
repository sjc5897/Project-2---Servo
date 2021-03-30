#include "servo_control.h"

// Cheats to assign channel and reg
unsigned char duty_channel_array[] = {TIM_CHANNEL_1, TIM_CHANNEL_2};

//Purpose:	Starts wait timer
//Input:	The servo that is being operated on
//Output:	void
void start_wait_timer(Servo *servo){
	HAL_TIM_Base_Start(&servo->wait_timer);
}
//Purpose:  Stops wait timer
//Input:	The servo that is being operated on
//Output:	void
void stop_wait_timer(Servo *servo){
	HAL_TIM_Base_Stop(&servo->wait_timer);
}

//Purpose: 	Gets the count from the wait timer
//Input:	The servo that is being operated on
//Output:	uint16_t, the timer counter
uint16_t get_wait_timer_count(Servo *servo){
	return __HAL_TIM_GET_COUNTER(&servo->wait_timer);
}

//Purpose:	Starts the wait process for the servo
//Input:	Servo and time
//Output: 	Void
void start_wait(Servo *servo, int time){
	servo->current_state = state_waiting;						//Set state to waiting
	int time_mili = time * MILLISECONDS_PER_WAIT_VALUE; 		//Wait time to milliseconds .
	servo->wait_time = time_mili;								//Set wait time in servo
	start_wait_timer(servo);								//Start wait timer
	servo->wait_start_time = get_wait_timer_count(servo);		//Get current time and set it to wait start time
}

//Purpose: 	Ends the wait
//Input:	Servo
//Output:	void
void end_wait(Servo *servo){
	stop_wait_timer(servo);					//Stop the wait timer
	servo->wait_time = 0;							//Reset wait time value
	servo->wait_start_time = 0;						//Reset wait start time
	if(servo->fanning == 0){
		servo->current_recipe_index++;					//Move to next recipe spot
	}
	servo->current_state = state_at_position;		//Indicate idle
}

//Purpose:	Checks if wait is over
//Input:	Servo
//Output:	void
void check_wait(Servo *servo){
	uint16_t current_cnt = get_wait_timer_count(servo);		//Gets current count

	// If the current count - the wait start time is greater than wait time
	if(current_cnt-servo->wait_start_time >= servo->wait_time){
		//end wait
		end_wait(servo);
	}
}

//Purpose: 	Starts the PWM Generation on the Servo's Pin
//Input:	Servo Structure
//Output:	Void
void start_pwm_gen(Servo *servo){
	HAL_TIM_PWM_Start(&servo->duty_timer, duty_channel_array[servo->id]);
}

//Purpose: 	Sets the servo position
//Input: 	Servo Struct and Desired Pos
//Output:	Void
void set_pos(Servo *servo, int pos){
	servo->current_state = state_waiting;							//Enter waiting state
	int pos_mov = servo->pos - pos;									//Get positional difference
	pos_mov = abs(pos_mov);
	uint16_t time_for_move = MILlISECONDS_PER_MOVE * pos_mov;		//Get milliseconds per move
	servo->wait_time = time_for_move;								//Set wait time

	// get pos for int pos
	switch(pos){
			case 0:
				servo->pos = 0;
				servo->duty_cycle = POS_0;
				break;
			case 1:
				servo->pos = 1;
				servo->duty_cycle = POS_1;
				break;
			case 2:
				servo->pos = 2;
				servo->duty_cycle = POS_2;
				break;
			case 3:
				servo->pos = 3;
				servo->duty_cycle = POS_3;
				break;
			case 4:
				servo->pos = 4;
				servo->duty_cycle = POS_4;
				break;
			case 5:
				servo->pos = 5;
				servo->duty_cycle = POS_5;
				break;
	}

	// Move the servo
	if(servo->id==0){
			servo->duty_timer.Instance->CCR1 = servo->duty_cycle;
	}
	else{
			servo->duty_timer.Instance->CCR2 = servo->duty_cycle;
	}

	// Start timing
	start_wait_timer(servo);
	servo->wait_start_time = get_wait_timer_count(servo);
}

//Purpose: 	Initializes a new server struct
//Input: 	Servo ID and Timer
//OutputL 	Servo Struct
Servo servo_init(int id, TIM_HandleTypeDef duty_timer, TIM_HandleTypeDef wait_timer){
	Servo servo = {
			id,
			state_user_pause,
			0,
			POS_0,
			duty_timer,
			wait_timer,
			0,0,0,0,0,0,0};
	//Start the motor and mode the motor to one
	start_pwm_gen(&servo);
	//Init pos
	set_pos(&servo,0);

	//set_pos does waits and changes state, I am just undoing this
	end_wait(&servo);
	servo.current_state = state_user_pause;
	servo.current_recipe_index -= 1;
	return servo;
}

//Purpose: 	Puts the servo in a user pause state
//Input:	Servo to pause
//Output: 	void
void pause_servo(Servo *servo){
	// Does not work at end of recipe or error state
	if(servo->current_state != state_param_error
			&& servo->current_state != state_nested_loop_error){
		servo->current_state = state_user_pause;
	}
}
//Purpose:  Continue Servo when given C command
//Input:	Servo to continue
//Output: 	void
void continue_servo(Servo *servo){
	// Does not work at end of recipe or error state
	if(servo->current_state != state_param_error
				&& servo->current_state != state_nested_loop_error){
			servo->current_state = state_at_position;
		}
}
// Purpose:	Moves the servo left one position
//Input:	Servo to continue
//Output: 	void
void left_servo(Servo *servo){
	// Only works if paused and position is not five
	if(servo->current_state == state_user_pause && servo->pos !=5 ){
		set_pos(servo, servo->pos + 1);

		// Since pause state, we don't care about the wait
		// It will harm the program if left
		end_wait(servo);
		servo->current_state = state_user_pause; //return to pause state
	}
}

// Purpose:	Moves the servo left one position
//Input:	Servo to continue
//Output: 	void
void right_servo(Servo *servo){
	// Only works if paused and position is not five
	if(servo->current_state == state_user_pause && servo->pos !=0 ){
		set_pos(servo, servo->pos - 1);
		// Since pause state, we don't care about the wait
		// It will harm the program if left
		end_wait(servo);
		servo->current_state = state_user_pause; //return to pause state
	}
}

//Purpose:	Restarts current recipe if in recipe or moves to next if at recipe end
//Input:	Servo to continue
//Output: 	void
void begin_servo(Servo *servo){
	servo->current_recipe_index = 0;
	servo->current_state = state_at_position;
	// Reset some internal values
	servo->looping = 0;
	servo->fanning = 0;
}

//Purpose:	Acts as an operational interface on servos for the main
//Input:	Target Servo and Recipe List
//Output:	Void
void operate(Servo *servo,unsigned char recipes[]){
	//Check the current state of the servo
	switch(servo->current_state){
		//If state is state at position (ie idle)
		case state_at_position: ;
			ParsedOp current_command;
			// Get current command
			if (recipes != NULL){
				current_command = parse_opcode(recipes[servo->current_recipe_index]);
			}
			else{
				servo->current_state = state_total_end;		//This ends the servo, restart is required at this point
				break;
			}
			//switch on command
			switch(current_command.command){
				//Opcode Command: MOVE
				case MOV:

					if(servo->id == 0){
						//Command Error = just red
						turn_green_led_on();
					}
					// Validate Move Param
					if(current_command.param >= ALL_MIN && current_command.param <= MOV_MAX){
						// Move the servo
						set_pos(servo,current_command.param);
					}
					// If invalid enter param error state
					else{
						servo->current_state = state_param_error;
					}
					break;

				//Opcode Command: Wait
				case WAIT:
					//Validate Wait Param
					if(current_command.param >= ALL_MIN && current_command.param <= WAIT_MAX){
						// Enter Wait
						start_wait(servo,current_command.param);
					}
					// If invalid. enter param error state
					else{
						servo->current_state = state_param_error;
					}
					break;

				//Opcode Command: Loop
				case LOOP:
					// If already looping, enter nested loop error
					if(servo->looping){
						servo->current_state = state_nested_loop_error;
					}
					// Validate Loop Param
					if(current_command.param >= ALL_MIN && current_command.param <= LOOP_MAX){
						//Set looping int to 1
						servo->looping = 1;
						//Set Loop Start index to current index
						servo->loop_start_index = servo->current_recipe_index+1;
						//set loop count to param
						servo->loops = current_command.param;
						//Go to text command
						servo->current_recipe_index+=1;
					}
					// If invalid, enter param error state
					else{
						servo->current_state = state_param_error;
					}
					break;

				//Opcode Command: End Loop
				case END_LOOP:
					// If we are in a loop
					if(servo->looping == 1){
						// Subtract one from loop count
						servo->loops-=1;

						//IF all loops done
						if(servo->loops < 0){
							//reset looping info
							servo->loops=0;
							servo->looping=0;
							servo->loop_start_index=0;
							//Move to command after looping
							servo->current_recipe_index+=1;
						}
						// otherwise continue loop
						else{
							servo->current_recipe_index=servo->loop_start_index;
						}
					}
					// If we are not looping, this is just ignored
					break;
				//  Opcode Command: Recipe End
				case RECIPE_END:
					// Sets the state to ended
					servo->current_state = state_recipe_ended;
					break;
				// Fans the servo
				case FAN:
					// Init fanning
					if(servo->fanning == 0){
						servo->fanning = 1;								//Set fanning var
						servo->fanning_count = current_command.param;	//Set times fanned
						set_pos(servo,0);								//Set position to start
					}
					// Already Fanning
					else{
						// one fan done
						if(servo->pos == 5){
							// decrement count
							servo->fanning_count -= 1;
							// If fanning count is < 0 we are done
							if(servo->fanning_count < 0){
								// reset
								servo->fanning = 0;
								servo->fanning_count = 0;
								set_pos(servo,0);					//This will now free the program
							}
							else{
								set_pos(servo,0);					//Set position to start
							}
						}
						else{
							set_pos(servo, servo->pos + 1);			//Set position to start
						}
					}

					break;

				//Unknown Opcode: Enter Param Error State
				default:
					servo->current_state = state_param_error;

			}
		break; //End idle case

		// If waiting
		case state_waiting:
			check_wait(servo);
			break;

		// If recipe ended
		case state_recipe_ended:
			// Reset current recipe index to 1
			servo->current_recipe_index =0;
			// If servo 1. turn off led
			if(servo->id == 0){
				turn_green_led_off();
			}
			break;

		// If param error state
		case state_param_error:
			if(servo->id == 0){
				//Command Error = just red
				turn_green_led_off();
				turn_red_led_on();
			}
			// TODO: Reset?
			break;

		// If state nested loop error
		case state_nested_loop_error:
			if(servo->id == 0){
				//Command Error = just red
				turn_green_led_on();
				turn_red_led_on();
			}
			// TODO: Reset?
			break;

		// If state pause
		case state_user_pause:
			// Basically do nothing
			if(servo->id == 0){
				//turn of all leds if servo 1
				turn_green_led_off();
				turn_red_led_off();
			}
			break;

		//End of recipe list
		case state_total_end:
			//Do nothing, maybe go back to start?
			if(servo->id == 0){
				//turn of all leds if servo 1
				turn_green_led_off();
				turn_red_led_off();
			}
			break;

}}



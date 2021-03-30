/**
 * Handles the control of the LEDs
 * created: 3/7/21
 * last updated: 3/9/21
 * author: Stephen Cook
 * Language: C
 */

#include "LED.h"

//Purpose: 	Toggles (changes current state) of the red LED
//Input:	None
//Output:	Void
void toggle_red_led(){
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
}
//Purpose: 	Toggles (changes current state) of the green LED
//Input:	None
//Output:	Void
void toggle_green_led(){
	HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_8);
}
//Purpose: 	Turns on the red LED
//Input:	None
//Output:	Void
void turn_red_led_on(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
}
//Purpose: 	Turns on the Green LED
//Input:	None
//Output:	Void
void turn_green_led_on(){
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
}
//Purpose: 	Turns off the red LED
//Input:	None
//Output:	Void
void turn_red_led_off(){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
}
//Purpose: 	Turns on the green LED
//Input:	None
//Output:	Void
void turn_green_led_off(){
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
}


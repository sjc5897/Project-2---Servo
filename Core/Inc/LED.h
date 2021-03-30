/**
 * Header for LED control
 * created: 3/7/21
 * last updated: 3/9/21
 * author: Stephen Cook
 * Language: C
 */

#ifndef INC_LED_H_
#define INC_LED_H_
//Start Includes

#include "stm32l4xx_hal.h"

//End Includes

//Start Defines

//End Defines

//Start Typedefs

//End Typedefs

//Start Functions

//Toggles (changes current state) of the red LED
void toggle_red_led();

//Toggles (changes current state) of the green LED
void toggle_green_led();

//Turns on the red LED
void turn_red_led_on();

//Turns on the Green LED
void turn_green_led_on();

//Turns off the red LED
void turn_red_led_off();

//Turns on the green LED
void turn_green_led_off();

//End Functions

#endif /* INC_LED_H_ */

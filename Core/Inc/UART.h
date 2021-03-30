/**
 * Header file for UART.c
 * created: 3/9/21
 * last updated: 3/10/21
 * author: Stephen Cook
 * Language: C
 */

#ifndef INC_UART_H_
#define INC_UART_H_

//Includes Start
#include "stm32l4xx_hal.h"
//Includes End

//Defines start
#define TRANSMIT_TIMEOUT (100)
#define RECIEVE_TIMEOUT (50)
//Defines end

//Functions Start
void uart_transmit(UART_HandleTypeDef *huart, uint8_t *buffer, uint32_t nBytes);
void uart_recieve(UART_HandleTypeDef *huart, uint8_t *buffer, int size);
//Function end

#endif /* INC_UART_H_ */

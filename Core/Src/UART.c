/**
 * Simple se to manage the communication to via UART
 * created: 3/9/21
 * last updated: 3/10/21
 * author: Stephen Cook
 * Language: C
 */
#include "UART.h"

//Purpose:	Writes to the UART
//Input:	UART_HandleTypeDef: Pointer to the huart we are using
//			uint8_t: Buffer that we are writing
//			uint32_t: Size of the buffer
//Output:	Void
void uart_transmit(UART_HandleTypeDef *huart, uint8_t *buffer, uint32_t nBytes){
  HAL_UART_Transmit(huart, buffer, nBytes, TRANSMIT_TIMEOUT);
}

//Purpose:	Reads the incoming value in uart
//Input:	UART_HandleTypeDef: Pointer to the huart we are using
//			uint8_t: Buffer that we are using to write to
//			int: Size of the buffer, probably 2 since we are reading one char at t time
//Output:	Void
void uart_recieve(UART_HandleTypeDef *huart, uint8_t *buffer, int size){
	HAL_UART_Receive(huart, buffer, size, RECIEVE_TIMEOUT);
}



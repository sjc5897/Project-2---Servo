/**
 * Header for the Opcodes, contains definitions, for OPCODES, MASKS and PARAMS.
 * created: 3/6/21
 * last updated: 3/9/21
 * author: Stephen Cook
 * Language: C
 */

#ifndef INC_OPCODE_H_ccc
#define INC_OPCODE_H_

// Start Includes

#include "stdint.h"

// End Includes

//Start Defines

// OPCODE FORMAT is 8 bits, first three bits are the command opcode, last 5 are the command parameter
#define MOV (0x20) 		//Move Command, Opcode 001, PARAM target position (range 0-5)
#define WAIT (0x40)		//Wait Command, Opcode 010, PARAM number of 1.10 seconds to delay (range 0-31)
#define LOOP (0x80) 	//Loop Command, Opcode 100, PARAM number of additional times to follow the recipe (0-31)
#define END_LOOP (0xA0)	//End Loop Command, Opcode 101, PARAM n/a. Marks end of loop
#define RECIPE_END (0x0)//End of Recipe
#define FAN (0xE0)		//Grad Ocode Reqirement, Opcode 111, PARAM 0-31, goes 0,1,2,3,4,5 n times

// OPCODE PARAM CONSTRAINTS
#define ALL_MIN (0)			//All commands have a min of 0 (if they have params)
#define MOV_MAX (5)			//Move's max param
#define WAIT_MAX (31)		//Wait's max param
#define LOOP_MAX (31)		//Loop's max param


//Masks
#define OP_CODE_MASK (0xE0)	//Mask to get just OP Code
#define PARAM_MASK (0x1F)	//Mask to get just Param

//End Defines

//Start Typedefs

// Typedef struct used to represent an opcode after it has been parsed.
typedef struct{
	uint8_t command;	//Opcode
	uint8_t param;		//Param
}ParsedOp;

//End Typedefs

//Start Functions

//Parses opcodes
ParsedOp parse_opcode(uint8_t code);

//End Functions


#endif /* INC_OPCODE_H_ */

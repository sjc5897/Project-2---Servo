/**
 * This file handles the parsing of Opcodes
 * created: 3/6/21
 * last updated: 3/9/21
 * author: Stephen Cook
 * Language: C
 */
#include "opcode.h"

//Purpose: Helps the parser get the opcode
//Input: The whole opcode
//Output: Just the command portion of the code
uint8_t get_opcode(uint8_t code){
	return code & OP_CODE_MASK;
}

//Purpose: Helps the parser get the opcode
//Input: The whole opcode
//Output: Just the param portion of the code
uint8_t get_param(uint8_t code){
	return code & PARAM_MASK;
}

//Purpose: Parses the Opcode for quick access
//Input: The whole opcode
//Output: A struct of a parsed opcode
ParsedOp parse_opcode(uint8_t code){
	ParsedOp c_parsed_op;
	c_parsed_op.command = get_opcode(code);
	c_parsed_op.param = get_param(code);
	return c_parsed_op;
}


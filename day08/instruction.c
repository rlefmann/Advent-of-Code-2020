#include "instruction.h"

#include "../lib/util.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>


static bool
is_valid_instruction_string(char * instr_str)
{
	if (strcmp(instr_str, "acc") == 0) {
		return true;
	} else if (strcmp(instr_str, "jmp") == 0) {
		return true;
	} else if (strcmp(instr_str, "nop") == 0) {
		return true;
	}
	return false;
}


void
instruction_init(instruction_t * ins, char * instruction, int value)
{
	if (!is_valid_instruction_string(instruction)) {
		die("Not a valid instruction: `%s`.\n", instruction);
	}
	strncpy(ins->instruction, instruction, 4);
	ins->value = value;
	ins->executed = false;
}


void
instruction_print(instruction_t * ins)
{
	printf("%s %d\n", ins->instruction, ins->value);
}

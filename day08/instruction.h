#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdbool.h>

typedef struct {
	char instruction[4];
	int value;
	bool executed;
} instruction_t;


void instruction_init(instruction_t * ins, char * instruction, int value);
void instruction_print(instruction_t * ins);

#endif

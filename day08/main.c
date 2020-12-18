#define	_POSIX_C_SOURCE 200809L

#include "../lib/util.h"
#include "instruction.h"
#define TYPE instruction_t *
#define NAME instr
#include "../lib/list.h"
#undef TYPE
#undef NAME

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


void instrlist_readfromfile(instrlist_t * lst, char * filepath);
bool execute_program_pt1(instrlist_t * instlist, int * acc);
void execute_instruction(instruction_t * ins, int * acc, int * idx);
int	fix_program(instrlist_t * instlist);
bool execute_program_pt2(instrlist_t * instlist, int * acc, int changedpos);
bool change_instruction(instruction_t * ins);
void unmark_executed(instrlist_t * instlist);


int
main(int argc, char * argv[])
{
	if (argc != 2) {
		die("Usage: %s FILE\n", argv[0]);
	}
	char * filepath = argv[1];

	instrlist_t lst;
	instrlist_init(&lst);
	instrlist_readfromfile(&lst, filepath);

	int acc = 0;
	execute_program_pt1(&lst, &acc);
	printf("pt1: %d\n", acc);
	unmark_executed(&lst);

	acc = fix_program(&lst);
	printf("pt2: %d\n", acc);

	for (int i = 0; i < lst.len; i++) {
		free(lst.items[i]);
	}
	instrlist_free(&lst);

	return EXIT_SUCCESS;
}


void
instrlist_readfromfile(instrlist_t * lst, char * filepath)
{
	FILE * fp = fopen(filepath, "r");
	if (fp == NULL) {
		die("Error opening file `%s`.\n", filepath);
	}

	char * line = NULL;
	size_t len;
	ssize_t nread;

	char instruction[4];
	int value;

	for (;;) {
		nread = getline(&line, &len, fp);
		if (nread == -1) {
			break;
		}
		sscanf(line, "%3s %d", instruction, &value);
		instruction_t * ins = malloc(sizeof(instruction_t));
		instruction_init(ins, instruction, value);
		instrlist_add(lst, ins);
	}

	fclose(fp);
}


bool
execute_program_pt1(instrlist_t * program, int * acc)
{
	*acc = 0;
	int instidx = 0;
	instruction_t * curinst;
	for (;;) {
		if (instidx < 0 || instidx > program->len) {
			return false;
		} else if (instidx == program->len) {
			printf("program executed successfully!\n");
			return true;
		}
		curinst = program->items[instidx];
		if (curinst->executed == true) {
			return false;
		}
		execute_instruction(curinst, acc, &instidx);
	}
	// never reached:
	return false;
}


void
execute_instruction(instruction_t * ins, int * acc, int * idx)
{
	if (strcmp(ins->instruction, "acc") == 0) {
		(*acc) += ins->value;
		(*idx)++;
	} else if (strcmp(ins->instruction, "jmp") == 0) {
		(*idx) += ins->value;
	} else if (strcmp(ins->instruction, "nop") == 0) {
		(*idx)++;
	} else {
		fprintf(stderr, "Invalid instruction: `%s`.\n", ins->instruction);
		exit(EXIT_FAILURE);
	}
	ins->executed = true;
}


int
fix_program(instrlist_t * program)
{
	int acc;
	bool success;
	for (int i = 0; i < program->len; i++) {
		acc = 0;
		success = execute_program_pt2(program, &acc, i);
		if (success) {
			printf("corrupted line is %d.\n", i);
			return acc;
		}
		unmark_executed(program);
	}
	fprintf(stderr, "Program could not be fixed.\n");
	exit(EXIT_FAILURE);
}


bool
execute_program_pt2(instrlist_t * program, int * acc, int changedpos)
{
	instruction_t * changeinstr = program->items[changedpos];
	bool has_changed = change_instruction(changeinstr);
	if (!has_changed) {
		return false;
	}

	bool success = execute_program_pt1(program, acc);
	change_instruction(changeinstr);
	return success;
}


bool
change_instruction(instruction_t * ins)
{
	if (strcmp(ins->instruction, "jmp") == 0) {
		strncpy(ins->instruction, "nop", 4);
		return true;
	} else if (strcmp(ins->instruction, "nop") == 0) {
		strncpy(ins->instruction, "jmp", 4);
		return true;
	}
	return false;
}


void
unmark_executed(instrlist_t * instlist)
{
	instruction_t * ins;
	for (int i = 0; i < instlist->len; i++) {
		ins = instlist->items[i];
		ins->executed = false;
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LIST_BUFSIZE 3

struct instr_t {
	char instruction[4];
	int value;
	bool executed;
};


bool
is_valid_instruction_string(char * inst)
{
	if (strcmp(inst, "acc") == 0) {
		return true;
	} else if (strcmp(inst, "jmp") == 0) {
		return true;
	} else if (strcmp(inst, "nop") == 0) {
		return true;
	}
	return false;
}


void
instr_init(struct instr_t * ins, char * instruction, int value)
{
	if (!is_valid_instruction_string(instruction)) {
		fprintf(stderr, "Not a valid instruction: `%s`.\n", instruction);
		exit(EXIT_FAILURE);
	}
	strncpy(ins->instruction, instruction, 4);
	ins->value = value;
	ins->executed = false;
}


void
instr_print(struct instr_t * ins)
{
	printf("%s %d\n", ins->instruction, ins->value);
}


struct list_t {
	size_t len;
	size_t cap;
	void ** items;
};


void
list_init(struct list_t * lst)
{
	lst->items = malloc(LIST_BUFSIZE * sizeof(void *));
	lst->len = 0;
	lst->cap = LIST_BUFSIZE;
}


// this does not free the items!
void
list_free(struct list_t * lst)
{
	free(lst->items);
	lst->len = 0;
	lst->cap = 0;
}


void
list_free_list_and_items(struct list_t * lst)
{
	for (int i = 0; i < lst->len; i++) {
		free(lst->items[i]);
	}
	list_free(lst);
}


void
list_add(struct list_t * lst, void * item)
{
	if (lst->len == lst->cap) {
		size_t cap_new = lst->cap + LIST_BUFSIZE;
		lst->items = realloc(lst->items, cap_new * sizeof(void *));
		if (lst->items == NULL) {
			fprintf(stderr, "Failed to allocate %d items.\n", cap_new);
			exit(EXIT_FAILURE);
		}
		lst->cap = cap_new;
	}
	lst->items[lst->len] = item;
	lst->len++;
}


void
exec_instruction(struct instr_t * ins, int * acc, int * idx)
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


bool
exec_program_pt1(struct list_t * instlist, int * acc)
{
	*acc = 0;
	int instidx = 0;
	struct instr_t * curinst;
	for (;;) {
		if (instidx < 0 || instidx > instlist->len) {
			return false;
		} else if (instidx == instlist->len) {
			printf("program executed successfully!\n");
			return true;
		}
		curinst = instlist->items[instidx];
		if (curinst->executed == true) {
			return false;
		}
		exec_instruction(curinst, acc, &instidx);
	}
	// never reached:
	return false;
}


void
unmark_executed(struct list_t * instlist)
{
	struct instr_t * ins;
	for (int i = 0; i < instlist->len; i++) {
		ins = instlist->items[i];
		ins->executed = false;
	}
}


bool
change_instruction(struct instr_t * ins)
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


bool
exec_program_pt2(struct list_t * instlist, int * acc, int changedpos)
{
	struct instr_t * changeinstr = instlist->items[changedpos];
	bool has_changed = change_instruction(changeinstr);
	if (!has_changed) {
		return false;
	}

	bool success = exec_program_pt1(instlist, acc);
	change_instruction(changeinstr);
	return success;
}


int
fix_program(struct list_t * instlist)
{
	int acc;
	bool success;
	for (int i = 0; i < instlist->len; i++) {
		acc = 0;
		success = exec_program_pt2(instlist, &acc, i);
		if (success) {
			printf("corrupted line is %d.\n", i);
			return acc;
		}
		unmark_executed(instlist);
	}
	fprintf(stderr, "Program could not be fixed.\n");
	exit(EXIT_FAILURE);
}



void
instructionlist_read(struct list_t * lst, char * filepath)
{
	FILE * fp = fopen(filepath, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error opening file `%s`.\n", filepath);
		exit(EXIT_FAILURE);
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
		struct instr_t * ins = malloc(sizeof(struct instr_t));
		instr_init(ins, instruction, value);
		list_add(lst, ins);
	}

	fclose(fp);
}


int
main(int argc, char * argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s FILE\n", argv[0]);
		return EXIT_FAILURE;
	}
	char * filepath = argv[1];

	struct list_t lst;
	list_init(&lst);
	instructionlist_read(&lst, filepath);

	int acc = 0;
	exec_program_pt1(&lst, &acc);
	printf("pt1: %d\n", acc);
	unmark_executed(&lst);

	acc = fix_program(&lst);
	printf("pt2: %d\n", acc);

	list_free_list_and_items(&lst);

	return EXIT_SUCCESS;
}

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


int
exec_program(struct list_t * instlist)
{
	int acc = 0;
	int instidx = 0;
	struct instr_t * curinst;
	for (;;) {
  	if (instidx < 0 || instidx >= instlist->len) {
    	break;
  	}
		curinst = instlist->items[instidx];
		if (curinst->executed == true) {
  		break;
		} else if (strcmp(curinst->instruction, "acc") == 0) {
  		acc += curinst->value;
  		instidx++;
		} else if (strcmp(curinst->instruction, "jmp") == 0) {
  		instidx += curinst->value;
		} else if (strcmp(curinst->instruction, "nop") == 0) {
  		instidx++;
		} else {
			fprintf(stderr, "Invalid instruction: `%s`.\n", curinst->instruction);
			exit(EXIT_FAILURE);
		}
		curinst->executed = true;
	}
	return acc;
}


int
main(int argc, char * argv[])
{
  if (argc != 2) {
    fprintf(stderr, "Usage: %s FILE\n", argv[0]);
    return EXIT_FAILURE;
  }
	char * filepath = argv[1];

	FILE * fp = fopen(filepath, "r");
	if (fp == NULL) {
  	fprintf(stderr, "Error opening file `%s`.\n", filepath);
  	return EXIT_FAILURE;
	}

	struct list_t lst;
	list_init(&lst);

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
		list_add(&lst, ins);
	}

	int acc = exec_program(&lst);
	printf("%d\n", acc);

	list_free_list_and_items(&lst);

	fclose(fp);

  return EXIT_SUCCESS;
}

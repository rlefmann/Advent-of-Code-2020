#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


typedef struct list_node {
	int item;
	struct list_node * next;
} list_node_t;


typedef struct {
	list_node_t * first;
	list_node_t * last;
	size_t len;
} linked_list_t;


int linked_list_pop(linked_list_t * list);

void
linked_list_init(linked_list_t * list)
{
	list->len = 0;
	list->first = NULL;
	list->last = NULL;
}


void
linked_list_free(linked_list_t * list)
{
	while (list->len > 0) {
		linked_list_pop(list);
	}
}



void
linked_list_push(linked_list_t * list, int item)
{
	list_node_t * newnode = malloc(sizeof(list_node_t));
	if (newnode == NULL) {
		fprintf(stderr, "Failed to allocate node for linked list.\n");
		exit(EXIT_FAILURE);
	}
	newnode->item = item;
	newnode->next = NULL;

	if (list->len == 0) {
		list->first = newnode;
		list->last = newnode;
	} else {
		list->last->next = newnode;
		list->last = newnode;
	}
	list->len++;
}


int
linked_list_peak(linked_list_t * list)
{
	if (list->len == 0) {
		fprintf(stderr, "Trying to access empty linked list.\n");
		exit(EXIT_FAILURE);
	}
	return list->first->item;
}


int
linked_list_pop(linked_list_t * list)
{
	if (list->len == 0) {
		fprintf(stderr, "Trying to access empty linked list.\n");
		exit(EXIT_FAILURE);
	}
	list_node_t * firstnode = list->first;

	list->first = list->first->next;
	list->len--;

	int item = firstnode->item;
	free(firstnode);
	return item;
}


void
linked_list_print(linked_list_t * list)
{
	list_node_t * node = list->first;
	while (node != NULL) {
		printf("%d ", node->item);
		node = node->next;
	}
}


void
playround(linked_list_t * deckp1, linked_list_t * deckp2)
{
	int valuep1 = linked_list_pop(deckp1);
	int valuep2 = linked_list_pop(deckp2);

	printf("Player 1 plays: %d\n", valuep1);
	printf("Player 2 plays: %d\n", valuep2);

	linked_list_t * winner;
	if (valuep1 > valuep2) {
		printf("Player 1 wins the round!\n");
		linked_list_push(deckp1, valuep1);
		linked_list_push(deckp1, valuep2);
	} else {
		printf("Player 2 wins the round!\n");
		linked_list_push(deckp2, valuep2);
		linked_list_push(deckp2, valuep1);
	}
}


long
compute_final_score(linked_list_t * deck)
{
	long score = 0;
	list_node_t * node = deck->first;
	int multiplier = deck->len;
	while (multiplier > 0) {
		score += (multiplier * node->item);
		node = node->next;
		multiplier--;
	}
	return score;
}


int
playgame(linked_list_t * deckp1, linked_list_t * deckp2)
{
	int roundnum = 1;
	for (;;) {

		if (deckp1->len == 0) {
			return compute_final_score(deckp2);
		} else if (deckp2->len == 0) {
			return compute_final_score(deckp1);
		}

		printf("-- Round %d --\n", roundnum);
		printf("Player 1's deck: ");
		linked_list_print(deckp1);
		printf("\n");
		printf("Player 2's deck: ");
		linked_list_print(deckp2);
		printf("\n");

		playround(deckp1, deckp2);
		roundnum++;
	}
}

int
main(int argc, char * argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s FILE\n", argv[0]);
		return EXIT_FAILURE;
	}
	char * filepath = argv[1];

	linked_list_t deckp1;
	linked_list_init(&deckp1);
	linked_list_t deckp2;
	linked_list_init(&deckp2);

	FILE * fp = fopen(filepath, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error opening file `%s`.\n", filepath);
		return EXIT_FAILURE;
	}

	char * line = NULL;
	size_t len;
	ssize_t nread;

	bool fillp1 = true;
	int value;
	for (;;) {
		nread = getline(&line, &len, fp);
		if (nread == -1) {
			break;
		} else if (strcmp(line, "Player 2:\n") == 0) {
			fillp1 = false;
		} else if (sscanf(line, "%d", &value) > 0) {
			if (fillp1) {
				linked_list_push(&deckp1, value);
			} else {
				linked_list_push(&deckp2, value);
			}
		}
	}

	free(line);
	fclose(fp);

	long score = playgame(&deckp1, &deckp2);
	printf("%ld\n", score);

	linked_list_free(&deckp1);	
	linked_list_free(&deckp2);

	return EXIT_SUCCESS;
}

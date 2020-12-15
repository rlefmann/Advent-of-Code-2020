#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#define BUSLIST_BUFSIZ 10

typedef struct {
	size_t len;
	size_t cap;
	int * items;
} buslist_t;


void
buslist_init(buslist_t * list)
{
	list->items = calloc(BUSLIST_BUFSIZ, sizeof(int));
	if (list->items == NULL) {
		fprintf(stderr, "Failed to allocate buslist.\n");
		exit(EXIT_FAILURE);
	}
	list->len = 0;
	list->cap = BUSLIST_BUFSIZ;
}


void
buslist_free(buslist_t * list)
{
	free(list->items);
	list->len = 0;
	list->cap = 0;
}


void
buslist_add(buslist_t * list, int bus_id)
{
	if (list->len == list->cap) {
		size_t new_cap = list->cap + BUSLIST_BUFSIZ;
		list->items = realloc(list->items, new_cap * sizeof(int));
		if (list->items == NULL) {
			fprintf(stderr, "Failed to reallocate buslist.\n");
			exit(EXIT_FAILURE);
		}
		list->cap = new_cap;
	}
	list->items[list->len] = bus_id;
	list->len++;
}


void
buslist_print(buslist_t * list)
{
	printf("bus ids: ");
	for (int i = 0; i < list->len; i++) {
		printf("%d ", list->items[i]);
	}
	printf("\n");
}


void
skip_non_numbers(FILE * fp)
{
	int c;
	for (;;) {
		c = fgetc(fp);
		if (!isspace(c) && c != ',' && c != 'x') {
			ungetc(c, fp);
			break;
		}
	}
}


int
read_next_number(FILE * fp, int * n)
{
	skip_non_numbers(fp);
	int nread = fscanf(fp, "%d", n);
	return nread;
}


int
read_earliest_departure(FILE * fp)
{
	int earliest_depature;
	int nread = read_next_number(fp, &earliest_depature);
	if (nread < 1) {
		fprintf(stderr, "Error reading earliest departure.\n");
		fclose(fp);
		return EXIT_FAILURE;
	}
	printf("Earliest departure: %d\n", earliest_depature);
	return earliest_depature;
}


void
buslist_init_from_file(buslist_t * list, FILE * fp)
{
	buslist_init(list);

	int id;
	int nread;

	for (;;) {
		nread = read_next_number(fp, &id);
		if (nread < 1) {
			break;
		}
		buslist_add(list, id);
	}
}


int
compute_waiting_time(int bus_id, int earliest_departure)
{
	return bus_id - (earliest_departure % bus_id);
}


int
find_earliest_bus(buslist_t * list, int earliest_departure)
{
	if (list->len == 0) {
		fprintf(stderr, "Error: buslist is empty.\n");
		exit(EXIT_FAILURE);
	}

	int id;
	int id_min;
	int wait;
	int wait_min = INT_MAX;

	for (int i = 0; i < list->len; i++) {
		id = list->items[i];
		wait = compute_waiting_time(id, earliest_departure);
		if (wait < wait_min) {
			wait_min = wait;
			id_min = id;
		}
	}

	return id_min;
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
		fprintf(stderr, "Failed to open file `%s`.\n", filepath);
		return EXIT_FAILURE;
	}

	int earliest = read_earliest_departure(fp);

	buslist_t list;
	buslist_init_from_file(&list, fp);

	fclose(fp);

	buslist_print(&list);

	int id_min = find_earliest_bus(&list, earliest);
	int wait_min = compute_waiting_time(id_min, earliest);

	printf("id: %d wait: %d\n", id_min, wait_min);
	printf("prod: %d\n", id_min * wait_min);

	buslist_free(&list);

	return EXIT_SUCCESS;
}

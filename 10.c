#include <stdio.h>
#include <stdlib.h>

#define LIST_BUFSIZE 10


typedef struct list {
	size_t len;
	size_t cap;
	int * items;
} list_t;


void
list_init(list_t * lst)
{
	size_t size = LIST_BUFSIZE * sizeof(int);
  lst->items = malloc(size);
  if (lst->items == NULL) {
	  fprintf(stderr, "Failed to allocate %d bytes of memory.\n", size);
	  exit(EXIT_FAILURE);
  }
  lst->len = 0;
  lst->cap = LIST_BUFSIZE;
}


void
list_free(list_t * lst)
{
	free(lst->items);
	lst->items = NULL;
	lst->len = 0;
	lst->cap = 0;
}


void
list_add(list_t * lst, int item)
{
	if (lst->len == lst->cap) {
  	size_t cap_new = lst->cap + LIST_BUFSIZE;
		lst->items = realloc(lst->items, cap_new * sizeof(int));
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
compareint(const void * a, const void * b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	}
	return 1;
}


void
list_sort(list_t * list)
{
	qsort(list->items, list->len, sizeof(int), compareint);	
}


void
count_joltage_differences(list_t * joltages, int * num_one_jolt_differences, int * num_three_jolt_differences)
{
	*num_one_jolt_differences = 0;
	*num_three_jolt_differences = 0;
	int prev_joltage = 0;
	int diff;
	for (int i = 0; i < joltages->len; i++) {
		diff = joltages->items[i] - prev_joltage;

		if (diff == 1) {
			(*num_one_jolt_differences)++;
		} else if (diff == 3) {
			(*num_three_jolt_differences)++;
		} else {
			fprintf(stderr, "Bad joltage difference: %d.\n", diff);
			exit(EXIT_FAILURE);
		}

		prev_joltage = joltages->items[i];
	}
	// the device's built-in adapter is always 3 higher
	// than the highest adapter
	(*num_three_jolt_differences)++;
}


int
main(int argc, char * argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s FILE\n", argv[0]);
		return EXIT_FAILURE;
	}
	char * filepath = argv[1];

	list_t list;
	list_init(&list);

	FILE * fp = fopen(filepath, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error opening file `%s`.\n", filepath);
		return EXIT_FAILURE;
	}

	char * line = NULL;
	size_t len;
	ssize_t nread;

	int value;
	for (;;) {
		nread = getline(&line, &len, fp);
		if (nread == -1) {
			break;
		}
		line[nread - 1] = '\0';
		nread = sscanf(line, "%d\n", &value);
		if (nread != 1) {
			fprintf(stderr, "Error processing line `%s`.\n", line);
			return EXIT_FAILURE;
		}
		list_add(&list, value);
	}

	list_sort(&list);

	int num_one_jolt_differences;
	int num_three_jolt_differences;
	count_joltage_differences(&list, &num_one_jolt_differences, &num_three_jolt_differences);

	printf("1: %d\n", num_one_jolt_differences);
	printf("3: %d\n", num_three_jolt_differences);

	printf("prod: %d\n", num_one_jolt_differences * num_three_jolt_differences);

	free(line);
	list_free(&list);

	return EXIT_SUCCESS;
}

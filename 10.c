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


void
list_read_from_file(list_t * list, char * filepath)
{
	FILE * fp = fopen(filepath, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error opening file `%s`.\n", filepath);
		exit(EXIT_FAILURE);
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
			exit(EXIT_FAILURE);
		}
		list_add(list, value);
	}
	free(line);
	fclose(fp);
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
create_joltage_list(list_t * list, char * filepath)
{
	// add charging outlet:
	list_add(list, 0);

	list_read_from_file(list, filepath);

	list_sort(list);

	// add device's built-in adapter:
	int max_joltage = list->items[list->len - 1];
	list_add(list, max_joltage + 3);
}


void
count_joltage_differences(list_t * joltages, int * num_diff_1_jolt, int * num_diff_3_jolt)
{
	*num_diff_1_jolt = 0;
	*num_diff_3_jolt = 0;
	int prev_joltage = joltages->items[0];
	int diff;
	for (int i = 1; i < joltages->len; i++) {
		diff = joltages->items[i] - prev_joltage;

		if (diff == 1) {
			(*num_diff_1_jolt)++;
		} else if (diff == 3) {
			(*num_diff_3_jolt)++;
		} else {
			fprintf(stderr, "Bad joltage difference: %d.\n", diff);
			exit(EXIT_FAILURE);
		}

		prev_joltage = joltages->items[i];
	}
}


long
count_paths(list_t * joltages)
{
	int num_adapters = joltages->len;
	long * num_ways_to_reach_adapter = calloc(num_adapters, sizeof(long));
	num_ways_to_reach_adapter[0] = 1;

	for (int i = 1; i < num_adapters; i++) {
		// find previous adapters:
		for (int j = i - 1; j >= i - 3; j--) {
			if (j < 0) {
				break;
			} else if (joltages->items[j] < joltages->items[i] - 3) {
				break;
			}
			num_ways_to_reach_adapter[i] += num_ways_to_reach_adapter[j];
		}
	}
	long num_paths = num_ways_to_reach_adapter[num_adapters - 1];
	free(num_ways_to_reach_adapter);
	return num_paths;
}


void
part1(list_t * list)
{
	int num_diff_1_jolt;
	int num_diff_3_jolt;
	count_joltage_differences(list, &num_diff_1_jolt, &num_diff_3_jolt);
	printf("1:\t%d\n", num_diff_1_jolt);
	printf("3:\t%d\n", num_diff_3_jolt);
	printf("prod:\t%d\n", num_diff_1_jolt * num_diff_3_jolt);
}


void
part2(list_t * list)
{
	long num_paths = count_paths(list);
	printf("paths:\t%ld\n", num_paths);
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
	create_joltage_list(&list, filepath);

	part1(&list);
	part2(&list);

	list_free(&list);

	return EXIT_SUCCESS;
}

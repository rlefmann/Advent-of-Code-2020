#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


struct queue_t {
	int * items;
	size_t len;
	size_t cap;
};


void
queue_init(struct queue_t * q, size_t cap)
{
	q->items = malloc(cap * sizeof(int));
	q->len = 0;
	q->cap = cap;
}


void
queue_free(struct queue_t * q)
{
	free(q->items);
	q->len = 0;
	q->cap = 0;
}


void
queue_add(struct queue_t * q, int item)
{
	if (q->len == q->cap) {
		for (int i = 1; i < q->len; i++) {
			q->items[i-1] = q->items[i];
		}
		q->items[q->len - 1] = item;
	} else {
		q->items[q->len] = item;
		q->len++;
	}
}


void
queue_print(struct queue_t * q)
{
	for (int i = 0; i < q->len; i++) {
		printf("%d ", q->items[i]);
	}
	printf("\n");
}


bool
is_sum_of_any_two(struct queue_t * q, int val)
{
	int sum;
	for (int i = 0; i < q->len; i++) {
		for (int j = i + 1; j < q->len; j++) {
			sum = q->items[i] + q->items[j];
			if (sum == val) {
				return true;
			}
		}
	}
	return false;
}


int
readxmas(char * filepath, size_t n)
{
	struct queue_t previous_numbers;
	queue_init(&previous_numbers, n);

	FILE * fp = fopen(filepath, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error opening file `%s`.\n", filepath);
		exit(EXIT_FAILURE);
	}

	char * line;
	size_t len;
	ssize_t nread;

	int line_number = 0;
	int value;
	for (;;) {
		nread = getline(&line, &len, fp);
		if (nread == -1) {
			fprintf(stderr, "Reached end of file.\n");
			exit(EXIT_FAILURE);
		}

		nread = sscanf(line, "%d\n", &value);
		if (nread != 1) {
			fprintf(stderr, "Error processing line `%s`.\n", line);
			exit(EXIT_FAILURE);
		}

		if (line_number < n) {
			queue_add(&previous_numbers, value);
		} else {
			if (is_sum_of_any_two(&previous_numbers, value)) {
				queue_add(&previous_numbers, value);
			} else {
				break;
			}
		}
		line_number++;
	}
	free(line);
	fclose(fp);
	queue_free(&previous_numbers);
	return value;
}


int
main(int argc, char * argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s FILE\n", argv[0]);
		return EXIT_FAILURE;
	}

	char * filepath = argv[1];
	int res = readxmas(filepath, 25);
	printf("%d\n", res);

	return EXIT_SUCCESS;
}

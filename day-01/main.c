#define  _POSIX_C_SOURCE 200809L

#include "../lib/listint.h"
#include "../lib/util.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


void
intlist_readfile(intlist_t * list, char * filepath)
{
	FILE * fp = fopen(filepath, "r");
	if (fp == NULL) {
		die("Error opening file `%s`.\n", filepath);
	}

	char * line = NULL;
	size_t len;
	ssize_t nread;

	while ((nread = getline(&line, &len, fp)) != -1) {
		intlist_add(list, atoi(line));
	}
	free(line);
	fclose(fp);
}


int
multiply_2_numbers_that_add_to_2020(intlist_t * list)
{
	for (int i = 0; i < list->len; i++) {
		for (int j = i+1; j < list->len; j++) {
			if (list->items[i] + list->items[j] == 2020) {
				return list->items[i] * list->items[j];
			}
		}
	}
	return -1;
}


int
multiply_3_numbers_that_add_to_2020(intlist_t * list)
{
	for (int i = 0; i < list->len; i++) {
		for (int j = i+1; j < list->len; j++) {
			for (int k = j + 1; k < list->len; k++) {
				if (list->items[i] + list->items[j] + list->items[k] == 2020) {
					return list->items[i] * list->items[j] * list->items[k];
				}
			}
		}
	}
	return -1;
}


int
main(int argc, char * argv[])
{
	if (argc != 2) {
		die("Usage %s FILEPATH\n", argv[0]);
	}
	char * filepath = argv[1];

	intlist_t list;
	intlist_init(&list);
	intlist_readfile(&list, filepath);

	printf("%d\n", multiply_2_numbers_that_add_to_2020(&list));
	printf("%d\n", multiply_3_numbers_that_add_to_2020(&list));

	intlist_free(&list);

	return EXIT_SUCCESS;
}

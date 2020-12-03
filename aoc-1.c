#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 10

struct dynamic_array {
	int * buf;
	size_t cap;
	size_t len;
};


void
dynamic_array_init(struct dynamic_array * da)
{
	da->buf = malloc(BUFSIZE * sizeof(int));
	da->cap = BUFSIZE;
	da->len = 0;
}


void
dynamic_array_resize(struct dynamic_array * da)
{
  da->buf = realloc(da->buf, (da->cap + BUFSIZE) * sizeof(int));
  da->cap += BUFSIZE;
}


void
dynamic_array_add(struct dynamic_array * da, int entry)
{
  if (da->len >= da->cap) {
    da->buf = realloc(da->buf, (da->cap + BUFSIZE) * sizeof(int));
    da->cap += BUFSIZE;
  }
  da->buf[da->len] = entry;
  da->len++;
}


void
dynamic_array_readfile(struct dynamic_array * da, char * filepath)
{
	FILE * fp = fopen(filepath, "r");
	if (fp == NULL) {
  	fprintf(stderr, "Error opening file `%s`.\n", filepath);
  	exit(EXIT_FAILURE);
	}
	char * line = NULL;
	size_t len;
	ssize_t nread;
	while ((nread = getline(&line, &len, fp)) != -1) {
  	dynamic_array_add(da, atoi(line));
	}
	free(line);
	fclose(fp);
}


int
multiply_2_numbers_that_add_to_2020(struct dynamic_array * da)
{
	for (int i = 0; i < da->len; i++) {
  	for (int j = i+1; j < da->len; j++) {
    	if (da->buf[i] + da->buf[j] == 2020) {
      	return da->buf[i] * da->buf[j];
    	}
  	}
	}
	return -1;
}


int
multiply_3_numbers_that_add_to_2020(struct dynamic_array * da)
{
	for (int i = 0; i < da->len; i++) {
  	for (int j = i+1; j < da->len; j++) {
    	for (int k = j + 1; k < da->len; k++) {
      	if (da->buf[i] + da->buf[j] + da->buf[k] == 2020) {
        	return da->buf[i] * da->buf[j] * da->buf[k];
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
    fprintf(stderr, "Usage %s FILEPATH\n", argv[0]);
    return EXIT_FAILURE;
  }
	char * filepath = argv[1];
	struct dynamic_array da;
	dynamic_array_init(&da);
	dynamic_array_readfile(&da, filepath);
	printf("%d\n", multiply_2_numbers_that_add_to_2020(&da));
	printf("%d\n", multiply_3_numbers_that_add_to_2020(&da));

	return EXIT_SUCCESS;
}

#define  _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define STRLEN_MAX 1024

struct pwd_t {
	char str[STRLEN_MAX];
	char restricted_letter;
	int min;	// first pos for part two
	int max;	// second pos for part two
} pwd;


bool
is_valid_password_pt1(struct pwd_t * pwd)
{
	int cnt = 0;
	size_t pwdlen = strlen(pwd->str);
	for (int i = 0; i < pwdlen; i++) {
		if (pwd->str[i] == pwd->restricted_letter) {
			cnt++;
		}
	}
	return (cnt >= pwd->min && cnt <= pwd->max);
}


bool
is_valid_password_pt2(struct pwd_t * pwd)
{
	char pos1 = pwd->str[pwd->min - 1];
	char pos2 = pwd->str[pwd->max - 1];
	char c = pwd->restricted_letter;
	return ((pos1 == c && pos2 != c) || (pos1 != c && pos2 == c));
}


void
read_password(struct pwd_t * pwd, char * line)
{
	int min;
	int max;
	char letter;
	char str[1024];
	int num_items = sscanf(line, "%d-%d %c:	%1024s", &(pwd->min), &(pwd->max), &(pwd->restricted_letter), &pwd->str);
	if (num_items != 4) {
		fprintf(stderr, "Error splitting line `%s`\n", line);
		exit(EXIT_FAILURE);
	}
}


int
count_valid_passwords(char * filepath, bool (*is_valid_password)(struct pwd_t *))
{
	FILE * fp = fopen(filepath, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error opening file `%s`.\n", filepath);
		exit(EXIT_FAILURE);
	}
	char * line = NULL;
	size_t len;
	ssize_t nread;
	size_t cnt = 0;
	while ((nread = getline(&line, &len, fp)) != -1) {
		read_password(&pwd, line);
		if (is_valid_password(&pwd)) {
			cnt++;
		}
	}
	free(line);
	fclose(fp);
	return cnt;
}



int
main(int argc, char * argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage %s FILEPATH\n", argv[0]);
		return EXIT_FAILURE;
	}
	char * filepath = argv[1];

	int cnt_pt1 = count_valid_passwords(filepath, &is_valid_password_pt1);
	printf("%d\n", cnt_pt1);

	int cnt_pt2 = count_valid_passwords(filepath, &is_valid_password_pt2);
	printf("%d\n", cnt_pt2);

	return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define NUM_QUESTIONS 26


struct counter_t {
	int answers[NUM_QUESTIONS];
	int cnt;
	int groupsize;
};


void
counter_init(struct counter_t * yes)
{
	for (int i = 0; i < NUM_QUESTIONS; i++) {
		yes->answers[i] = 0;
	}
	yes->cnt = 0;
	yes->groupsize = 0;
}


void
counter_add_answer(struct counter_t * yes, char qletter)
{
	int idx = qletter - 'a';
	if (idx < 0 || idx >= NUM_QUESTIONS) {
		fprintf(stderr, "Invalid question letter `%c`.\n", qletter);
		exit(EXIT_FAILURE);
	}
	if (yes->answers[idx] == 0) {
		yes->cnt++;
	}
	yes->answers[idx]++;
}


void
counter_add_person(struct counter_t * yes, char * answers)
{
	for (int i = 0; i < strlen(answers); i++) {
		counter_add_answer(yes, answers[i]);
	}
	yes->groupsize++;
}


int
counter_cnt_anyone(struct counter_t * yes)
{
	return yes->cnt;
}


int
counter_cnt_everyone(struct counter_t * yes)
{
	int cnt = 0;
	for (int i = 0; i < NUM_QUESTIONS; i++) {
		if (yes->answers[i] >= yes->groupsize) {
			cnt++;
		}
	}
	return cnt;
}


bool
isemptyline(char * line)
{
	for (int i = 0; i < strlen(line); i++) {
		if (!isspace(line[i])) {
			return false;
		} 
	}
	return true;
}


void
count_yes_answers(char * filepath)
{
	int cnt_anyone = 0;
	int cnt_everyone = 0;

	struct counter_t yes;
	counter_init(&yes);

	FILE * fp = fopen(filepath, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error opening file `s`.\n", filepath);
		exit(EXIT_FAILURE);
	}

	char * line = NULL;
	size_t len;
	ssize_t nread;
	for (;;) {
		nread = getline(&line, &len, fp);

		// process end of group:
		if (nread == -1 || isemptyline(line)) {
			cnt_anyone += counter_cnt_anyone(&yes);
			cnt_everyone += counter_cnt_everyone(&yes);
			if (nread == -1) {
				break;
			}
			counter_init(&yes);
			continue;
		}

		// process line:
		if (line[nread - 1] == '\n') {
			line[nread - 1] = '\0';
		}
		counter_add_person(&yes, line);
	}
	printf("pt1 - anyone:	 %d\n", cnt_anyone);
	printf("pt2 - everyone: %d\n", cnt_everyone);
	free(line);
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
	count_yes_answers(filepath);
	return EXIT_SUCCESS;
}

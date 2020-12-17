#define  _POSIX_C_SOURCE 200809L

#define TYPE struct rule_t *
#define NAME rule
#include "lib/list.h"
#undef TYPE
#undef NAME

#include "lib/liststring.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


struct rule_t {
	char * bag_color_outer;
	char * bag_color_inner;
	int num;
};


void
rule_init(struct rule_t * rule, char * bag_color_outer, char * bag_color_inner, int num)
{
	size_t l = strlen(bag_color_outer) + 1;
	rule->bag_color_outer = malloc(l * sizeof(char));
	strcpy(rule->bag_color_outer, bag_color_outer);
	l = strlen(bag_color_inner) + 1;
	rule->bag_color_inner = malloc(l * sizeof(char));
	strcpy(rule->bag_color_inner, bag_color_inner);
	rule->num = num;
}


void
rule_free(struct rule_t * rule)
{
	free(rule->bag_color_outer);
	free(rule->bag_color_inner);
	rule->num = 0;
}


void
rule_print(struct rule_t * rule)
{
	printf("`%s` \t %d `%s`\n", rule->bag_color_outer, rule->num, rule->bag_color_inner);
}


bool
stringlist_contains(stringlist_t * lst, char * color)
{
	for (int i = 0; i < lst->len; i++) {
		if (strcmp(lst->items[i], color) == 0) {
			return true;
		}
	}
	return false;
}


void
rulelist_print(rulelist_t * lst)
{
	printf("---\n");
	for (int i = 0; i < lst->len; i++) {
		rule_print(lst->items[i]);
	}
	printf("---\n");
}


// insert a null terminator at the beginning position of the substring substr
// This function returns a pointer to the remaining part of the input string
// which follows after the first occurence of the substring.
// The function returns NULL, if the substring is not found.
// After execution, the input pointer str contains the part of the string
// before the first occurence of substr.
char *
split_before_after_first_occurence(char * str, char * substr)
{
	char * beginning_of_substr = strstr(str, substr);
	if (beginning_of_substr == NULL) {
		return NULL;
	}
	beginning_of_substr[0] = '\0';
	return beginning_of_substr + strlen(substr);
}


char *
skip_whitespace_and_commas(char * str)
{
	for (;;) {
		if (isspace(str[0]) || str[0] == ',') {
			str++;
		} else {
			break;
		}
	}
	return str;
}


// reads a line from the beginning of string str
char *
read_num(char * str, int * n)
{
	int nread = sscanf(str, "%d", n);
	if (nread != 1) {
		fprintf(stderr, "Expected a number.\n");
		exit(EXIT_FAILURE);
	}
	for (;;) {
		if (isdigit(str[0])) {
			str++;
		} else {
			break;
		}
	}
	return str;
}


// after execution lineptr contains the outer color
// The function returns the remainder of the string after "contain ".
char *
find_outer_color(char * lineptr)
{
		char * substr = " bags contain ";
		char * after = split_before_after_first_occurence(lineptr, substr);
		if (after == NULL) {
			fprintf(stderr, "Error finding substr `%s` in `%s`.\n", substr, lineptr);
			exit(EXIT_FAILURE);
		}
		return after;
}


void
rulelist_fill(rulelist_t * lst, char * filepath)
{
	FILE * fp = fopen(filepath, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error opening file `%s`.\n", filepath);
		exit(EXIT_FAILURE);
	}

	char * line = NULL;
	size_t len;
	ssize_t nread;
	for (;;) {
		nread = getline(&line, &len, fp);
		if (nread == -1) {
			break;
		}
		char * lineptr = line;

		char * after = find_outer_color(lineptr);
		char * outer_color = lineptr;
		lineptr = after;

		char * inner_color = NULL;
		int num;
		for (;;) {
			lineptr = skip_whitespace_and_commas(lineptr);
			if (lineptr[0] == '.' || isalpha(lineptr[0])) {
				break;
			}
			printf("	%s", lineptr);
			lineptr =	read_num(lineptr, &num);
			lineptr = skip_whitespace_and_commas(lineptr);
			after = split_before_after_first_occurence(lineptr, " bag");
			if (after == NULL) {
				break;
			} else if (after[0] == 's') {
				after++;
			}
			inner_color = lineptr;
			lineptr = after;

			struct rule_t * rule = malloc(sizeof(struct rule_t));
			rule_init(rule, outer_color, inner_color, num);
			rulelist_add(lst, rule);
		}
	}
	free(line);
	fclose(fp);
}


int
count_bags_that_can_contain_bag_of_color(rulelist_t * rulelst, char * color)
{
	stringlist_t colorlst;
	stringlist_init(&colorlst);
	stringlist_add(&colorlst, color);
	for (;;) {
		size_t len_old = colorlst.len;

		char * curcolor;
		struct rule_t * currule;
		for (int i = 0; i < len_old; i++) {
			curcolor = colorlst.items[i];
			for (int j = 0; j < rulelst->len; j++) {
				currule = rulelst->items[j];
				if (strcmp(currule->bag_color_inner, curcolor) == 0) {
						printf("%s matches\n", curcolor);
						char * outer_color = currule->bag_color_outer;
						if (!stringlist_contains(&colorlst, outer_color)) {
							stringlist_add(&colorlst, outer_color);
							printf("Added %s!\n", outer_color);
						}
				}
			}
		}

		if (colorlst.len == len_old) {
			break;
		}
	}
	size_t num_colors = colorlst.len;
	stringlist_free(&colorlst);
	return num_colors - 1;
}


int
main(int argc, char * argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s RULEFILE\n", argv[0]);
		return EXIT_FAILURE;
	}

	char * filepath = argv[1];

	rulelist_t lst;
	rulelist_init(&lst);

	rulelist_fill(&lst, filepath);

	printf("---\n");
	for (int i = 0; i < lst.len; i++) {
		rule_print(lst.items[i]);
	}
	printf("---\n");

	int cnt = count_bags_that_can_contain_bag_of_color(&lst, "shiny gold");
	printf("%d\n", cnt);

	for (int i = 0; i < lst.len; i++) {
  	free(lst.items[i]);
	}
	rulelist_free(&lst);

	return EXIT_SUCCESS;
}

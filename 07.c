#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define LIST_BUFSIZE 3
#define RULELIST_BUFSIZE 3
#define COLORLIST_BUFSIZE 3

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


struct list_t {
	size_t len;
	size_t cap;
	void ** items;
};


void
list_init(struct list_t * lst)
{
  lst->items = malloc(LIST_BUFSIZE * sizeof(void *));
  lst->len = 0;
  lst->cap = LIST_BUFSIZE;
}


// this does not free the items!
void
list_free(struct list_t * lst)
{
	free(lst->items);
	lst->len = 0;
	lst->cap = 0;
}


void
list_free_list_and_items(struct list_t * lst)
{
	for (int i = 0; i < lst->len; i++) {
  	free(lst->items[i]);
	}
	list_free(lst);
}


void
list_add(struct list_t * lst, void * item)
{
	if (lst->len == lst->cap) {
  	printf("resizing...\n");
  	size_t cap_new = lst->cap + LIST_BUFSIZE;
		lst->items = realloc(lst->items, cap_new * sizeof(void *));
		if (lst->items == NULL) {
  		fprintf(stderr, "Failed to allocate %d items.\n", cap_new);
  		exit(EXIT_FAILURE);
		}
		lst->cap = cap_new;
	}
	lst->items[lst->len] = item;
	lst->len++;
}


bool
list_contains(struct list_t * lst, char * color)
{
	for (int i = 0; i < lst->len; i++) {
  	if (strcmp(lst->items[i], color) == 0) {
    	return true;
  	}
	}
	return false;
}


struct rulelist_t {
  size_t len;
  size_t cap;
  struct rule_t * items;
};


void
rulelist_init(struct rulelist_t * lst)
{
	lst->items = malloc(RULELIST_BUFSIZE * sizeof(struct rule_t));
	lst->len = 0;
	lst->cap = RULELIST_BUFSIZE;
}


void
rulelist_add(struct rulelist_t * lst, char * bag_color_outer, char * bag_color_inner, int num)
{
	if (lst->len == lst->cap) {
  	printf("resizing...\n");
  	size_t cap_new = lst->cap + RULELIST_BUFSIZE;
		lst->items = realloc(lst->items, cap_new * sizeof(struct rule_t));
		lst->cap = cap_new;
	}
	rule_init(&lst->items[lst->len], bag_color_outer, bag_color_inner, num);
	lst->len++;
}


void
rulelist_free(struct rulelist_t * lst)
{
	for (int i = 0; i < lst->len; i++) {
		rule_free(&lst->items[i]);
	}
	free(lst->items);
	lst->len = 0;
	lst->cap = 0;
}


void
rulelist_print(struct rulelist_t * lst)
{
	printf("---\n");
	for (int i = 0; i < lst->len; i++) {
  	rule_print(&lst->items[i]);
	}
	printf("---\n");
}


// insert a null terminator at the beginning position of the substring substr
// This function returns a pointer to the remaining part of the input string
// which follows after the first occurence of the substring.
// THe function returns NULL, if the substring is not found.
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
rulelist_fill(struct list_t * lst, char * filepath)
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
  		printf("  %s", lineptr);
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
			list_add(lst, rule);
    	//rulelist_add(lst, outer_color, inner_color);
		}
	}
	free(line);
	fclose(fp);
}


int
count_bags_that_can_contain_bag_of_color(struct list_t * rulelst, char * color)
{
  struct list_t colorlst;
  list_init(&colorlst);
  list_add(&colorlst, color);
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
						if (!list_contains(&colorlst, outer_color)) {
							list_add(&colorlst, outer_color);
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
	list_free(&colorlst);
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

	struct list_t lst;
	list_init(&lst);

	rulelist_fill(&lst, filepath);

	printf("---\n");
	for (int i = 0; i < lst.len; i++) {
  	rule_print(lst.items[i]);
	}
	printf("---\n");

	int cnt = count_bags_that_can_contain_bag_of_color(&lst, "shiny gold");
	printf("%d\n", cnt);

	list_free_list_and_items(&lst);

  return EXIT_SUCCESS;
}

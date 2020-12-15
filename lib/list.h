#include "alloc.h"

#include <stdlib.h>

#ifndef LIST_BUFSIZE
#define LIST_BUFSIZE 10
#endif

#define TOKENPASTE(x, y) x ## y

#define LIST(NAME) TOKENPASTE(NAME, list_t)

#define LIST_INIT(NAME) TOKENPASTE(NAME, list_init)
#define LIST_FREE(NAME) TOKENPASTE(NAME, list_free)
#define LIST_ADD(NAME)  TOKENPASTE(NAME, list_add)


typedef struct {
	size_t len;
	size_t cap;
	TYPE * items;
} LIST(NAME);


void
LIST_INIT(NAME)(LIST(NAME) * list)
{
	size_t size = LIST_BUFSIZE * sizeof(TYPE);
	list->items = xmalloc(size);
	list->len = 0;
	list->cap = LIST_BUFSIZE;
}


void
LIST_FREE(NAME)(LIST(NAME) * list)
{
	free(list->items);
	list->len = 0;
	list->cap = 0;
}


void
LIST_ADD(NAME)(LIST(NAME) * list, TYPE item)
{
	if (list->len == list->cap) {
		size_t newcap = list->cap + LIST_BUFSIZE;
		size_t newsize = newcap * sizeof(TYPE);
		list->items = xrealloc(list->items, newsize);
		list->cap = newcap;
	}
	list->items[list->len] = item;
	list->len++;
}

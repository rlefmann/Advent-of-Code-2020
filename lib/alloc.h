#ifndef ALLOC_H
#define ALLOC_H

#include <stddef.h>

void * xmalloc(size_t size);
void * xcalloc(size_t nmemb, size_t size);
void * xrealloc(void * ptr, size_t size);

#endif

#include "alloc.h"

#include "util.h"

#include <stdio.h>
#include <stdlib.h>


void *
xmalloc(size_t size)
{
	void * ret = malloc(size);
	if (ret == NULL && size > 0) {
		die("Failed to allocate %u bytes of memory.\n", size);
	}
	return ret;
}


void *
xcalloc(size_t nmemb, size_t size)
{
	void * ret = calloc(nmemb, size);
	if (ret == NULL && nmemb > 0 && size > 0) {
		die("Failed to allocate %u items of size %u bytes.\n", nmemb, size);
	}
	return ret;
}


void *
xrealloc(void * ptr, size_t size)
{
	void * ret = realloc(ptr, size);
	if (ret == NULL && size > 0) {
		die("Failed to allocate %u bytes of memory.\n", size);
	}
	return ret;
}

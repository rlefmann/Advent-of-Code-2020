#include "alloc.h"

#include <stdlib.h>

#define TOKENPASTE(x, y) x ## y

#define MATRIX(NAME) TOKENPASTE(NAME, matrix_t)
#define MATRIX_INIT(NAME) TOKENPASTE(NAME, matrix_init)
#define MATRIX_FREE(NAME) TOKENPASTE(NAME, matrix_free)


typedef struct {
	size_t width;
	size_t height;
	TYPE ** items;
} MATRIX(NAME);


void
MATRIX_INIT(NAME)(MATRIX(NAME) * matrix, size_t width, size_t height)
{
	matrix->items = xcalloc(height, sizeof(TYPE *));
	for (int i = 0; i < height; i++) {
		matrix->items[i] = xcalloc(width, sizeof(TYPE));
	}
	matrix->width = width;
	matrix->height = height;
}


void
MATRIX_FREE(NAME)(MATRIX(NAME) * matrix)
{
	for (int i = 0; i < matrix->height; i++) {
		free(matrix->items[i]);
	}
	free(matrix->items);
	matrix->width = 0;
	matrix->height = 0;
}

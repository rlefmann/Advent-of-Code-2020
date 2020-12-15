#define  _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct map_t {
	int ** data;
	size_t width;
	size_t height;
} map;


void
map_init(struct map_t * map, size_t width, size_t height)
{
	map->width = width;
	map->height = height;
	map->data = malloc(height * sizeof(int *));
	for (int i = 0; i < height; i++) {
		map->data[i] = malloc(width * sizeof(int));
	}
}


void
map_free(struct map_t * map)
{
	for (int i = 0; i < map->height; i++) {
		free(map->data[i]);
	}
	free(map->data);
}


void
map_read_from_file(struct map_t * map, char * filepath)
{
	FILE * fp = fopen(filepath, "r");

	// get width and height:
	char * line = NULL;
	size_t bufsize;

	ssize_t len = getline(&line, &bufsize, fp);
	size_t width = strlen(line) - 1;
	size_t height = 1;
	for (;;) {
		len = getline(&line, &bufsize, fp);
		if (len == -1) {
			break;
		}
		height += 1;
	}
	rewind(fp);

	map_init(map, width, height);

	line = NULL;
	int row = 0;
	for (;;) {
		len = getline(&line, &bufsize, fp);
		if (len == -1) {
			break;
		}
		for (int i = 0; i < width; i++) {
			if (line[i] == '.') {
				map->data[row][i] = 0;
			} else if (line[i] == '#') {
				map->data[row][i] = 1;
			} else {
				fprintf(stderr, "Invalid symbol `%c`.\n", line[i]);
				exit(EXIT_FAILURE);
			}
		}
		row++;
	}

	fclose(fp);
}


void
map_print(struct map_t * map)
{
	for (int i = 0; i < map->height; i++) {
		for (int j = 0; j < map->width; j++) {
			printf("%d", map->data[i][j]);
		}
		printf("\n");
	}
}


int
map_getfield(struct map_t * map, int vpos, int hpos)
{
	if (vpos < 0 || vpos >= map->height || hpos < 0) {
		fprintf(stderr, "Error: invalid position (%d, %d).\n", vpos, hpos);
		exit(EXIT_FAILURE);
	}
	hpos = hpos % map->width;
	return map->data[vpos][hpos];
}


int
count_trees_encountered(struct map_t * map, int vslope, int hslope)
{
	if (vslope != 1 && hslope != 1) {
		fprintf(stderr, "Error: one of the slope parameters has to be 1.\n");
		exit(EXIT_FAILURE);
	} else if (vslope < 1 || hslope < 1) {
		fprintf(stderr, "Error: slope parameters must be >= 1.\n");
		exit(EXIT_FAILURE);
	}
	int vpos = 0;
	int hpos = 0;
	int cnt = 0;
	for (;;) {
		vpos += vslope;
		hpos += hslope;
		if (vpos >= map->height) {
			break;
		} else if (map_getfield(map, vpos, hpos) == 1) {
			cnt++;
		}
	}
	return cnt;
}


long
prod_of_trees_encountered
(struct map_t * map, int * vslopes, int * hslopes, int num_slopes)
{
	long prod = 1;
	int cnt;
	for (int i = 0; i < num_slopes; i++) {
		cnt = count_trees_encountered(map, vslopes[i], hslopes[i]);
		prod *= (long)cnt;
	}
	return prod;
}


int
main(int argc, char * argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s MAPFILE\n", argv[0]);
		return EXIT_FAILURE;
	}
	char * filepath = argv[1];
	map_read_from_file(&map, filepath);
	int cnt = count_trees_encountered(&map, 1, 3);
	printf("pt1: %d\n", cnt);
	int vslopes[] = {1, 1, 1, 1, 2};
	int hslopes[] = {1, 3, 5, 7, 1};
	long prod = prod_of_trees_encountered(&map, vslopes, hslopes, 5);
	printf("pt2: %ld\n", prod);
	map_free(&map);
	return EXIT_SUCCESS;
}

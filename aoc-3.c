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


void readmap(char * filepath);
int count_trees_encountered(int slope_x, int slope_y);

void
readmap(char * filepath)
{
  FILE * fp = fopen(filepath, "r");

  // get width and height:
  char * line = NULL;
  size_t bufsize;

  ssize_t len = getline(&line, &bufsize, fp);
  size_t width = strlen(line) - 1;
  size_t height = 1;
  printf("w: %d\n", width);
  for (;;) {
		len = getline(&line, &bufsize, fp);
		if (len == -1) {
  		break;
		}
		height += 1;
  }
  printf("h: %d\n", height);
  rewind(fp);
	struct map_t map;
	map_init(&map, width, height);

	line = NULL;
	int row = 0;
	for (;;) {
		len = getline(&line, &bufsize, fp);
		if (len == -1) {
  		break;
		}
		for (int i = 0; i < width; i++) {
  		if (line[i] == '.') {
    		map.data[row][i] = 0;
  		} else if (line[i] == '#') {
    		map.data[row][i] = 1;
  		} else {
    		fprintf(stderr, "Invalid symbol `%c`.\n", line[i]);
    		exit(EXIT_FAILURE);
  		}
		}
		row++;
	}

	for (int i = 0; i < height; i++) {
  	for (int j = 0; j < width; j++) {
    	printf("%d", map.data[i][j]);
  	}
  	printf("\n");
	}

	map_free(&map);
  fclose(fp);
}

int
main(int argc, char * argv[])
{
  if (argc != 2) {
    fprintf(stderr, "Usage: %s MAPFILE\n", argv[0]);
    return EXIT_FAILURE;
  }
  char * filepath = argv[1];
  readmap(filepath);
	return EXIT_SUCCESS;
}

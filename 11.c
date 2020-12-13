#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define THRESHOLD_PT1 4
#define THRESHOLD_PT2 5

typedef enum {
	FLOOR, SEAT_EMPTY, SEAT_OCCUPIED
} fieldtype_e;


typedef struct {
	size_t width;
	size_t height;
	fieldtype_e ** items;
} map_t;


void
map_init(map_t * map, size_t width, size_t height)
{
	map->width = width;
	map->height = height;
	map->items = malloc(height * sizeof(fieldtype_e *));
	for (int i = 0; i < height; i++) {
		map->items[i] = malloc(width * sizeof(fieldtype_e));
	}
}


void
map_free(map_t * map)
{
	for (int i = 0; i < map->height; i++) {
		free(map->items[i]);
	}
	free(map->items);
	map->width = 0;
	map->height = 0;
}


void
map_print(map_t * map)
{
	int j;
	fieldtype_e ft;
	for (int i = 0; i < map->height; i++) {
		for (j = 0; j < map->width; j++) {
			ft = map->items[i][j];
			if (ft == FLOOR) {
				putchar('.');
			} else if (ft == SEAT_EMPTY) {
				putchar('L');
			} else {
				putchar('#');
			}
		}
		putchar('\n');
	}
	putchar('\n');
}


void
get_width_and_height(FILE * fp, size_t * width, size_t * height)
{
	*width = 0;
	*height = 0;

	char * line = NULL;
	size_t len;
	ssize_t nread;

	size_t nlines = 0;
	for (;;) {
		nread = getline(&line, &len, fp);
		if (nread == -1) {
			break;
		}
		if (nread - 1 > *width) {
			*width = nread - 1;
		}
		(*height)++;
	}
	free(line);
	rewind(fp);
}


void
map_fill_from_file(map_t * map, FILE * fp)
{
	char * line = NULL;
	size_t len;
	ssize_t nread;
	size_t nlines = 0;
	for (;;) {
		nread = getline(&line, &len, fp);
		if (nread == -1) {
			break;
		}
		line[nread - 1] = '\0';
		for (int i = 0; i < nread - 1; i++) {
			if (line[i] == '.') {
				map->items[nlines][i] = FLOOR;
			} else if (line[i] == 'L') {
				map->items[nlines][i] = SEAT_EMPTY;
			} else if (line[i] == '#') {
				map->items[nlines][i] = SEAT_OCCUPIED;
			} else {
				fprintf(stderr, "Invalid field symbol `%c`.\n", line[i]);
				exit(EXIT_FAILURE);
			}
 		}
 		nlines++;
	}
	free(line);
}


void
map_init_from_file(map_t * map, char * filepath)
{
	FILE * fp = fopen(filepath, "r");
	if (fp == NULL) {
		fprintf(stderr, "Failed to open file `%s`.\n", filepath);
		exit(EXIT_FAILURE);
	}

	size_t width;
	size_t height;
	get_width_and_height(fp, &width, &height);

	map_init(map, width, height);
	map_fill_from_file(map, fp);

	fclose(fp);
}


void
map_copy(map_t * dst, map_t * src)
{
	if (dst->width != src->width || dst->height != src->height) {
		fprintf(stderr, "Failed to copy map: dimensions differ.\n");
		exit(EXIT_FAILURE);
	}
	for (int vpos = 0; vpos < dst->height; vpos++) {
		for (int hpos = 0; hpos < dst->width; hpos++) {
			dst->items[vpos][hpos] = src->items[vpos][hpos];
		}
	}
}


bool
is_valid_field(map_t * map, size_t vpos, size_t hpos)
{
	return (vpos >= 0 && vpos < map->height && hpos >= 0 && hpos < map->width);
}


bool
is_seat_occupied(map_t * map, size_t vpos, size_t hpos)
{
	return (map->items[vpos][hpos] == SEAT_OCCUPIED);
}


bool
is_seat_empty(map_t * map, size_t vpos, size_t hpos)
{
	return (map->items[vpos][hpos] == SEAT_EMPTY);
}


int
count_occupied_adjacent_seats(map_t * map, int vpos, int hpos)
{
	int cnt = 0;

	for (int i = vpos - 1; i <= (vpos + 1); i++) {
		for (int j = hpos - 1; j <= (hpos + 1); j++) {
			if (i == vpos && j == hpos) {
				continue;
			} else if (!is_valid_field(map, i, j)) {
				continue;
			} else if (is_seat_occupied(map, i, j)) {
				cnt++;
			}
		}
	}
	return cnt;
}


bool
is_first_seat_occupied(map_t * map, int vpos, int hpos, int vdir, int hdir)
{
	int vstop;
	if (vdir == 0) {
		vstop = INT_MAX;
	} else if (vdir == 1) {
		vstop = map->height - vpos;
	} else {
		vstop = vpos + 1;
	}

	int hstop;
	if (hdir == 0) {
		hstop = INT_MAX;
	} else if (hdir == 1) {
		hstop = map->width - hpos;
	} else {
		hstop = hpos + 1;
	}

	int stop = vstop < hstop ? vstop : hstop;

	int vpos_new;
	int hpos_new;
	for (int i = 1; i < stop; i++) {
		vpos_new = vpos + vdir * i;
		hpos_new = hpos + hdir * i;

		if (is_seat_occupied(map, vpos_new, hpos_new)) {
			return true;
		} else if (is_seat_empty(map, vpos_new, hpos_new)) {
			return false;
		}
	}
	return false;
}


int
count_occupied_visible_seats(map_t * map, int vpos, int hpos)
{
	int cnt = 0;

	for (int vdir = -1; vdir <= 1; vdir++) {
		for (int hdir = -1; hdir <= 1; hdir++) {

			if (vdir == 0 && hdir == 0) {
				continue;
			} else if (is_first_seat_occupied(map, vpos, hpos, vdir, hdir)) {
				cnt++;
			}

		}
	}

	return cnt;
}


typedef int (*count_func)(map_t * map, int vpos, int hpos);

typedef struct {
	count_func c;
	int threshold;
} rule_t;


bool
apply_rules_to_field(map_t * map, size_t vpos, size_t hpos, map_t * map_cpy, rule_t * rules)
{
	int num_seats = rules->c(map, vpos, hpos);

	fieldtype_e * field = &(map->items[vpos][hpos]);
	fieldtype_e * fieldcpy = &(map_cpy->items[vpos][hpos]);

	if (*field == SEAT_EMPTY && num_seats == 0) {
		*fieldcpy = SEAT_OCCUPIED;
		return true;
	} else if (*field == SEAT_OCCUPIED && num_seats >= rules->threshold) {
		*fieldcpy = SEAT_EMPTY;
		return true;
	}
	return false;
}


bool
apply_rules_to_map(map_t * map, map_t * map_cpy, rule_t * rules)
{
	bool any_field_changed = false;
	bool field_changed;

	for (int i = 0; i < map->height; i++) {
		for (int j = 0; j < map->width; j++) {
			field_changed = apply_rules_to_field(map, i, j, map_cpy, rules);
			if (field_changed) {
				any_field_changed = true;
			}
		}
	}
	return any_field_changed;
}


size_t
count_occupied_seats(map_t * map)
{
	size_t cnt = 0;
	for (int vpos = 0; vpos < map->height; vpos++) {
		for (int hpos = 0; hpos < map->width; hpos++) {
			if (is_seat_occupied(map, vpos, hpos)) {
				cnt++;
			}
		}
	}
	return cnt;
}


void
simulate(map_t * map, rule_t * rules)
{
	map_t map_new;
	map_init(&map_new, map->width, map->height);
	map_copy(&map_new, map);

	bool any_field_changed;
	for (;;) {
		any_field_changed = apply_rules_to_map(map, &map_new, rules);
		if (!any_field_changed) {
			break;
		}
		map_copy(map, &map_new);
	}

	map_free(&map_new);
}


int
load_map_and_simulate(map_t * map, char * filepath, rule_t * rules)
{
	map_init_from_file(map, filepath);
	simulate(map, rules);
	size_t cnt = count_occupied_seats(map);
	map_free(map);
	return cnt;
}


int
main(int argc, char * argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s FILE\n", argv[0]);
		return EXIT_FAILURE;
	}
	char * filepath = argv[1];

	map_t map;

	rule_t rules_pt1 = { &count_occupied_adjacent_seats, THRESHOLD_PT1 };
	rule_t rules_pt2 = { &count_occupied_visible_seats, THRESHOLD_PT2 };

	size_t cnt = load_map_and_simulate(&map, filepath, &rules_pt1);
	printf("%d\n", cnt);

	cnt = load_map_and_simulate(&map, filepath, &rules_pt2);
	printf("%d\n", cnt);

	return EXIT_SUCCESS;
}

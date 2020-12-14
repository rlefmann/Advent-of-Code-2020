#include <stdio.h>
#include <stdlib.h>

typedef enum {
	NORTH,
	EAST,
	SOUTH,
	WEST
} direction_e;


typedef struct {
	int vpos;
	int hpos;
	direction_e dir;
} ship_t;


void
move_north(ship_t * ship, int value)
{
	ship->vpos = ship->vpos - value;
}


void
move_south(ship_t * ship, int value)
{
	ship->vpos = ship->vpos + value;
}


void
move_west(ship_t * ship, int value)
{
	ship->hpos = ship->hpos - value;
}


void
move_east(ship_t * ship, int value)
{
	ship->hpos = ship->hpos + value;
}


void
turn_right(ship_t * ship, int value)
{
	if (value % 90 != 0) {
		fprintf(stderr, "Turns must be a multiple of 90°.\n");
		exit(EXIT_FAILURE);
	}
	value = value / 90;
	ship->dir = (ship->dir + value) % 4;
}


void
turn_left(ship_t * ship, int value)
{
	if (value % 90 != 0) {
		fprintf(stderr, "Turns must be a multiple of 90°.\n");
		exit(EXIT_FAILURE);
	}
	value = value / 90;

	int dir = (int)ship->dir;
	dir = (dir - value) % 4;
	if (dir < 0) {
		dir = 4 + dir;
	}
	ship->dir = dir;
}


void
move_forward(ship_t * ship, int value)
{
	if (ship->dir == NORTH) {
		move_north(ship, value);
	} else if (ship->dir == EAST) {
		move_east(ship, value);
	} else if (ship->dir == SOUTH) {
		move_south(ship, value);
	} else {
		move_west(ship, value);
	}
}


void
follow_instruction(ship_t * ship, char instr, int value)
{
	if (instr == 'N') {
		move_north(ship, value);
	} else if (instr == 'E') {
		move_east(ship, value);
	} else if (instr == 'S') {
		move_south(ship, value);
	} else if (instr == 'W') {
		move_west(ship, value);
	} else if (instr == 'R') {
		turn_right(ship, value);
	} else if (instr == 'L') {
		turn_left(ship, value);
	} else if (instr == 'F') {
		move_forward(ship, value);
	} else {
		fprintf(stderr, "Invalid instruction: `%c`.\n", instr);
		exit(EXIT_FAILURE);
	}
}


void
navigate(ship_t * ship, char * filepath)
{
	FILE * fp = fopen(filepath, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error opening file `%s`.\n", filepath);
		exit(EXIT_FAILURE);
	}

	char * line = NULL;
	size_t len;
	ssize_t nread;

	char instr;
	int value;
	for (;;) {
		nread = getline(&line, &len, fp);
		if (nread == -1) {
			break;
		}
		line[nread - 1] = '\0';

		instr = line[0];
		value = atoi(line + 1);

		follow_instruction(ship, instr, value);
	}

	free(line);
	fclose(fp);
}


int
main(int argc, char * argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s NAVFILE\n", argv[0]);
		return EXIT_FAILURE;
	}
	char * filepath = argv[1];

	ship_t ship = { 0, 0, EAST };

	navigate(&ship, filepath);

	printf("%d\n", ship.vpos + ship.hpos);
	return EXIT_SUCCESS;
}

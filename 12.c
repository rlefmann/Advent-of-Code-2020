#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>  // REMOVE

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
	// for pt. 2:
	int vpos_waypoint;
	int hpos_waypoint;
} ship_t;


void
move_north(ship_t * ship, int value)
{
	ship->vpos = ship->vpos + value;
}


void
move_south(ship_t * ship, int value)
{
	ship->vpos = ship->vpos - value;
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
move_waypoint_north(ship_t * ship, int value)
{
	ship->vpos_waypoint += value;
}


void
move_waypoint_south(ship_t * ship, int value)
{
	ship->vpos_waypoint -= value;
}


void
move_waypoint_west(ship_t * ship, int value)
{
	ship->hpos_waypoint -= value;
}


void
move_waypoint_east(ship_t * ship, int value)
{
	ship->hpos_waypoint += value;
}


void
move_toward_waypoint(ship_t * ship, int value)
{
	ship->vpos += value * ship->vpos_waypoint;
	ship->hpos += value * ship->hpos_waypoint;
}


double
deg2rad(double deg)
{
	return deg * M_PI / 180;
}


void
rotate(double * vpos, double * hpos, double alpha, bool right)
{
	double alpha_rad = deg2rad(alpha);
	double cs = cos(alpha_rad);
	double sn = sin(alpha_rad);

	double vc = *vpos * cs;
	double vs = *vpos * sn;
	double hc = *hpos * cs;
	double hs = *hpos * sn;

	if (right) {
		*vpos = vc - hs;
		*hpos = hc + vs;
	} else {
		*vpos = hs + vc;
		*hpos = hc - vs; 
	}

}


void
rotate_waypoint(ship_t * ship, int value, bool right)
{
	if (value % 90 != 0) {
		fprintf(stderr, "Turns must be a multiple of 90°.\n");
		exit(EXIT_FAILURE);
	}
	double vpos = ship->vpos_waypoint;
	double hpos = ship->hpos_waypoint;
	rotate(&vpos, &hpos, value, right);

	// round to avoid numerical problems
	// we could also add a small number like 0.001, instead
	ship->vpos_waypoint = (int)round(vpos);
	ship->hpos_waypoint = (int)round(hpos);
}


void
follow_instruction_pt1(ship_t * ship, char instr, int value)
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
follow_instruction_pt2(ship_t * ship, char instr, int value)
{
	if (instr == 'N') {
		move_waypoint_north(ship, value);
	} else if (instr == 'E') {
		move_waypoint_east(ship, value);
	} else if (instr == 'S') {
		move_waypoint_south(ship, value);
	} else if (instr == 'W') {
		move_waypoint_west(ship, value);
	} else if (instr == 'R') {
		rotate_waypoint(ship, value, true);
	} else if (instr == 'L') {
		rotate_waypoint(ship, value, false);
	} else if (instr == 'F') {
		move_toward_waypoint(ship, value);
	} else {
		fprintf(stderr, "Invalid instruction: `%c`.\n", instr);
		exit(EXIT_FAILURE);
	}
}


typedef void (*nav_func)(ship_t * ship, char instr, int value);


void
navigate(ship_t * ship, char * filepath, nav_func follow_instruction)
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
manhattan(ship_t * ship)
{
	return abs(ship->vpos) + abs(ship->hpos);
}


int
main(int argc, char * argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s NAVFILE\n", argv[0]);
		return EXIT_FAILURE;
	}
	char * filepath = argv[1];
	
	ship_t ship_pt1 = { 0, 0, EAST, 0, 0 };
	navigate(&ship_pt1, filepath, &follow_instruction_pt1);
	printf("pt1: %d\n", manhattan(&ship_pt1));
	
	ship_t ship_pt2 = { 0, 0, EAST, 1, 10 };
	navigate(&ship_pt2, filepath, &follow_instruction_pt2);
	printf("pt2: %d\n", manhattan(&ship_pt2));

	return EXIT_SUCCESS;
}

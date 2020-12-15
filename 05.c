#define  _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BSPLEN 10
#define BSPROWLEN 7
#define NUMROWS 128
#define NUMCOLS 8

int
compute_seat_id(int row, int col)
{
	return row * 8 + col;
}


int
compute_seat_id_from_bsp(char * bsp)
{
	int f = 0;
	int	b = NUMROWS - 1;
	int m;
	for (int i = 0; i < BSPROWLEN; i++) {
		m = (f + b) / 2;
		if (bsp[i] == 'F') {
			b = m;
		} else {
			f = m + 1;
		}
	}
	int row = f;

	int l = 0;
	int	r = NUMCOLS - 1;
	for (int i = BSPROWLEN; i < BSPLEN; i++) {
		m = (l + r) / 2;
		if (bsp[i] == 'L') {
			r = m;
		} else {
			l = m + 1;
		}
	}
	int col = l;	

	return compute_seat_id(row, col);
}


bool
is_valid_bsp(char * bsp)
{
	if (strlen(bsp) != BSPLEN) {
		return false;
	}
	for (int i = 0; i < BSPROWLEN; i++) {
		if (bsp[i] != 'F' && bsp[i] != 'B') {
			return false;
		}
	}
	for (int i = BSPROWLEN; i < BSPLEN; i++) {
		if (bsp[i] != 'L' && bsp[i] != 'R') {
			return false;
		}
	}
	return true;
}


int
find_max_seat_id(FILE * fp)
{
	int seatids[NUMCOLS * NUMROWS] = { 0 };
	int sid = -1;
	int max = -1;
	char * line = NULL;
	size_t len;
	ssize_t nread;
	for (;;) {
		nread = getline(&line, &len, fp);
		if (nread == -1) {
			break;
		} else if (line[nread - 1] == '\n') {
			line[nread - 1] = '\0';
		}
		if (!is_valid_bsp(line)) {
			fprintf(stderr, "Error: `%s` is not a valid binary space partition.\n", line);
			free(line);
			fclose(fp);
			exit(EXIT_FAILURE);
		}
		sid = compute_seat_id_from_bsp(line);
		seatids[sid] = 1;
		if (sid > max) {
			max = sid;
		}
	}
	free(line);
	for (int i = 1; i < NUMROWS * NUMCOLS - 1; i++) {
		if (seatids[i] == 0 && seatids[i-1] == 1 && seatids[i+1] == 1) {
			printf("Your seat has id %d.\n", i);
			break;
		}
	}
	return max;
}


int
main(int argc, char * argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s BPFILE\n", argv[0]);
		return EXIT_FAILURE;
	}
	char * filepath = argv[1];

	FILE * fp = fopen(filepath, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error opening file `%s`.\n", filepath);
		return EXIT_FAILURE;
	}
	int sidmax = find_max_seat_id(fp);
	printf("Maximum id is %d.\n", sidmax);
	fclose(fp);
	return EXIT_SUCCESS;
}

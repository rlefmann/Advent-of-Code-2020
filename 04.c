#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define COLORCODE_LENGTH 8
#define PID_LENGTH 10

#define PASSPORT_EOF -1
#define PASSPORT_INVALID 0
#define PASSPORT_VALID 1

#define NUM_FIELDS 8
#define END_OF_FIELDS -1
enum { byr, iyr, eyr, hgt, hcl, ecl, pid, cid };


bool
isemptyline(char * line)
{
	int i = 0;
	for (;;)	{
		if (line[i] == '\0') {
			return true;
		} else if (!isspace(line[i])) {
			return false;
		}
		i++;
	}
}


struct pw_t {
	int byr;
	int iyr;
	int eyr;
	char hgt[6];
	char hcl[8];
	char ecl[4];
	char pid[10];
	int num_fields_set;
};


void
pw_init(struct pw_t * pw)
{
	pw->byr = -1;
 	pw->iyr = -1;
 	pw->eyr = -1;
 	strcpy(pw->hgt, "");
 	strcpy(pw->hcl, "");
 	strcpy(pw->ecl, "");
 	strcpy(pw->pid, "");
	pw->num_fields_set = 0;
}


void
pw_set_field(struct pw_t * pw, char * fieldname, char * fieldval)
{
	printf("$ %s -> %s\n", fieldname, fieldval);
	if (strcmp(fieldname, "byr") == 0) {
		pw->byr = atoi(fieldval);
	} else if (strcmp(fieldname, "iyr") == 0) {
		pw->iyr = atoi(fieldval);
	} else if (strcmp(fieldname, "eyr") == 0) {
		pw->eyr = atoi(fieldval);
	} else if (strcmp(fieldname, "hgt") == 0) {
		strcpy(pw->hgt, fieldval);
	} else if (strcmp(fieldname, "hcl") == 0) {
		strcpy(pw->hcl, fieldval);
	} else if (strcmp(fieldname, "ecl") == 0) {
		strcpy(pw->ecl, fieldval);
	} else if (strcmp(fieldname, "pid") == 0) {
		strcpy(pw->pid, fieldval);
	} else if (strcmp(fieldname, "cid") == 0) {
		// do nothing
		pw->num_fields_set--;	// TODO: this is rather hacky
	} else {
		fprintf(stderr, "Invalid field name `%s`\n", fieldname);
		exit(EXIT_FAILURE);
	}
	pw->num_fields_set++;
}


bool
pw_isvalid_pt1(struct pw_t * pw)
{
	if (pw->num_fields_set != NUM_FIELDS - 1) {
		return false;
	}
	return true;
}


bool
is_valid_byr(int byr)
{
	return (byr >= 1920 && byr <= 2002);
}


bool
is_valid_iyr(int iyr)
{
	return (iyr >= 2010 && iyr <= 2020);
}


bool
is_valid_eyr(int eyr)
{
	return (eyr >= 2020 && eyr <= 2030);
}


bool
is_valid_hgt(char * hgt)
{
	int len = strlen(hgt);
	if (len < 4 || len > 5) {
		return false;
	}
	char * unit = hgt + len - 2;
	char val[4];
	strncpy(val, hgt, len - 2);
	val[len - 2] = '\0';
	int hgtval = atoi(val);
	if (strcmp(unit, "cm") == 0) {
		if (hgtval >= 150 && hgtval <= 193) {
			return true;
		}
	} else if (strcmp(unit, "in") == 0) {
		if (hgtval >= 59 && hgtval <= 76) {
			return true;
		}
	}
	return false;
}


bool
is_valid_hcl(char * hcl)
{
	int len = strlen(hcl);
	if (len != 7) {
		return false;
	} else if (hcl[0] != '#') {
		return false;
	}
	char c;
	for (int i = 1; i < len; i++) {
		c = hcl[i];
		if (!isdigit(c) && (c < 'a' || c > 'f')) {
			return false;
		}
	}
	return true;
}


bool
is_valid_ecl(char * ecl)
{
	char * colors[] = { "amb", "blu", "brn", "gry", "grn", "hzl", "oth" };
	for (int i = 0; i < 7; i++) {
		if (strcmp(ecl, colors[i]) == 0) {
			return true;
		}
	}
	return false;
}


bool
is_valid_pid(char * pid)
{
	int len = strlen(pid);
	if (len != 9) {
		return false;
	}
	for (int i = 0; i < len; i++) {
		if (!isdigit(pid[i])) {
			return false;
		}
	}
	return true;
}


bool
pw_isvalid_pt2(struct pw_t * pw)
{
	if (!is_valid_byr(pw->byr)) {
		return false;
	} else if (!is_valid_iyr(pw->iyr)) {
		return false;
	} else if (!is_valid_eyr(pw->eyr)) {
		return false;
	} else if (!is_valid_hgt(pw->hgt)) {
		return false;
	} else if (!is_valid_hcl(pw->hcl)) {
		return false;
	} else if (!is_valid_ecl(pw->ecl)) {
		return false;
	} else if (!is_valid_pid(pw->pid)) {
		return false;
	}
	return true;
}


void
readfield(struct pw_t * pw, char * field)
{
	char * saveptr;
	char * fieldname = strtok_r(field, ":", &saveptr);
	if (fieldname == NULL || strlen(fieldname) != 3) {
		fprintf(stderr, "Invalid fieldname.\n");
		exit(EXIT_FAILURE);
	}
	char * fieldval = strtok_r(NULL, ":", &saveptr);
	if (fieldval == NULL || strlen(fieldname) < 1) {
		fprintf(stderr, "Invalid fieldvalue.\n");
		exit(EXIT_FAILURE);
	}
	pw_set_field(pw, fieldname, fieldval);
}


int
count_valid_passports(char * filepath)
{
	int cnt = 0;
	FILE * fp = fopen(filepath, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error opening passport file `%s`.\n", filepath);
		exit(EXIT_FAILURE);
	}
	char * line = NULL;
	size_t len;
	ssize_t nread;

	struct pw_t pw;
	pw_init(&pw);

	for (;;) {
		nread = getline(&line, &len, fp);
		if (nread == -1) {
			if (pw_isvalid_pt2(&pw)) {
				printf("valid\n\n");
				cnt++;
			} else {
				printf("not valid\n\n");
			}
			break;
		}
		if (isemptyline(line)) {
			if (pw_isvalid_pt2(&pw)) {
				printf("valid\n\n");
				cnt++;
			} else {
				printf("not valid\n\n");
			}
			pw_init(&pw);	// reset password
			continue;
		} 
		if (line[nread-1] == '\n') {
			line[nread-1] = '\0';
		}
		char * field = strtok(line, " ");
		readfield(&pw, field);
		for (;;) {
			field = strtok(NULL, " ");
			if (field == NULL) {
				break;
			}
			readfield(&pw, field);
		}
	}
	free(line);
	fclose(fp);
	return cnt;
}



int
main(int argc, char * argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s PASSPORTFILE\n", argv[0]);
		return EXIT_FAILURE;
	}
	char * filepath = argv[1];
	int cnt = count_valid_passports(filepath);
	printf("%d\n", cnt);
	return EXIT_SUCCESS;
}

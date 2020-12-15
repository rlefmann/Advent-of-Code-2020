.POSIX:

executables = 01 02 03 04 05 06

objects = 01.o 02.o 03.o 04.o 05.o 06.o lib/alloc.o lib/util.o

all: $(executables)

01-sources = 01.c lib/alloc.c lib/util.c
01-objects = $(01-sources:.c=.o)
01: $(01-objects)
01.o: 01.c lib/listint.h lib/list.h lib/alloc.h

02: 02.o
02.o: 02.c

03: 03.o
03.o: 03.c

04: 04.o
04.o: 04.c

05: 05.o
05.o: 05.c

06: 06.o
06.o: 06.c

lib/alloc.o: lib/alloc.c lib/alloc.h
lib/util.o: lib/util.c lib/util.h

clean:
	rm -rf $(executables) $(objects) 

#.SUFFIXES: .c .o
#.c.o:
#	$(CC) -c $(CFLAGS) -o $@ $<

.POSIX:

executables = 03 04 05 06 07

objects = 03.o 04.o 05.o 06.o 07.o /lib/alloc.o lib/util.o

all: $(executables)

03-sources = 03.c lib/alloc.c lib/util.c
03-objects = $(03-sources:.c=.o)
03: $(03-objects)
03.o: 03.c lib/matrixint.h lib/matrix.h lib/alloc.h

04: 04.o
04.o: 04.c

05: 05.o
05.o: 05.c

06: 06.o
06.o: 06.c

07-sources = 07.c lib/alloc.c lib/util.c
07-objects = $(07-sources:.c=.o)
07: $(07-objects)
07.o: 07.c lib/list.h lib/alloc.h lib/liststring.h lib/list.h

lib/alloc.o: lib/alloc.c lib/alloc.h
lib/util.o: lib/util.c lib/util.h

clean:
	rm -rf $(executables) $(objects) 

#.SUFFIXES: .c .o
#.c.o:
#	$(CC) -c $(CFLAGS) -o $@ $<

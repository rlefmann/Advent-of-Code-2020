.POSIX:

all: 01

01-sources = 01.c lib/alloc.c lib/util.c
01-objects = $(01-sources:.c=.o)

01: $(01-objects)
	gcc -o $@ $(01-objects)

01.o: 01.c lib/listint.h lib/list.h lib/alloc.h
lib/alloc.o: lib/alloc.c lib/alloc.h
lib/util.o: lib/util.c lib/util.h

clean:
	rm -rf 01 $(01-objects)

.SUFFIXES: .c .o
.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

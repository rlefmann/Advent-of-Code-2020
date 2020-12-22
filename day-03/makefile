.POSIX:

sources = main.c ../lib/alloc.c ../lib/util.c
objects = $(sources:.c=.o)

main: $(objects)
main.o: main.c ../lib/matrixint.h ../lib/matrix.h ../lib/alloc.h

clean:
	rm -rf main $(objects)

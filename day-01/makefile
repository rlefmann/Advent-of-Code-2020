.POSIX:

sources = main.c ../lib/alloc.c ../lib/util.c
objects = $(sources:.c=.o)

main: $(objects)
main.o: main.c ../lib/listint.h ../lib/list.h ../lib/alloc.h ../lib/util.h

clean:
	rm -rf main $(objects)

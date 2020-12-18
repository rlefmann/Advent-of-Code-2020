.POSIX:

executables = main
sources = main.c instruction.c ../lib/alloc.c ../lib/util.c
objects = $(sources:.c=.o)

all: $(executables)

main: $(objects)

main.o: main.c ../lib/util.h instruction.h ../lib/list.h ../lib/alloc.h
instruction.o: instruction.c instruction.h ../lib/util.h
alloc.o: ../lib/alloc.c ../lib/alloc.h ../lib/util.h
util.o: ../lib/util.c ../lib/util.h

clean:
	rm -rf $(executables) $(objects)

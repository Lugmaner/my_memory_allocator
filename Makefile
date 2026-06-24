CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wpedantic -std=c18

all: alloc test
	$(CC) $(CFLAGS) test.o l_mem_alloc.o -o runable


alloc: l_mem_alloc.c
	$(CC) $(CFLAGS) -c l_mem_alloc.c -o l_mem_alloc.o

test: test.c
	$(CC) $(CFLAGS) -c test.c -o test.o

clean:
	rm -f *.o runable
CC = gcc
PR = -g
INCFLAGS = -I../inc
CFLAGS = -m32 -Wno-builtin-declaration-mismatch $(INCFLAGS)

all: halloc.o

halloc.o: halloc.c
	$(CC) $(PR) $(CFLAGS) -c halloc.c -o halloc.o

main.o: main.c
	$(CC) $(PR) $(CFLAGS) -c main.c -o main.o

run: all
	./start

clean:
	rm -f *.o start

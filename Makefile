CC = gcc
PR = -g

all: halloc.o main.o
	$(CC) $(PR) halloc.o main.o -o start

halloc.o: halloc.c
	$(CC) $(PR) -c halloc.c -o halloc.o

main.o: main.c
	$(CC) $(PR) -c main.c -o main.o

run: all
	./start

clean:
	rm -f *.o start

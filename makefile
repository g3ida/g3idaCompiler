CC=gcc
CPP=g++
CFLAGS=-W -Wall -ansi -pedantic -Wvariadic-macros
CPPFLAGS=-std=c++11
LDFLAGS=

all: g3c vm

g3c: util.o parse.o lexical.o main.o
	$(CC) -o g3c util.o parse.o lexical.o main.o $(LDFLAGS)

util.o: util.c util.h
	$(CC) -o util.o -c util.c $(CFLAGS)

parse.o: parse.c parse.h util.h lexical.h
	$(CC) -o parse.o -c parse.c $(CFLAGS)

lexical.o: lexical.c lexical.h util.o
	$(CC) -o lexical.o -c lexical.c $(CFLAGS)

main.o: main.c lexical.h parse.h util.h
	$(CC) -o main.o -c main.c $(CFLAGS)

vm : vm.cpp
	$(CPP) -o vm vm.cpp $(CPPFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
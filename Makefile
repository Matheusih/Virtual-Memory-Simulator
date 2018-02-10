CC=gcc
CXX=g++

CFLAGS=-g -O1 -std=c99 -Wall -Wextra -Werror
CXXFLAGS=$(CFLAGS) -std=c++11
LDFLAGS=-lm


all: test grade ./teste

memvirt.o: memvirt.c

test: memvirt.o test.c
	$(CC) $(CFLAGS) memvirt.o test.c -o test $(LDFLAGS)

grade: memvirt.o test.c
	$(CC) $(CFLAGS) memvirt.o test.c -o grade $(LDFLAGS)

./teste:
	-./test

run_student: test
	./test

clean:
	rm -rf *.o test grade

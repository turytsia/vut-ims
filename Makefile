#
# File: Makefile
# Author: xshevc01
#

CC = g++
CFLAGS = -lsimlib
SRC = pool.cpp

all: pool

pool: pool.cpp
	$(CC) $(SRC) -o pool $(CFLAGS)

run: pool
	./pool

clean:
	rm -f pool
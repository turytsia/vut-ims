#
# File: Makefile
# Author: xshevc01
#

CC = g++
CFLAGS = -lsimlib
SRC1 = pool.cpp
SRC2 = pool_after.cpp
SRC3 = pool_weekend.cpp

all: pool pool_after pool_weekend

pool: pool.cpp
	$(CC) $(SRC1) -o pool $(CFLAGS)

pool_after: pool_after.cpp
	$(CC) $(SRC2) -o pool_after $(CFLAGS)

pool_weekend: pool_weekend.cpp
	$(CC) $(SRC3) -o pool_weekend $(CFLAGS)

run: pool pool_after pool_weekend
	./pool
	./pool_after
	./pool_weekend

clean:
	rm -f pool pool_after pool_weekend *.out
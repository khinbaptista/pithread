########################################################################
#
#	INF01142 - Operating Systems
#		Assignment 1 - Threads
#
#	Tests for PICCOLO THREADS (pithread)
#
#	João Lauro Garibaldi Jr		195505
#	Khin Baptista				217443
#
########################################################################

CC = gcc
FLAGS = -Wall

LIB_DIR = ../lib
INC_DIR = ../include

########################################################################

all: yield.run mutex.run

yield.run: yield.c $(LIB_DIR)/libpithread.a
	$(CC) -o $@ $^ -I$(INC_DIR) -L$(LIB_DIR) -lpithread $(FLAGS)

mutex.run: mutex.c $(LIB_DIR)/libpithread.a
	$(CC) -o $@ $^ -I$(INC_DIR) -L$(LIB_DIR) -lpithread $(FLAGS)

clean:
	rm -rf *.run *.o *~

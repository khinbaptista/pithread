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
FLAGS = -Wall -std=c99

LIB_DIR = ../lib
INC_DIR = ../include

########################################################################

all: teste1.run

teste1.run: teste1.c $(LIB_DIR)/libpithread.a
	$(CC) -o $@ $^ -I$(INC_DIR) -L$(LIB_DIR) -lpithread $(FLAGS)

clean:
	rm -rf *.run *.o *~

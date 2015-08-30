########################################################################
#
#	INF01142 - Operating Systems
#		Assignment 1 - Threads
#
#	PICCOLO THREADS (pithread)
#
#	João Lauro Garibaldi Jr		1xxxxx
#	Khin Baptista				217443
#
#	This makefile creates libpithread.a inside the lib subdirectory
#		The library implements a thread management system at user level
#
########################################################################

LIB = libpithread.a

#################################################

CC = gcc

CFLAGS = -Wall

#################################################

LIB_DIR = ./lib
INC_DIR = ./include
BIN_DIR = ./bin
SRC_DIR = ./src

#################################################

SRC =

OBJ = $(SRC:.c=.o)
OBJECTS = $(patsubst %, $(BIN_DIR)/%, $(OBJ))

#################################################

all: $(LIB_DIR)/$(LIB)

$(LIB_DIR)/$(LIB): $(OBJECTS)
	ar crs $@ $^
	
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c -o $@ $^ -I$(INC_DIR) $(CFLAGS)
	
#################################################

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~

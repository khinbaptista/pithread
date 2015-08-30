#################################################
#
#	pithread.a makefile header
#		complete later
#
#################################################

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

all: $(LIB)

$(LIB): $(OBJECTS)
	ar crs $@ $^
	
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c -o $@ $^ -I$(INC_DIR) $(CFLAGS)
	
#################################################

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~


CC=gcc

INCLUDE =-I./src
SHARED_LIBS =-lm -lpthread -lmenu -lpanel -lncursesw -lev

CFLAGS = $(INCLUDE) -Wall -g $(SHARED_LIBS)

SRC = $(wildcard ./src/*.c) $(wildcard ./src/lib/*.c)
OBJ = $(SRC:.c=.o)
LIB = $(wildcard ./src/lib/*.a)

# The compiled executable
EXECUTE = dork.run

all: $(EXECUTE)

$(EXECUTE): $(OBJ) $(LIB)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(EXECUTE) $(OBJ)



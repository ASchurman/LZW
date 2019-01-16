#-------------------------------------------------------------------------------

#	Makefile for LZW
#
#	Alexander Schurman
#	alexander.schurman@gmail.com

# source files with extensions, separated by spaces
SOURCES	:=main.c stringTable.c lzw.c stack.c code.c

# define DEBUG=1 in command line for debug

#-------------------------------------------------------------------------------

CC              := gcc

# flags------------------------------------
CFLAGSBASE      := -std=c99 -Wall -pedantic -Werror

DEBUGFLAGS      := -g3
RELEASEFLAGS    := -O3

ifeq ($(DEBUG),1)
	CFLAGS  := $(CFLAGSBASE) $(DEBUGFLAGS)
else
	CFLAGS  := $(CFLAGSBASE) $(RELEASEFLAGS)
endif

# building---------------------------------

OBJ             := $(SOURCES:.c=.o)

all: $(OBJ)
	$(CC) $(CFLAGS) -o encode $^
	ln -f encode decode

encode: $(OBJ)
	$(CC) $(CFLAGS) -o encode $^
decode: $(OBJ)
	$(CC) $(CFLAGS) -o decode $^

main.o: lzw.h
lzw.o: lzw.h stringTable.h stack.h
stack.o: stack.h
stringTable.o: stringTable.h

# cleaning---------------------------------

clean:
	rm -f encode decode *.o

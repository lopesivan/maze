##
# \file Makefile
# \author Matthew Eckert (mteckert@gmail.com)
#
# Makefile for the maze project. Generates three executables:
#     maze-gen
#     maze-solve
#     maze-show
#

CC = gcc
CFLAGS = -g -ansi -pedantic-errors -Wall -Wextra -Werror
objects = maze.o maze-solve.o maze-gen.o maze-show.o
headers = maze.h bool.h try.h

all : maze-solve maze-gen maze-show
.PHONY : all

maze-solve : maze-solve.o maze.o
	$(CC) $(CFLAGS) -o maze-solve maze-solve.o maze.o

maze-gen : maze-gen.o maze.o
	$(CC) $(CFLAGS) -o maze-gen maze-gen.o maze.o

maze-show : maze-show.o maze.o
	$(CC) $(CFLAGS) -o maze-show maze-show.o maze.o

maze-solve.o : maze-solve.c maze.c $(headers)
maze-gen.o : maze-gen.c maze.c $(headers)
maze-show.o : maze-show.c maze.c $(headers)
maze.o : maze.c $(headers)

.PHONY : clean
clean :
	rm *.exe maze-solve maze-gen maze-show $(objects)

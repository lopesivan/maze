#include <stdio.h>
#include <stdlib.h>

#define DEBUG

#include "bool.h"
#include "try.h"
#include "maze.h"

int main(int argc, char **argv)
{
    bool printed;
    Maze myMaze;

    printed = EXIT_FAILURE;
    TRY( (myMaze = Maze_new()) );

    TRY(argc > 1);
    TRY(Maze_import(myMaze, argv[1]));
    Maze_print(myMaze);
    printed = EXIT_SUCCESS;

FINALLY:
    Maze_free(&myMaze);
    return printed;
}

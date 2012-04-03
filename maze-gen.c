#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEBUG

#include "bool.h"
#include "try.h"
#include "maze.h"

#define DEFAULTSIZE 10

void generateFrom(Maze maze, Room room);
void shuffleRoomArray(Room *array, int size);
void makeRandomTunnels(Maze maze, int nwalls);
void parseArguments(int argc, char **argv, int *height, int *width, char *filename);

int main(int argc, char **argv)
{
    bool generated;
    int height, width;
    char filename[256];
    Maze myMaze;

    generated = EXIT_FAILURE;
    height = width = DEFAULTSIZE;
    strcpy(filename, "-");
    parseArguments(argc, argv, &height, &width, filename);
    TRY( (myMaze = Maze_newFilled(height, width)) );
    srand(time(NULL));
    generateFrom(myMaze, Maze_getRandomRoom(myMaze));
    makeRandomTunnels(myMaze, height/2);
    Maze_setStart(myMaze, Maze_getRandomRoom(myMaze));
    Maze_setFinish(myMaze, Maze_getRandomRoom(myMaze));
    TRY(Maze_export(myMaze, filename));
    generated = EXIT_SUCCESS;

FINALLY:
    Maze_free(&myMaze);
    return generated;
}

void generateFrom(Maze maze, Room room)
{
    int i, size;
    Room adjacent[4];

    Maze_setMarker(room, Room_visited);
    size = Maze_getAdjacent(maze, room, adjacent, true);
    if (size > 1) shuffleRoomArray(adjacent, size);
    for (i = 0; i < size; i++)
        if (Maze_getMarker(adjacent[i]) == Room_cleared)
        {
            generateFrom(maze, adjacent[i]);
            Maze_tunnel(maze, room, adjacent[i]);
        }
}

void shuffleRoomArray(Room *array, int size)
{
    int i, chosen;
    Room temp;

    for (i = 0; i < size - 1; i++)
    {
        chosen = i + (rand() % (size - i));
        temp = array[i];
        array[i] = array[chosen];
        array[chosen] = temp;
    }
}

void makeRandomTunnels(Maze maze, int ntunnels)
{
    int tunnels, nadjacent;
    Room randomRoom, adjacent[4];

    tunnels = 0;
    do
    {
        randomRoom = Maze_getRandomRoom(maze);
        nadjacent = Maze_getAdjacent(maze, randomRoom, adjacent, true);
        if (nadjacent > 1) shuffleRoomArray(adjacent, nadjacent);
        if (adjacent[0]) Maze_tunnel(maze, randomRoom, adjacent[0]), tunnels++;
    } while (tunnels < ntunnels);
}

void parseArguments(int argc, char **argv, int *height, int *width, char *filename)
{
    int i;

    for (i = 1; i < argc; i++)
    {
        if (strcmp("-h", argv[i]) == 0 && argv[++i])
            *height = strtol(argv[i], NULL, 0);
        else if (strcmp("-w", argv[i]) == 0 && argv[++i])
            *width = strtol(argv[i], NULL, 0);
        else strcpy(filename, argv[i]);
    }
    if (!*width) *width = DEFAULTSIZE;
    if (!*height) *height = DEFAULTSIZE;
}

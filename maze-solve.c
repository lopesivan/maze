#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

#include "bool.h"
#include "try.h"
#include "maze.h"

/**
 * Solves the maze from a given start room.
 *
 * \param maze The maze to solve.
 * \param room The room to start from.
 *
 * \return true if the maze has a solution, false if not.
 */
bool solveFrom(Maze maze, Room room);

/**
 * Prints all solutions to a maze.
 *
 * \param maze The maze to solve.
 * \param room The room to start from.
 *
 * \return The number of solutions found.
 */
int solveAll(Maze maze, Room room);

/**
 * Finds the shortest solution to the maze. The starting depth should be 0 when
 * calling the function. If the depth passed/returned is 0, it can be assumed
 * that there was no solution to the maze found.
 *
 * \param maze The maze to solve.
 * \param room The room to start from.
 * \param depth The starting depth.
 * \param shortMaze An initialized maze that will hold a snapshot of the maze
 *     solved with the shortest path.
 *
 * \return The depth of the shortest solution (length of the shortest path).
 */
int solveShort(Maze maze, Room room, int depth, Maze shortMaze);

/* argument parsing */
bool parseArguments(int argc, char **argv, bool *shortest, bool *all, char *filename);
void printHelp(void);

int main(int argc, char **argv)
{
    bool status, optshort, optall;
    char filename[256];
    Maze myMaze;

    status = EXIT_FAILURE;
    optshort = optall = false;
    strcpy(filename, "-");
    myMaze = Maze_new();

    TRY( (parseArguments(argc, argv, &optshort, &optall, filename)) );
    TRY(myMaze);
    TRY(argc > 1);
    TRY(Maze_import(myMaze, filename));
    if (optall)
    {
        int solutions;

        solutions = solveAll(myMaze, Maze_getStart(myMaze));
        printf("Found %i solutions.\n", solutions);
    }
    else if (optshort)
    {
        Maze shortMaze;
        int depth;

        shortMaze = Maze_new();
        if ( (depth = solveShort(myMaze, Maze_getStart(myMaze), 0, shortMaze)) )
        {
            puts("Shortest solution:");
            Maze_print(shortMaze);
            printf("Path length: %i\n", depth);
        }
        else puts("No solution found.");

    }
    else if (solveFrom(myMaze, Maze_getStart(myMaze)))
    {
        puts("Solution found!");
        Maze_replaceMarkers(myMaze, Room_deadend, Room_cleared);
        Maze_print(myMaze);
    }
    else puts("No solution found.");
    status = EXIT_SUCCESS;

FINALLY:
    Maze_free(&myMaze);
    return status;
}

bool solveFrom(Maze maze, Room room)
{
    int i, nadjacent;
    char marker;
    Room adjacent[4];

    marker = Maze_getMarker(room);
    if (room == Maze_getFinish(maze)) return true;
    if (marker == Room_visited || marker == Room_deadend) return false;
    Maze_setMarker(room, Room_visited);
    Maze_print(maze);
    getchar();
    nadjacent = Maze_getAdjacent(maze, room, adjacent, false);
    for (i = 0; i < nadjacent; i++)
        if (solveFrom(maze, adjacent[i])) return true;
    Maze_setMarker(room, Room_deadend);
    return false;
}

int solveAll(Maze maze, Room room)
{
    int i, nadjacent;
    static int solutionCount;
    char marker;
    Room adjacent[4];

    marker = Maze_getMarker(room);
    if (marker == Room_visited) return solutionCount;
    if (room == Maze_getFinish(maze))
    {
        printf("Solution #%i:\n", ++solutionCount);
        Maze_print(maze);
        getchar();
        return solutionCount;
    }
    Maze_setMarker(room, Room_visited);
    nadjacent = Maze_getAdjacent(maze, room, adjacent, false);
    for (i = 0; i < nadjacent; i++)
        solveAll(maze, adjacent[i]);
    Maze_setMarker(room, Room_cleared);
    return solutionCount;
}

int solveShort(Maze maze, Room room, int depth, Maze shortMaze)
{
    int i, nadjacent;
    static int shortDepth, solutionCount;
    char marker;
    Room adjacent[4];

    marker = Maze_getMarker(room);
    if (marker == Room_visited) return shortDepth;
    if (room == Maze_getFinish(maze))
    {
        if (depth < shortDepth || solutionCount == 0)
        {
            Maze_copy(shortMaze, maze);
            shortDepth = depth;
        }
        solutionCount++;
        return shortDepth;
    }
    Maze_setMarker(room, Room_visited);
    nadjacent = Maze_getAdjacent(maze, room, adjacent, false);
    for (i = 0; i < nadjacent; i++)
        solveShort(maze, adjacent[i], depth + 1, shortMaze);
    Maze_setMarker(room, Room_cleared);
    return shortDepth;
}

bool parseArguments(int argc, char **argv, bool *optshort, bool *optall, char *filename)
{
    int i;

    for (i = 1; i < argc; i++)
    {
        if (strcmp("-help", argv[i]) == 0) {printHelp(); return false;}
        if (strcmp("-h", argv[i]) == 0) {printHelp(); return false;}
        if (strcmp("-short", argv[i]) == 0) *optshort = true;
        if (strcmp("-all", argv[i]) == 0) *optall = true;
        else strcpy(filename, argv[i]);
    }
    return true;
}

void printHelp(void)
{
    fprintf(stderr, "USAGE: maze-solve [-short] [-all] <filename>\n");
    fprintf(stderr, "    where -short shows the shortest solution,\n");
    fprintf(stderr, "          -all shows all solutions\n");
    fprintf(stderr, "    no flags solves the maze in filename step-by-step\n");
}

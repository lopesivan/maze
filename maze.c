#include "maze.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "try.h"

#define NORTH 1
#define EAST  2
#define SOUTH 4
#define WEST  8
int opposite(int dir) { return dir > 2 ? dir >> 2 : dir << 2; }

struct room
{
    int walls;
    char marker;
};

struct maze
{
    int nx, ny;
    Room start, finish, grid;
};

int getX(Maze this, Room room) { return (room - this->grid) / (this->ny); }
int getY(Maze this, Room room) { return (room - this->grid) % this->ny; }
bool hasWall(Room room, int wall) { return (room->walls & wall) != 0; }
bool inMaze(Maze this, int x, int y) { return x >= 0 && y >= 0 && x < this->nx && y < this->ny; }
Room getRoom(Maze this, int x, int y) { return inMaze(this, x, y) ? &this->grid[x * this->ny + y] : NULL; }

Maze Maze_new(void)
{
    Maze this;
    this = malloc(sizeof *this);

    TRY(this);

    this->nx = this->ny = 0;
    this->start = this->finish = this->grid = NULL;

FINALLY:
    return this;
}

bool Maze_import(Maze this, char *filename)
{
    bool imported;
    int x, y;
    Room room;
    FILE *fp;

    imported = false;
    fp = NULL;
    TRY(this);
    if (strcmp(filename, "-") == 0) fp = stdin;
    else TRY( (fp = fopen(filename, "r")) );
    TRY(fscanf(fp, "%i %i ", &this->nx, &this->ny) == 2);
    TRY( (this->grid = malloc(sizeof *this->grid * this->nx * this->ny)) );
    TRY(fscanf(fp, "%d %d", &x, &y) == 2);
    TRY( (this->start = getRoom(this, x, y)) );
    TRY(fscanf(fp, "%d %d", &x, &y) == 2);
    TRY( (this->finish = getRoom(this, x, y)) );
    for (x = 0; x < this->nx; x++)
        for (y = 0; y < this->ny; y++)
        {
            room = getRoom(this, x, y);
            TRY(fscanf(fp, "%d", &room->walls) == 1);
            Maze_setMarker(room, Room_cleared);
        }
    imported = true;

FINALLY:
    if (fp && fp != stdin) fclose(fp);
    return imported;
}

bool Maze_export(Maze this, char *filename)
{
    bool exported;
    int x, y;
    FILE *fp;

    exported = false;
    fp = NULL;
    TRY(this);
    if (strcmp(filename, "-") == 0) fp = stdout;
    else TRY( (fp = fopen(filename, "w")) );
    TRY(fprintf(fp, "%2d %2d\n", this->nx, this->ny) > 0);
    TRY(fprintf(fp, "%2d %2d\n", getX(this, this->start), getY(this, this->start)) > 0);
    TRY(fprintf(fp, "%2d %2d\n", getX(this, this->finish), getY(this, this->finish)) > 0);
    for (x = 0; x < this->nx; x++)
    {
        for (y = 0; y < this->ny; y++)
            TRY(fprintf(fp, "%2d ", getRoom(this, x, y)->walls) > 0);
        TRY(fprintf(fp, "\n") > 0);
    }
    TRY(fprintf(fp, "\n") > 0);
    exported = true;

FINALLY:
    if (fp && fp != stdout) fclose(fp);
    if (fp == stdout) fflush(stdout);
    return exported;
}

bool Maze_copy(Maze destination, Maze source)
{
    bool copied;
    int x, y;
    size_t gridbytes;

    copied = false;
    TRY(destination && source);
    gridbytes = (sizeof *source->grid * source->nx * source->ny);
    memcpy(destination, source, sizeof *source);
    TRY( (destination->grid = malloc(gridbytes)) );
    memcpy(destination->grid, source->grid, gridbytes);
    x = getX(source, source->start); y = getY(source, source->start);
    destination->start = getRoom(destination, x, y);
    x = getX(source, source->finish); y = getY(source, source->finish);
    destination->finish = getRoom(destination, x, y);
    copied = true;

FINALLY:
    return copied;
}

void printBorder(Maze this, int x, int wall);
void printMeat(Maze this, int x);
void Maze_print(Maze this)
{
    int x;

    printBorder(this, 0, NORTH);
    for (x = 0; x < this->nx; x++)
    {
        printMeat(this, x);
        printBorder(this, x, SOUTH);
    }
}

void printBorder(Maze this, int x, int wall)
{
    int y;

    printf("+");
    for (y = 0; y < this->ny; y++)
    {
        if ((getRoom(this, x, y)->walls & wall) != 0)
            printf("---");
        else
            printf("   ");
        printf("+");
    }
    printf("\n");
}

void printMeat(Maze this, int x)
{
    int y;
    Room room;

    if ((getRoom(this, x, 0)->walls & WEST) != 0) printf("|");
    else printf(" ");
    for (y = 0; y < this->ny; y++)
    {
        room = getRoom(this, x, y);
        if (room == Maze_getStart(this)) printf(" S ");
        else if (room == Maze_getFinish(this)) printf(" F ");
        else printf(" %c ", Maze_getMarker(room));
        if ((room->walls & EAST) != 0) printf("|");
        else printf(" ");
    }
    printf("\n");
}

Room Maze_getStart(Maze this) { return this? this->start : NULL; }
Room Maze_getFinish(Maze this) { return this? this->finish : NULL; }
void Maze_setStart(Maze this, Room room) { if (this) this->start = room; }
void Maze_setFinish(Maze this, Room room) { if (this) this->finish = room; }

int Maze_getAdjacent(Maze this, Room room, Room *adjacent, bool walls)
{
    int x, y, count;

    count = 0;
    adjacent[0] = adjacent[1] = adjacent[2] = adjacent[3] = NULL;
    TRY(this && this->grid && room);
    x = getX(this, room);
    y = getY(this, room);
    if (hasWall(room, NORTH) == walls)
        if ( (adjacent[count] = getRoom(this, x - 1, y)) ) count++;
    if (hasWall(room, EAST) == walls)
        if ( (adjacent[count] = getRoom(this, x, y + 1)) ) count++;
    if (hasWall(room, SOUTH) == walls)
        if ( (adjacent[count] = getRoom(this, x + 1, y)) ) count++;
    if (hasWall(room, WEST) == walls)
        if ( (adjacent[count] = getRoom(this, x, y - 1)) ) count++;

FINALLY:
    return count;
}

void Maze_setMarker(Room room, char marker) { room->marker = marker; }
char Maze_getMarker(Room room) { return room->marker; }
void Maze_replaceMarkers(Maze this, char find, char replace)
{
    int x, y;
    Room room;

    for (x = 0; x < this->nx; x++)
        for (y = 0; y < this->ny; y++)
        {
            room = getRoom(this, x, y);
            if (Maze_getMarker(room) == find) Maze_setMarker(room, replace);
        }
}

Maze Maze_newFilled(int x, int y)
{
    bool created;
    Maze this;
    Room room;

    created = false;
    TRY( (this = Maze_new()) );
    this->nx = x; this->ny = y;
    TRY( (this->grid = malloc(sizeof *this->grid * this->nx * this->ny)) );
    for (x = 0; x < this->nx; x++)
        for (y = 0; y < this->ny; y++)
        {
            room = getRoom(this, x, y);
            room->walls = NORTH + EAST + SOUTH + WEST;
            Maze_setMarker(room, Room_cleared);
        }
    this->start = this->finish = this->grid;
    created = true;

FINALLY:
    if (!created) Maze_free(&this);
    return this;
}

Room Maze_getRandomRoom(Maze this)
{
    int x, y;
    x = rand() % this->nx;
    y = rand() % this->ny;
    return getRoom(this, x, y); }

bool Maze_tunnel(Maze this, Room from, Room to)
{
    int fx, fy, tx, ty, direction;

    direction = 0;
    fx = getX(this, from); fy = getY(this, from);
    tx = getX(this, to);   ty = getY(this, to);
    if      (fx - 1 == tx && fy == ty) direction = NORTH;
    else if (fx + 1 == tx && fy == ty) direction = SOUTH;
    else if (fx == tx && fy - 1 == ty) direction = WEST;
    else if (fx == tx && fy + 1 == ty) direction = EAST;
    if (direction)
    {
        from->walls &= ~direction;
        to->walls &= ~(opposite(direction));
    }
    return direction != false;
}

void Maze_free(Maze *this)
{
    if (*this)
        free((*this)->grid);
    free(*this);
    *this = NULL;
}

#undef NORTH
#undef EAST
#undef SOUTH
#undef WEST

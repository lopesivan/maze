/**
 * \file maze.h
 * \author Matthew Eckert (mteckert@gmail.com)
 *
 * Maze ADT interface specification. This is the header file for a maze ADT. The
 * goal of this interface is to provide just enough functionality to allow for
 * recursive backtracking generation and solving techniques, with a bias toward
 * a command line interface. The ADT allows only movement in cardinal directions
 * and rectangular mazes. It uses a file specification that will be henceforth
 * known as the maze description file, or MDF.
 */

#ifndef MAZE_HEADER
#define MAZE_HEADER

#include "bool.h"

/**
 * These characters can be used to mark a room. They are defined purely for
 * convenience and should have no consequence in the implementation of the maze.
 * Other characters may be used freely instead.
 */
#define Room_cleared ' '
#define Room_visited 'O'
#define Room_deadend 'X'

/**
 * Opaque pointers that hold the storage data. This implementation provides two
 * conceptual data structures: the maze and the room. A maze is made up of many
 * rooms, and each room may be marked with a character. A room cannot be
 * directly accessed; it must be retrieved from one of the functions below that
 * operate on the maze.
 */
typedef struct room *Room;
typedef struct maze *Maze;

/**
 * Constructor that initializes a maze variable. This function prepares a maze
 * variable for importing or copying. Printing a maze that is not imported or
 * filled via some other method is undefined, but probably won't hurt anything.
 *
 * \return A new maze or, if not successfully allocated, NULL.
 */
Maze Maze_new(void);

/**
 * Imports a maze from an MDF. A maze description file is a text file with the
 * following format:
 *
 *     x  y
 *     sx sy
 *     fx fy
 *     r(0,0) r(0,1) r(0,2) ... r(0, y-1)
 *     r(1,0) r(1,1) r(1,2) ... r(1, y-1)
 *     .
 *     .
 *     .
 *     r(x,0) r(x,1) r(x,2) ... r(x-1, y-1);
 *
 *     where x and y are the height and width of the maze respectively, sx and
 *     sy are the coordinates for the start room of the maze, and fx and fy are
 *     the coordinates for the finish room of the maze. r(0,0) to r(x-1, y-1)
 *     are values describing a room's walls at a given coordinate in the maze.
 *
 * The wall values are described as north: 1, east: 2, south: 4, west: 8.
 *         1
 *       +---+
 *     8 |   | 2
 *       +---+
 *         4
 * For example, a room with a north and south wall would have a 5 as its value,
 * and the example MDF,
 *
 *               4  4
 *               3  2
 *               1  1
 *              11  9  5  7
 *              10  8  1  7
 *               8  4  4  3
 *              12  5  5  6
 *
 *       would look something like this when printed to a terminal:
 *
 *              +---+---+---+---+
 *              |   |           |
 *              +   +   +---+---+
 *              |   | F         |
 *              +   +   +   +---+
 *              |               |
 *              +   +---+---+   +
 *              |         S     |
 *              +---+---+---+---+
 *
 * \param this The maze to import to. Must have been initalized with Maze_new.
 * \param filename The filename of the MDF to read into the maze.
 *
 * \return true if the import was successful, false if not.
 */
bool Maze_import(Maze this, char *filename);

/**
 * Exports a maze to the MDF format. See the Maze_import documentation for the
 * description of this format. Note that this function may create or overwrite
 * any file that shares a name with its filename parameter. This function may
 * also leave a bad MDF behind if the export was unsuccessful.
 *
 * \param this The maze to export.
 * \param filename The filename of the file to export to.
 *
 * \return true if the export was successful, false if not.
 */
bool Maze_export(Maze this, char *filename);

/**
 * Performs a deep copy of a maze. This function copies all values, allocates
 * space for them, etc. This is not a shallow copy. The destination maze may
 * contain garbage if this function returns false, in which case it should be
 * freed with Maze_free.
 *
 * \param destination The destination maze to copy into.
 * \param source The source maze to copy from.
 *
 * \return true if the copy was successful, false if not.
 */
bool Maze_copy(Maze destination, Maze source);

/**
 * Prints a maze. This function displays the maze. This function has no other
 * requirements.
 *
 * \param this The maze to display.
 */
void Maze_print(Maze this);

/**
 * Gets the start room of the maze.
 *
 * \param this The maze to get the start room of.
 *
 * \return The start room.
 */
Room Maze_getStart(Maze this);

/**
 * Gets the finish room of the maze.
 *
 * \param this The maze to get the finish room of.
 *
 * \return The start room.
 */
Room Maze_getFinish(Maze this);

/**
 * Populates an array with rooms that are adjacent to a given room. This
 * function is the workhorse of this interface. It is useful for both solving
 * and generation. The array passed to this function must have space for at
 * least 4 rooms (one for each cardinal direction), though it may not fill each
 * slot in the array. The walls boolean parameter to this function signals
 * whether the array will contain rooms that have walls between them and the
 * original room or not. The rooms are added to the array contiguously. Unused
 * slots up to 4 are set to NULL.
 *
 * \param this The maze to get the adjacent rooms from.
 * \param room The room to get the adjacent rooms of.
 * \param adjacent The array which will be populated with adjacent rooms.
 * \param walls A boolean value; if true, the rooms in the array will have a
 *     wall between them and the origin room. If false, the rooms will not have
 *     a wall between them and the origin room.
 *
 * \return int The number of rooms added to the array.
 */
int Maze_getAdjacent(Maze this, Room room, Room *adjacent, bool walls);

/**
 * Sets the character marker associated with a room. This can be useful for
 * marking visited rooms or dead-ends. See the top of this file for convenient
 * predefined values for this function.
 *
 * \param room The room to set the character marker of.
 * \param marker The character marker.
 */
void Maze_setMarker(Room room, char marker);

/**
 * Gets the character marker associated with a room.
 *
 * \param room The room to get the character marker of.

 * \return The character marker.
 */
char Maze_getMarker(Room room);

/**
 * Replaces every marker of some value in each room of the maze with a different
 * marker.
 *
 * \param this The maze to operate on.
 * \param find The character marker that is checked for.
 * \param replace The character marker that is applied.
 */
void Maze_replaceMarkers(Maze this, char find, char replace);

/*****************************
 * GENERATION ONLY FUNCTIONS *
 *****************************/

/**
 * A constructor like Maze_new that fills the maze with rooms that have all
 * their walls up.
 *
 * \param x The height of the maze.
 * \param y The width of the maze.
 *
 * \return A new maze with x height and y width, with rooms that have all four
 *     walls.
 */
Maze Maze_newFilled(int x, int y);

/**
 * Breaks the wall between two rooms. If the rooms are not adjacent or there is
 * no wall between them, this function returns false.
 *
 * \param this The maze to operate on.
 * \param from The room to check.
 * \param to The other room to check.
 *
 * \return true if a wall was broken between the rooms, false if not.
 */
bool Maze_tunnel(Maze this, Room from, Room to);

/**
 * Gets a random room in the maze.
 *
 * \param this The maze to operate on.
 *
 * \return A random room from the maze.
 */
Room Maze_getRandomRoom(Maze this);

/**
 * Sets the start room of the maze. Note that the easiest way to set the start
 * room provided by this interface is Maze_getRandomRoom.
 *
 * \param this The maze to set the start room of.
 * \param room The room to set the start room to.
 */
void Maze_setStart(Maze this, Room room);

/**
 * Sets the finish room of the maze. Note that the easiest way to set the finish
 * room provided by this interface is Maze_getRandomRoom.
 *
 * \param this The maze to set the finish room of.
 * \param room The room to set the finish room to.
 */
void Maze_setFinish(Maze this, Room room);

/**
 * Frees the Maze and sets the variable to NULL. This function is safe to call
 * on all initialized maze variables.
 *
 * @param this The maze to free.
 */
void Maze_free(Maze *this);

#endif

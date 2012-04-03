/**
 * \file try.h
 * \author Matthew Eckert (mteckert@gmail.com)
 *
 * Macro for requirements checking and cleanup. This macro is used expressly for
 * cleanup of resources should the state of a given function not meet some
 * requirement. With the DEBUG preprocessor macro defined, this function will
 * also print diagnostic information to the standard error stream.
 *
 * USAGE:
 *
 *         TRY( (char_this = malloc(500)) );
 *         ... other code that can assume char_this malloc was successful ...
 *
 *     FINALLY:
 *         return(char_this); <-- will return null immediately if malloc failed
 *
 * A program will not compile if there is no FINALLY label in the function where
 * this macro is used. FINALLY should be actually mark the final block of code,
 * where the function cleans up resources and returns its data.
 *
 * \warning Calling TRY within FINALLY is considered harmful.
 */

#ifndef TRY_HEADER
#define TRY_HEADER

#ifdef DEBUG

#define TRY(EXP) do { if (!(EXP)) { \
    fprintf(stderr, "%s:%i: TRY FAILED: !(%s)\n", __FILE__ , __LINE__ , #EXP );\
    goto FINALLY; \
} } while (0)

#else

#define TRY(EXP) do { if (!(EXP)) { goto FINALLY; } } while (0)

#endif /* DEBUG */

#endif /* TRY_HEADER */

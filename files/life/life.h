#ifndef LIFE_H_
#define LIFE_H_

#include <stdio.h>

/*
 * This file is a port to C by Luther Tychonievich  of a file written in C++ 
 * by Charles Reiss.
 * 
 * This file is released as part of a programming assignment and MUST NOT
 * be shared or replicated except within a single student's private workspace
 * and class submission space. If it is found, in whole or in part, to have been
 * shared outside of the class, the sharing is in violation of this license
 * agreement and will be prosecuted accordingly.
 */

///
typedef unsigned char LifeCell;

/**
 * Holds the state of a matrix of life cells.
 *
 * Each cell is represented by a LifeCell which is either 0 or 1.
 *
 * To produce the simulation, multiple of these are used since we need
 * the current state to produce the next state.
 */
typedef struct {
    int width, height;
    LifeCell *cells;
} LifeBoard;

/** Given a board, resize and clear it as needed */
void LB_initialize(LifeBoard *self, int w, int h);
/** Given a board, reduce it to 0-by-0 and reclaim heap memory */
void LB_clear(LifeBoard *self);

/**
 * malloc and initialize; should be followed by LB_del to avoid memory leak
 */
LifeBoard *LB_new(int w, int h);
/**
 * malloc and initialize to match provided board; 
 * should be followed by LB_del to avoid memory leak
 */
LifeBoard *LB_clone(const LifeBoard *other);

/** clear and then free() */
void LB_del(LifeBoard *self);

/** Access a particular cell of a board */
LifeCell LB_get(const LifeBoard *self, int x, int y);
/** Set a particular cell of a board */
void LB_set(LifeBoard *self, int x, int y, LifeCell value);
/** Swaps the contents of two LifeBoards */
void LB_swap(LifeBoard *self, LifeBoard *other);

/** Check all cells for equality; return 1 if equal, 0 otherwise */
int LB_equals(const LifeBoard *self, const LifeBoard *other);

/** Print the board */
void LB_display(const LifeBoard *self, FILE *to);
/** Import a printed board into an existing board */
void LB_load(LifeBoard *self, FILE *from);
/** Import a printed board into a new board */
LifeBoard *LB_import(FILE *from);

/**
 * Simulates the specified number of steps by the Game of Life rules,
 * updating the LifeBoard state to have the new state.
 *
 * Two versions: a serial version and a parallel version you must implement.
 */ 
extern void simulate_life_serial(LifeBoard *state, int steps);
/// ditto
extern void simulate_life_parallel(int threads, LifeBoard *state, int steps);

#endif

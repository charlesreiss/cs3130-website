#include "life.h"

/*
 * This file is a port to C by Luther Tychonievich  of a file written in C++ 
 * by Charles Reiss.
 * 
 * This file is released as part of a programming assignment and MUST NOT
 * be shared or replicated except within a single students' private workspace
 * and class submission space. If it is found, in whole or in part, to have been
 * shared outside of the class, the sharing is in violation of this license
 * agreement and will be prosecuted accordingly.
 * 
 * See life.h for documentation of this function.
 */


void simulate_life_serial(LifeBoard *state, int steps) {
    LifeBoard *next_state = LB_new(state->width, state->height);
    
    for (int step = 0; step < steps; step += 1) {

        /* We use the range [1, width - 1) here instead of
         * [0, width) because we fix the edges to be all 0s.
         */
        for (int y = 1; y < state->height - 1; y += 1) {
            for (int x = 1; x < state->width - 1; x += 1) {
                
                /* For each cell, examine a 3x3 "window" of cells around it,
                 * and count the number of live (true) cells in the window. */
                int live_in_window = 0;
                for (int y_offset = -1; y_offset <= 1; y_offset += 1)
                    for (int x_offset = -1; x_offset <= 1; x_offset += 1)
                        if (LB_get(state, x + x_offset, y + y_offset))
                            live_in_window += 1;
                
                /* Cells with 3 live neighbors remain or become live.
                   Live cells with 2 live neighbors remain live. */
                LB_set(next_state, x, y,
                    live_in_window == 3 /* dead cell with 3 neighbors or live cell with 2 */ ||
                    (live_in_window == 4 && LB_get(state, x, y)) /* live cell with 3 neighbors */
                );
            }
        }
        
        /* now that we computed next_state, make it the current state */
        LB_swap(state, next_state);
    }
    LB_del(next_state);
}

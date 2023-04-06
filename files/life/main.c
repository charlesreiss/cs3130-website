#include "life.h"
#include "timing.h"
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

/** Global variables to manage wrapping functions without arguments */
static LifeBoard *original_board;
static LifeBoard *working_board;
static LifeBoard *target_board;
static int global_steps;

/** Global variables to track errors in computation */
static int function_index;
static int error_count = 0;
static int show_result_on_error = 0;


/** Wrapper for timing code */
void serial() {
    simulate_life_serial(working_board, global_steps);
}
/** Wrapper for timing code */
void threads_1() {
    simulate_life_parallel(1, working_board, global_steps);
}
/** Wrapper for timing code */
void threads_2() {
    simulate_life_parallel(2, working_board, global_steps);
}
/** Wrapper for timing code */
void threads_4() {
    simulate_life_parallel(4, working_board, global_steps);
}
/** Wrapper for timing code */
void threads_8() {
    simulate_life_parallel(8, working_board, global_steps);
}


typedef void (*vfv)(void);

/** Array of function pointers to test and time */
vfv test_functions[] = {
    serial,
    /* UNCOMMENT THIS WHEN YOU'VE IMPLEMENTED PARALLEL VERSION
    threads_1,
    threads_2,
    threads_4,
    threads_8,
    */
};
/** Display names for functions (must match above array) */
const char *function_names[] = {
    "serial",
    "1 thread",
    "2 threads",
    "4 threads",
    "8 threads",
};


/** Makes a working board from the starting board */
void setup() {
    working_board = LB_clone(original_board);
}
/** Verifies correctness and frees the working board */
void teardown() {
    if (!LB_equals(working_board, target_board)) {
        fprintf(stderr, "ERROR: %s result disagreed with expected results\n", function_names[function_index]);
        if (!error_count && show_result_on_error) {
            fprintf(stderr, "INCORRECT RESULTS from %s:\n", function_names[function_index]);
            LB_display(working_board, stderr);
        }
        error_count += 1;
    }
    LB_del(working_board);
    working_board = NULL;
}

/* Given a number return a string using T, M, K to describe in a more readable way 
 * Uses global array (not thread-safe) */
char * pretty_number(uint64_t cycles) {
    static char backing[64];
    const uint64_t kilo = 1000;
    const uint64_t mega = 1000 * kilo;
    const uint64_t giga = 1000 * mega;
    const uint64_t tera = 1000 * giga;
    if (cycles > tera * 10) {
        snprintf(backing, sizeof(backing)-1, "%"PRIu64"T", cycles/tera);
    } else if (cycles > mega * 10) {
        snprintf(backing, sizeof(backing)-1, "%"PRIu64"M", cycles/mega);
    } else if (cycles > kilo * 10) {
        snprintf(backing, sizeof(backing)-1, "%"PRIu64"K", cycles/kilo);
    } else {
        snprintf(backing, sizeof(backing)-1, "%"PRIu64, cycles);
    }
    return backing;
}


int main(int argc, const char **argv) {
    // parse command-line options
    int show_result = 0;
    int do_timings = 0;
    int iterations = 0;
    const char *input_file;
    if (argc == 4) {
        iterations = atoi(argv[1]);
        input_file = argv[2];
        if (strcmp(argv[3], "serial-result") == 0) {
            show_result = 1;
        } else if (strcmp(argv[3], "time") == 0) {
            do_timings = 1;
        } else if (strcmp(argv[3], "time-and-result") == 0) {
            show_result = 1;
            show_result_on_error = 1;
            do_timings = 1;
        }
    }
    
    // show usage message if that fails
    if (!show_result && !do_timings) {
        puts("USAGE:");
        printf("  %s ITERATIONS FILENAME serial-result\n", argv[0]);
        puts("    Run serial computation and print out result");
        printf("  %s ITERATIONS FILENAME time\n", argv[0]);
        puts("    Run all versions and compare their timings");
        printf("  %s ITERATIONS FILENAME time-and-result\n", argv[0]);
        puts("    Print out result and compare timings of all versions");
        puts("\nExample:");
        printf("  %s 10 input/make-a serial-result", argv[0]);
        puts("   [run with 10 replaced by 1, 2, 3, 4, ... to see animated example]\n");
        return 1;
    }
    
    // load the starting board from a file
    FILE *in = fopen(input_file, "r");
    if (!in) {
        fprintf(stderr, "ERROR: could not open input file \"%s\"\n", input_file);
        return 1;
    }
    original_board = LB_import(in);
    fclose(in);
    if (original_board->width <= 0 || original_board->height <= 0) {
        fprintf(stderr, "ERROR: valid initial board not found in \"%s\"\n", input_file);
        return 1;
    }
    
    target_board = LB_clone(original_board);
    simulate_life_serial(target_board, iterations);
    if (show_result)
        LB_display(target_board, stdout);
    
    int exit_code = 0;
    if (do_timings) {
        puts("---TIMING---");
#if defined(__SANITIZE_ADDRESS__) || defined(__SANITIZE_THREAD__)
        puts("NOTE: running with AddressSanitizer or ThreadSanitizer\n"
             "      so times reported below are not representative.");
#endif
        int n = sizeof(test_functions) / sizeof(test_functions[0]);
        for(function_index=0; function_index<n; function_index+=1) {
            // run once without timing to see if it works
            error_count = 0;
            global_steps = iterations;
            setup();
            test_functions[function_index]();
            teardown();
            if (error_count != 0) {
                exit_code = 1;
                printf("[BROKEN] %s: [not timed]\n", function_names[function_index]);
                continue; // don't time broken code
            }

            cycles_type cycles = measure_function(test_functions[function_index], setup, teardown);
            if (error_count) {
                exit_code = 1;
                printf("[BROKEN] %s: %" PRIu64 "? cycles (%s?)\n",
                    function_names[function_index],
                    cycles,
                    pretty_number(cycles)
                );
            } else {
                printf("%s: %" PRIu64 " cycles (%s)\n",
                    function_names[function_index],
                    cycles,
                    pretty_number(cycles)
                );
            }
            
        }
    }
    
    LB_del(target_board);
    LB_del(original_board);
    return exit_code;
}

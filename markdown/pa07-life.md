---
title: Parallel Game of Life
author:
    - Charles Reiss
    - Luther Tychonievich. 
...

# Your Task

## Before you code

Examine the supplied [skeleton code](files/life-c-skeleton.tar.gz) which implements [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway's_Game_of_Life).
You should only edit `life-parallel.c`, but that interacts with many other files in the package.

Make sure you look at `input/README.md`, as well as `life.h` and any other files you wish.

The provided code will compile and run.
We encourage experimenting with it before adding any code of your own.

{.aside ...} Address sanitizer on department machines

Some students have reported that `clang` fails to build the `life-asan` version once they add threading.
There are at least two known workarounds.

- Change the makefile to use `gcc` instead of `clang`, and `module load gcc`{.sh} instead of `module load clang-llvm`{.sh}.

- Use `make life`{.sh} instead of just `make`, so that `make` builds only the non-address-sanitizer version.

I've asked the systems staff to add threaded asan for clang, but it is not yet clear when that will be done.
{/}

Make sure you understand how `simulate_life_serial` works.
You'll be making a (more complicated) parallel version of this, and are unlikely to be successful if you don't understand this starting point.

## Implement parallel Life

Create a parallel version (using pthreads) in `life-parallel.c`.
You must use the provided header

````c
void simulate_life_parallel(int threads, LifeBoard &state, int steps)
````

`life-serial.c` contains a correct, working single-threaded implementation you are welcome to borrow from. 
You should write helper functions as appropriate to keep your code well-organized.


Additional constraints:

1. Your implementation must be correct (result in the expected final board configuration) for all boards and iteration counts.

1. You must call `pthread_create` exactly `threads` times. Do not create more or fewer threads, nor create new threads for each iteration of the simulation.

1. We must be able to call `simulate_life_parallel` from multiple threads concurrently. Do not use global variables or other thread-unsafe practices.

1. You must have no memory leaks or other memory errors. The provided `Makefile` builds a version with the address sanitizer enabled automatically as `life-asan`; this must run without errors.

1. You must call the appropriate `pthread_***_destroy` for each `pthread_***_init` you call to reclaim any pthreads-allocated memory.

1. On a multi-core machine, your parallel code must be noticeably faster than the serial code when run on sufficiently large boards for sufficiently many iterations.

1. And, of course, you are still bound by all the usual [course policies](policies.html#write-your-own-code).

## Test your code.

Uncomment the lines of `main.c` that are marked as appropriate to uncomment after `simulate_life_parallel` is written. Then test your code, as e.g. by running

- `./life`{.bash} to see the usage hints.

- `life-asan 10 input/make-a time`{.bash} to check for memory leaks (but do not trust its timing, the address sanitizer slows things down a lot).

- `./life 0 input/o0075 serial-result`{.bash} to ensure you can load an example file.

- `./life 110 input/o0075 serial-result`{.bash} to ensure you can simulate an example file.

- `./life 10 input/make-a time`{.bash} to time the `make-a` file with 10 steps.

- `./time-examples.sh`{.bash} to get a sense of how you are doing on parallel performance.

# Tips

1.
    This assignment was designed to be a natural fit for [barriers](pthreads.html#barrier). You are strongly encourage to get a barrier-based solution working first before attempting any other approaches.

1.
    You should choose some way to split up the grid between threads. You will get best performance if each thread works on a part of the grid that is contiguous in memory, so you get better locality within a thread. This also will avoid performance problems associated with two processors trying to modify data in different parts of the same cache block.

1.
    To compute the value of a cell in generation $i$, one needs the value of its neighbors in generation $i-1$. Barriers are one way to make sure that the values from generation $i-1$ are available before any thread starts computing generation $i$.

1.
    The supplied code calls `LB_swap()` to exchange boards. If one uses that code as is, one must ensure that all threads stop accessing the boards before the swap happens and don’t start accessing the boards again until the swap finishes.

    An alternative, which calls wait on barriers fewer times each iteration, is to avoid swapping by having an “even” state and “odd” state and choose which board to write to based on the generation number. (In even iterations, threads would use the “odd” version of the state to write to the “even” state; and in odd iterations, threads would use the “even” version of the state to write to the “odd” state.) In this way, rather than waiting for the swap to finish before starting the next generation, threads just need to wait for all other threads to have finished the current generation.

1.
    You should be able to reuse almost all of the `simulate_life_serial` code. You will probably need to split it into at least two functions — one that represents the work done in separate threads and one that spawns the threads and waits for them to finish.

1.
    If you find yourself tempted to use a global variable (such as a global mutex or barrier), you can usually get away with adding it as a field of a `struct` passed in by reference to your thread function's `void *` parameter instead.

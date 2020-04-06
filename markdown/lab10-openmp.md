---
title: OpenMP -- parallelism made easy
...

# Introduction

OpenMP is designed to make common kinds of speed-oriented parallelism simple and painless to write.
It uses a special set of C pre-processor directives called "pragmas" to annotate common programming constructs and have then transformed into threaded versions.
It also automatically picks a level of parallelism that matches the number of cores available.

## Set-up

Your compiler has to be built with OpenMP support for OpenMP to work.
On `portal.cs.virginia.edu`, the GCC compiler has been built that way but the CLang compiler has not, so you'll need to use `gcc`{.sh}, not `clang`{.sh}, to compile code for this lab if you use portal. Note this requires `module load gcc`{.sh}.

To verify your set-up works, try the following `test.c`:

```c
#include <stdio.h>
#include <omp.h>

int main() {
    #pragma omp parallel
    puts("I'm a thread");
    puts("non-threaded");
}
```

Compile with `gcc -fopenmp test.c`{.sh} and run as `./a.out`{.sh}; if it worked, you'll see multiple `I'm a thread` lines printed out.

If you compile without the `-fopenmp` flag it will run on just one thread and print our `I'm a thread` just once.

We won't use optimization flags in this lab because they can turn some of our test cases into single-statement assembly.

Note that an OpenMP `#pragma` applies to the subsequent statement, only.
Thus in the above `puts("I'm a thread");` is threaded but `puts("non-threaded");` is not.
If you want several statements to be parallelized, you'd put them in braces to combine them into one block statement, as e.g.

```c
#pragma omp parallel
{
    puts("I'm a thread");
    puts("Also a thread");
}
puts("but not this one");
```

# Task

{.exercise ...}
Work with a partner to use OpenMP to parallelize the [Starter code](#starter-code).
You should be able to understand all of the code provided, but will only need to work on the `geomean` function for this lab.

To run the code, compile with the `-lm` flag and give it file names as command-line arguments. It will return the number of bytes in those files and the geometric mean value of those bytes. You can provide multiple files on the command line if you wish; all will be combined before analysis. Parallelism give the biggest benefit when given large inputs, so you might want to try some larger files: for example, on portal `/usr/bin/emacs-24.3` has more than 14 million characters.

    $ gcc -lm -fopenmp lab10starter.c
    $ ./a.out /usr/bin/emacs-24.3
    313710435 nanoseconds to process 14784560 characters: geometric mean is 6.46571


To parallelize, separate the code into Map and Reduce steps and trying several OpenMP parallelizations; keep track of which was best.

Then explain to a TA which approach was fastest and your guess as to why.
{/}


# The Map-Reduce paradigm

Data races are a major limiting factor on parallel computation.
Synchronization can remove races, but at the cost of reduced parallelism.
Several programming patterns can avoid these problems;
one of the most popular is the map-reduce paradigm.

Map-reduce works as follows

1. Map: Turn an array of input values into an array of output values. Ensure that each output value is independent of the others.

2. Reduce: combine an array of values into a single value.

In both cases, the "array" might be implicitly defined; for example, the array of integers from 0 to 10000 could be implicit in the existence of a for loop.

Many problems that take large amounts of data as input can be re-posed as a map, followed by a reduce.
Both Map and Reduce can be efficiently parallelized

## Parallel Map

There are two main ways to parallelize Map:
one that is easy but assumes every piece of work is of similar difficulty
and another that is a bit trickier but allows for some tasks to be much harder than others efficiently.

### Even split

If your serial code looks like

```c
for(int i=0; i<N; i+=1) {
    // ...
}
```

then the parallel code

```c
#pragma omp parallel for
for(int i=0; i<N; i+=1) {
    // ...
}
```

works my splitting up the `0`--`N` range into a number of chunks equal to the number of threads.
For example, if it uses 4 threads then they will get the following range of `i`:

- `0`--`N/4`
- `N/4`--`N/2`
- `N/2`--`3*N/4`
- `3*N/4`--`N`

**OpenMP pragmas used**:

- `#pragma omp parallel for`{.c} means "have each thread run the next statement (a `for`{.c} loop) with different bounds".


### Task queue

If your serial code looks like

```c
for(int i=0; i<N; i+=1) {
    // work on item i
}
```

then the parallel code

```c
int j = 0;
#pragma omp parallel
while (1) {
    int i;
    #pragma omp atomic capture
    i = j++;
    if (i >= N) break;
    // work on item i
}
```

has each thread atomically update a shared counter until it gets too big.
This ensures that if some threads take a long time in the `// work on item i`{.c} of the other threads can still progress.
It has more overhead than the previous method, though, as atomic actions are slower than non-atomic actions.

**OpenMP pragmas used**:

- `#pragma omp parallel`{.c} means "have each thread run the next statement (the `while`{.c} loop)".

- `#pragma omp atomic capture`{.c} means "the next statement (`i = j++`{.c}) is a capture-type statement and needs to run atomically". There are other atomic statement types; see <https://www.openmp.org/spec-html/5.0/openmpsu95.html>.

## Parallel Reduce

There are two main ways to parallelize Reduce:
either use an atomic operation
or do partial reductions in each of several threads and then combine them all in oen thread afterwards.

### Atomic reduction -- non-parallel

The simplest way to reduce is to make the reduction step an `#pragma opm atomic`{.c} of some type (usually `update`; see <https://www.openmp.org/spec-html/5.0/openmpsu95.html>).
This greatly limits the performance value of parallelism, so it's not recommended in general.

### Many-to-few reduction -- atomic version

An alternative approach is to to reduce an array of *N* values into an array of *n* values, where *n* is the number of threads; then have one thread further reduce the *n* to 1.

Given reduction code

```c
my_type result = zero_for_my_type;
for(int i=0; i<N; i+=1) {
    result op= array[i];
}
```

(where `op=` is some kind of augmented assignment, like `+=` or `*=`)
then the parallel code

```c
my_type result = zero_for_my_type;
#pragma omp parallel
{
    my_type local_result = zero_for_my_type;
    #pragma omp for nowait
    for(int i=0; i<N; i+=1) {
        local_result op= array[i];
    }
    
    #pragma omp atomic update
    result op= local_result;
}
```

will have each thread to its share of reductions on its own local copy of the result,
then atomically update the shared total


**OpenMP pragmas used**:

- `#pragma omp parallel`{.c} means "have each thread run the next statement (the block in `{ ... }`)".

- `#pragma omp for nowait`{.c} means "the next statement (a `for` loop) should have its bounds adjusted depending on which thread is running it." It's like the `#pragma omp parallel for`{.c} discussed under the [Even split](#even-split) section above, but instead of creating new threads it uses those already existing.
    
    The `nowait` means if one thread finishes before another, it can move on to post-`for`-loop code without waiting for the others to finish.

- `#pragma omp atomic capture`{.c} means "the next statement (`result op= local_result`{.c}) is an "update" type statement and needs to run atomically". There are other atomic statement types; see <https://www.openmp.org/spec-html/5.0/openmpsu95.html>.


### Many-to-few reduction -- array version

If our reduction operations is more complicated than a single atomic operation can support, we can store the threads' intermediate results in an array.

Given reduction code

```c
my_type result = zero_for_my_type;
for(int i=0; i<N; i+=1) {
    // arbitrary code to add array[i] to result
}
```

then the parallel code

```c
// find out how many threads we are using:
#ifdef OPENMP_ENABLE
    #pragma omp parallel
        #pragma omp master    
            int threads = omp_get_num_threads();
#else
    int threads = 1;
#endif

my_type *results = (my_type *)malloc(threads * sizeof(my_type));

#pragma omp parallel
{
#ifdef OPENMP_ENABLE
    int myid = omp_get_thread_num();
#else
    int myid = 0;
#endif
    
    results[myid] = zero_for_my_type;
    #pragma omp for nowait
    for(int i=0; i<N; i+=1) {
        // arbitrary code to add array[i] to results[myid]
    }
}

my_type result = zero_for_my_type;
for(int i=0; i<threads; i+=1) {
    // arbitrary code to add results[i] to result
}
```

will have each thread to its share of reductions on its own local copy of the result,
then have one thread update them all.


**OpenMP pragmas, macros, and functions used**:

- `#pragma omp master`{.c} means "only one thread (called the master thread) gets to run this".

- `#ifdef OPENMP_ENABLE`{.c} means "only if `-fopenmp` was provided at compile time"

- `omp_get_num_threads()`{.c} returns the number of threads OpenMP has running.

- `omp_get_thread_num()`{.c} returns the index of this thread (between 0 and the number of threads)

- `#pragma omp parallel`{.c} means "run the next statement in multiple threads"

- `#pragma omp for nowait`{.c} means "the next statement (a `for` loop) should have its bounds adjusted depending on which thread is running it." It's like the `#pragma omp parallel for`{.c} discussed under the [Even split](#even-split) section above, but instead of creating new threads it uses those already existing.
    
    The `nowait` means if one thread finishes before another, it can move on to post-`for`-loop code without waiting for the others to finish.

# Starting code

```c
#include <stdio.h> // fopen, fread, fclose, printf, fseek, ftell
#include <math.h> // log, exp
#include <stdlib.h> // free, realloc
#include <time.h> // struct timespec, clock_gettime, CLOCK_REALTIME
#include <errno.h>


// computes the geometric mean of a set of values.
// You should use OpenMP to make faster versions of this.
// Keep the underlying sum-of-logs approach.
double geomean(unsigned char *s, size_t n) {
    double answer = 0;
    for(int i=0; i<n; i+=1) {
        if (s[i] > 0) answer += log(s[i]) / n;
    }
    return exp(answer);
}

/// returns the number of nanoseconds that have elapsed since 1970-01-01 00:00:00
long long nsecs() {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return t.tv_sec*1000000000 + t.tv_nsec;
}


/// reads arguments and invokes geomean; should not require editing unless you rename geomean
int main(int argc, char *argv[]) {
    // step 1: get the input array (the bytes in this file)
    char *s = NULL;
    size_t n = 0;
    for(int i=1; i<argc; i+=1) {
        // add argument i's file contents (or string value if no file found) to s
        FILE *f = fopen(argv[i], "rb");
        if (f) { // was a file; read it
            fseek(f, 0, SEEK_END); // go to end of file
            size_t size = ftell(f); // find out how many bytes in that was
            fseek(f, 0, SEEK_SET); // go back to beginning
            s = realloc(s, n+size); // make room
            fread(s+n, 1, size, f); // append this file on end of others
            fclose(f);
            n += size; // not new size
        } else { // not a file; treat as a string
            errno = 0; // clear the read error
        }
    }

    // step 2: invoke and time the geometric mean function
    long long t0 = nsecs();
    double answer = geomean(s,n);
    long long t1 = nsecs();

    free(s);

    // step 3: report result
    printf("%lld nanoseconds to process %zd characters: geometric mean is %g\n", t1-t0, n, answer);
}
```

---
title: Using POSIX Threads
...

This document is intended to give a practical overview of using pthreads.
It is leaving out a lot of details.

For brevity, code in this document does not check error codes.
This is **bad coding practice**!
You should check error codes in your code.

# Jumping-off point

For those eager to start coding, here's a parallel summation program you might find to be a useful starting point.

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* Allow compiler -DTHREADCOUNT=4 but have a default */
#ifndef THREADCOUNT
#define THREADCOUNT 16
#endif

/** Defines a particular task to handle */
typedef struct {
    size_t from;
    size_t to;
    double (*getnum)(size_t);
} task_description;

/**
 * Function invoked by each new thread.
 * The argument must be a task_description *;
 * the return value is a malloced double *.
 */
void *sum_array(void *args) {
    task_description *task = (task_description *)args;
    printf("  Summing from %zd to %zd...\n", 
        task->from, task->to);
    double work = 0;
    for(size_t i=task->from; i<task->to; i+=1) {
        work += task->getnum(i);
    }
    printf("  ... sub-sum from %zd to %zd = %g\n", 
        task->from, task->to, work);
    double *sum = malloc(sizeof(double));
    *sum = work;
    return (void *)sum;
}

/** A simple reciprocal function */
double fraction(size_t i) {
    return 1.0/(i+1);
}

/**
 * Sum all fractions 1/n from 1 to pow(2,-28)
 * in THREADCOUNT parallel threads
 */
int main(int argc, const char *argv[]) {
    // set up task sizes to take a few seconds on 2020-era laptops
    size_t max = 1<<28;
    size_t step = max / THREADCOUNT;
    
    // store per-thread information (don't re-use, memory is shared)
    pthread_t id[THREADCOUNT];
    task_description tasks[THREADCOUNT];
    
    // spawn the threads
    for(int i=0; i<THREADCOUNT; i+=1) {
        tasks[i].from = i*step;
        tasks[i].to = (i+1)*step;
        tasks[i].getnum = fraction;
        pthread_create(id+i, NULL, sum_array, tasks+i);
    }

    // wait for and combine the results
    double result = 0;
    for(int i=0; i<THREADCOUNT; i+=1) {
        void *ans;
        pthread_join(id[i], &ans);
        result += *(double *)ans;
        free(ans); // was malloced in just-joined thread
    }

    printf("The sum is %g\n", result);
    return 0;
}
```

To see the time impact of threading, try comparing

```bash
clang -lpthread -O3 -DTHREADCOUNT=8 thiscode.c && time ./a.out
```

and
```bash
clang -lpthread -O3 -DTHREADCOUNT=1 thiscode.c && time ./a.out
```


# Managing thread existance

Every process has at least one thread, the one that invoked `main`.
Each other thread is created by invoking a system call, wrapped by the various `pthread_` library functions.

## `pthread_create`

The library function `pthread_create` makes a new thread.
It is given four arguments:

Type                        Kind    Purpose
--------------------------  ------  -------------------------------------------
`pthread_t *`               output  Set to the ID of the created thread
`const pthread_attr_t *`    input   Rules about how the new thread will behave
`void *(*)(void *)`         input   Pointer to a function the new thread runs
`void *`                    input   Value passed as argument to new thread

### `pthreads_attr_t`

The second argument of `pthread_create`
is used to control how the thread behaves.
Much of this is fairly specialized, and passing `NULL` will work in many cases.
If you want more control, though, you need to use a few extra functions to gain such.

A `pthread_attr_t` must be initialized using `pthread_attr_init` before invoking `pthread_create` and cleaned up using `pthread_attr_destroy` afterwards.
`pthread_attr_init` is permitted to `malloc` fields inside the `pthread_t`
and if it did, `pthread_attr_destroy` will `free` them.


<div class="example long">
The following is a skeleton of how to create a thread.

```c
pthread_attr_t attr;
pthread_attr_init(&attr);
pthread_t id;
void *argument = /* ... initialize this here */
/* ... use various pthread_attr_setXXXX functions to set behavior ... */
pthread_create(&id, &attr, run_this, argument);
pthread_attr_destroy(&attr);
```

</div>


Many of the attributes that can be placed into a `pthread_attr_t`
have to do with scheduling priority (how often the thread gets CPU time)
and stack organization (how large, etc, the thread's stack is)
and can be ignored by the casual thread user.
However, one (the detached or joinable state of the thread) is important enough to deserve its own section.

## `pthread_attr_setdetachstate` and `pthread_join`

Every created thread is either detached or joinable.

`pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE)`
:   A joinable thread (the default if not otherwise set)
    will, when it exits, continue to exist until `pthread_join`
    is called to retrieve its exist status and reclaim its resources.

`pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)`
:   A detached thread will, when it exists,
    immediately be reclaimed by the OS.
    Its exit status is lost,
    and other threads cannot use `pthread_join` to wait for it to terminate.

### Return values

If a thread with `pthread_t id` is joinable,
then invoking `pthread_join(id, &retval)`{.c}
will cause the invoking thread to suspend operation
until thread `id` terminates;
when thread `id` terminates, `retval` stores the results of the thread's computation.

The results is one of

- The return value of the start function of the terminated thread.
- The argument passed into `pthread_exit` to terminate the thread early.
- The special value `PTHREAD_CANCELED` if the thread was stopped by another thread.

If you have a joinable thread, you need to join it before exiting.
If you have detached threads, you cannot wait for them to terminate
and the program will exit when the main thread does.

## Crashing

What happens if one thread crashes?
Since a crash means an unhandled signal,
and since the behavior of an unhandled signal is to terminate the process,
the whole program crashes.

However, signals are delivered to specific threads,
so if you add a signal handler it will be run by the thread
that the OS believes is the recipient of the signal.

## Debugging

Debugging threaded programs can be tricky.
Debuggers like `lldb` work fine on multithreaded programs,
but with multiple threads there is more information to display.
Additionally, some bugs (e.g., race conditions and deadlock) can depend on exact instruction scheduling, which may be different in a debugger than when run normally.

We will not have time in this course to dive into multithreaded debugging in any great detail.

# Synchronization with pthreads

## Mutex

Recall that a mutex only lets one thread have it locked at a time,
excluding others until it it unlocked.

```c
pthread_mutex_t mutex;

void *thread_function(void *) {
    /* ... */
    pthread_mutex_lock(&mutex);
    /* only one thread can get here at a time */
    pthread_mutex_unlock(&mutex);
    /* ... */
}

int main(int argc, const char *argv[]) {
    /* ... */
    pthread_mutex_init(&mutex, NULL);
    for(int i=0; i<THREADCOUNT; i+=1) {
        /* ... */
        pthread_create(&id[i], NULL, thread_function, &arg[i]);
    }
    /* ... */
}
```

See also `pthread_mutex_trylock` for the "acquire if possible" behavior.


## Barrier

Recall that a barrier acts like a meet-up: no one moves until everyone expected arrives.
In pthreads, that "everyone" criterion is determined by a count: once that number of threads arrive, all will be allowed to proceed.

```c
pthread_barrier_t barrier;

void *thread_function(void *) {
    /* ... */
    /* each thread reaches here at its own time */
    pthread_barrier_wait(&barrier);
    /* all threads proceed from here together */
    /* ... */
}

int main(int argc, const char *argv[]) {
    /* ... */
    pthread_barrier_init(&barrier, THREADCOUNT);
    for(int i=0; i<THREADCOUNT; i+=1) {
        /* ... */
        pthread_create(&id[i], NULL, thread_function, &arg[i]);
    }
    /* ... */
}
```

## Reader-writer lock

Recall that a reader-writer lock has two modes:
either it can be used by exactly one writer (like a mutex)
or by any number of readers (like unsynchronized data)
but not both at once.

The relevant functions are documented in the following manual pages:

- `pthread_rwlock_init` -- this is complicated because they have multiple attributes to handle how they handle if a writer is waiting for the readers to finish and a new reader arrives.
- `pthread_rwlock_rdlock` and `pthread_rwlock_wrlock` -- acquire the lock in two different ways.
- `pthread_rwlock_timedrdlock` and `pthread_rwlock_timedwrlock` -- try to acquire the lock, but if a specified time passes without success return an error code instead.
- `pthread_rwlock_unlock` -- release the lock (no matter how it was acquired).

However, these details aside the overall usage looks similar to how [mutexes](#mutex) are used with pthreads.

---
title: Dining Philosophers
...

# The situation

A famous problem in concurrency and synchronization is the [Dining Philosophers problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem).
An example C implementation of this problem looks like

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_t philosopher[5];
pthread_mutex_t chopstick[5];

void *eat(void *arg) {
    int n = (int)arg;
    // take two chopsticks
    pthread_mutex_lock(&chopstick[n]);
    printf("Philosopher %d got chopstick %d\n", n, n);
    pthread_mutex_lock(&chopstick[(n+1)%5]);
    printf("Philosopher %d got chopstick %d\n", n, (n+1)%5);
    
    printf ("Philosopher %d is eating\n",n);
    sleep(1);
    
    // set them back down
    printf("Philosopher %d set down chopstick %d\n", n, (n+1)%5);
    pthread_mutex_unlock(&chopstick[(n+1)%5]);
    printf("Philosopher %d set down chopstick %d\n", n, n);
    pthread_mutex_unlock(&chopstick[n]);
    return NULL;
}

int main(int argc, const char *argv[]) {
    for(int i = 0; i < 5; i += 1)
        pthread_mutex_init(&chopstick[i], NULL);

    for(int i =0; i < 5; i += 1)
        pthread_create(&philosopher[i], NULL, eat, (void *)(size_t)i);

    for(int i=0; i < 5; i += 1)
        pthread_join(philosopher[i], NULL);

    for(int i=0; i < 5; i += 1)
        pthread_mutex_destroy(&chopstick[i]);

    return 0;
}
```

# Your Task

For this PA, create and submit **two** of the following single-file programs.
Both should be based on the above code, but with two changes:

1. If there are arguments to `main`, covert the first to an integer and use it instead of 5 above. You should allocate the `philosopher` and `chopstick` arrays on the heap using `malloc`, `calloc`, or the like.

2. Implements the deadlock-avoidance algorithm described for that file below.

Neither should depend on any external files and work if compiled as `clang -O2 -lpthread dp_`*kind*`.c`.

## `dp_arbitrator.c`

This implementation should ensure there is no deadlock
by using the [Arbitrator solution](https://en.wikipedia.org/wiki/Dining_philosophers_problem#Arbitrator_solution).
A correct solution can be just four lines:

1. declare a global arbitrator mutex
2. initialize that mutex in `main`
3. lock it and
4. unlock it, both in `eat`, so that only one philosopher can reach for chopsticks at a time

If correctly implemented,

- There will be no deadlock; the program will always terminate
- The output will never have two "Philosopher *i* got chopstick" lines with a different "Philosopher *j* got chopstick" line in between

## `dp_hierarchy.c`

This implementation should ensure there is no deadlock
by using the [Resource hierarch solution](https://en.wikipedia.org/wiki/Dining_philosophers_problem#Resource_hierarchy_solution).

Require every philosopher to pick up their lower-numbered chopstick before their higher-numbered chopstick.

If correctly implemented,

- There will be no deadlock; the program will always terminate
- The output will never have two "Philosopher *i* got chopstick *j*" line following a different "Philosopher *i* got chopstick *k*" line unless *j* < *k*.


## `dp_message.c`

This implementation should ensure there is no deadlock
by using the [Chandy/Misra solution](https://en.wikipedia.org/wiki/Dining_philosophers_problem#Chandy/Misra_solution).

You'll probably need to add some kind of data structure to keep track of who owns which chopstick and which chopsticks are dirty,
as well as functions to handle requesting chopsticks.

This is definitely more involved than the other two files,
but also will give you a better understanding of how to use pthreads' mutex objects to solve non-trivial problems.

If correctly implemented,

- There will be no deadlock; the program will always terminate
- Philosopher 0 will always eat without waiting, starting with both chopstick 0 and chopstick *n* − 1 (where *n* is the number of philosophers).

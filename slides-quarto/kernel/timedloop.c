#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#define NUM_TIMINGS 10000000

long get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000L + ts.tv_nsec;
}

void output_timings(long *timings) {
    for (int i = 0; i < NUM_TIMINGS; ++i) {
        printf("%ld\n", timings[i]);
    }
}
#define N NUM_TIMINGS
long times[NUM_TIMINGS];
int main(void) {
    for (int i = 0; i < N; ++i) {
        long start, end;
        start = get_time();
        /* do nothing */
        end = get_time();
        times[i] = end - start;
    }
    output_timings(times);
}

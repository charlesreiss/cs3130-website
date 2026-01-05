#define _XOPEN_SOURCE 700

#include "pool.h"

#include <assert.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_TEST_TASKS 24


static const char *task_names[] = {
    "task00", "task01", "task02", "task03", "task04", "task05", "task06", "task07",
    "task08", "task09", "task10", "task11", "task12", "task13", "task14", "task15",
    "task16", "task17", "task18", "task19", "task20", "task21", "task22", "task23",
};

static pthread_mutex_t test_lock = PTHREAD_MUTEX_INITIALIZER;
static int test_order[MAX_TEST_TASKS];
static int test_order_index;
static int test_next_submit_index;
static int test_next_wait_index;
static struct TestScenario *test_scenario;
static pthread_barrier_t test_barrier;

struct TestScenario {
    const char *description;
    int thread_count;
    int submit_count;
    int barrier_count;
    bool need_wait_after_submit[MAX_TEST_TASKS];
    bool need_barrier_after_submit[MAX_TEST_TASKS];
    bool need_submit_extra[MAX_TEST_TASKS];
    bool need_barrier[MAX_TEST_TASKS];
    bool need_wait[MAX_TEST_TASKS];
    int wait_for_index[MAX_TEST_TASKS];
    int min_test_order[MAX_TEST_TASKS];
    int max_test_order[MAX_TEST_TASKS];
};

static void fail_test(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    fprintf(stderr, "FAILED TEST %s: ", test_scenario->description);
    vfprintf(stderr, format, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    abort();
}


// FIXME: check for thread reuse (pthread_current)
// FIXME: count threads in process
// FIXME: sleep after intermeidate wait to catch premature exit
struct TestScenario scenarios[] = {
    {
        .description = "one thread, one task",
        .thread_count = 1,
        .submit_count = 1,
        .min_test_order = {0},
        .max_test_order = {0},
    },
    {
        .description = "three threads, one task",
        .thread_count = 3,
        .submit_count = 1,
        .min_test_order = {0},
        .max_test_order = {0},
    },
    {
        .description = "one thread, one task, wait, one task",
        .thread_count = 1,
        .submit_count = 2,
        .need_wait_after_submit = {true, false},
        .min_test_order = {0, 1},
        .max_test_order = {0, 1},
    },
    {
        .description = "one thread, two task, wait, two task",
        .thread_count = 1,
        .submit_count = 4,
        .need_wait_after_submit = {false, true, false, false},
        .min_test_order = {0, 1, 2, 3},
        .max_test_order = {0, 1, 2, 3},
    },
    {
        .description = "two thread, two task, wait, two task",
        .thread_count = 2,
        .submit_count = 4,
        .need_wait_after_submit = {false, true, false, false},
        .min_test_order = {0, 0, 2, 2},
        .max_test_order = {1, 1, 3, 3},
    },
    {
        .description = "two thread, four task, first waits for fourth",
        .thread_count = 2,
        .submit_count = 4,
        .need_wait = {true, false, false, false},
        // barrier needed to ensure that fourth  task is submitted before first runs wait
        .barrier_count = 2,
        .need_barrier = {true, false, false, false},
        .need_barrier_after_submit = {false, false, false, true},
        .wait_for_index[0] = 3,
        // labeling these tasks A B C D
            // A must be after D (from wait)
            // D must be after B and C (because only one other worker thread available
        .min_test_order = {3, 0, 1, 2},
        .max_test_order = {3, 0, 1, 2},
    },
    {
        .description = "one thread, five tasks",
        .thread_count = 1,
        .submit_count = 5,
        .min_test_order = {0, 1, 2, 3, 4},
        .max_test_order = {0, 1, 2, 3, 4},
    },
    {
        .description = "one thread, five tasks, wait on barrier with 1st task before submitting last two",
        .thread_count = 1,
        .submit_count = 5,
        .barrier_count = 2,
        .need_barrier = {true, false, false, false, false},
        .need_barrier_after_submit = {false, false, true, false, false},
        .min_test_order = {0, 1, 2, 3, 4},
        .max_test_order = {0, 1, 2, 3, 4},
    },
    {
        .description = "two threads, five tasks",
        .thread_count = 2,
        .submit_count = 5,
        .min_test_order = {0, 0, 0, 0, 0},
        .max_test_order = {4, 4, 4, 4, 4},
    },
    {
        .description = "two thread, six tasks, wait on barrier after 3rd before submitting rest",
        .thread_count = 2,
        .submit_count = 6,
        .barrier_count = 3,
        .need_barrier = {true, false, true, false, false, false},
        .need_barrier_after_submit = {false, false, false, true, false, false},
        .min_test_order = {1, 0, 1, 3, 3, 3},
        .max_test_order = {2, 0, 2, 5, 5, 5},
    },
    {
        .description = "two threads, five tasks, 1/2/4/5 waiting on barrier",
        .thread_count = 2,
        .submit_count = 5,
        .barrier_count = 2,
        .need_barrier = {false, true, false, true, false},
        .min_test_order = {0, 0, 2, 3, 3},
        .max_test_order = {1, 1, 2, 4, 4},
    },
    {
        .description = "two threads, six tasks, 2nd/4th/5th/6th waiting on barrier",
        .thread_count = 2,
        .submit_count = 6,
        .barrier_count = 2,
        .need_barrier = {false, true, false, true, true, true},
        .min_test_order = {0, 2, 1, 2, 4, 4},
        .max_test_order = {0, 3, 1, 3, 5, 5},
    },
    {
        .description = "one thread, one task submitting extra task",
        .thread_count = 1,
        .submit_count = 1,
        .barrier_count = 0,
        .need_submit_extra = {true, false},
        .min_test_order = {0, 1},
        .max_test_order = {0, 1},
    },
    {
        .description = "two thread, four tasks, one submitting extra task, tasks 0/1, 4/5 using barriers",
        .thread_count = 2,
        .submit_count = 4,
        .barrier_count = 2,
        .need_barrier = {true, true, false, true, true},
        .need_submit_extra = {true, false},
        .min_test_order = {0, 0, 0, 3, 3},
        .max_test_order = {2, 2, 2, 4, 4},
    },
    // FIXME: wait from task test
};



static int test_submit_next(void);
static bool test_wait_next(void);
static void test_wait_for(int index);

static void *test_task_function(void *argument) {
    int index = (int) argument;
    if (test_scenario->need_barrier[index]) {
        pthread_barrier_wait(&test_barrier);
    }
    if (test_scenario->need_submit_extra[index]) {
        test_submit_next();
    }
    if (test_scenario->need_wait[index]) {
        test_wait_for(test_scenario->wait_for_index[index]);
    }
    // FIXME: deadlock detection
    pthread_mutex_lock(&test_lock);
    if (test_order_index >= MAX_TEST_TASKS)
        fail_test("task functions run too many time (running %d)", index);
    test_order[index] = test_order_index;
    test_order_index += 1;
    pthread_mutex_unlock(&test_lock);
    return (void*) index+1000;
}

static int test_submit_next(void) {
    pthread_mutex_lock(&test_lock);
    int index = test_next_submit_index;
    test_next_submit_index += 1;
    pthread_mutex_unlock(&test_lock);
    assert(test_next_submit_index < MAX_TEST_TASKS);
    pool_submit_task(task_names[index], test_task_function, (void*) index);
    return index;
}

static void test_wait_for(int index) {
    int result = (int) pool_wait_for_task(task_names[index]);
    if (result != index + 1000) {
        fail_test("wrong return value from task %d", index);
    }
}

static bool test_wait_next(void) {
    pthread_mutex_lock(&test_lock);
    if (test_next_wait_index < test_next_submit_index) {
        bool advanced;
        do {
            advanced = false;
            for (int i = 0; i < MAX_TEST_TASKS; i += 1) {
                if (test_scenario->need_wait[i] &&
                    test_scenario->wait_for_index[i] == test_next_wait_index) {
                    test_next_wait_index += 1;
                    advanced = true;
                    break;
                }
            }
        } while (advanced);
    }
    if (test_next_wait_index < test_next_submit_index) {
        int index = test_next_wait_index;
        test_next_wait_index += 1;
        pthread_mutex_unlock(&test_lock);
        int result = (int) pool_wait_for_task(task_names[index]);
        if (result != index + 1000) {
            fail_test("wrong return value from task %d", index);
        }
        pthread_mutex_lock(&test_lock);
    }
    bool result = test_next_wait_index != test_next_submit_index;
    pthread_mutex_unlock(&test_lock);
    return result;
}

static void test_setup() {
    for (int i = 0; i < MAX_TEST_TASKS; i += 1) {
        test_order[i] = -1;
    }
    test_order_index = 0;
    test_next_submit_index = 0;
    test_next_wait_index = 0;
    if (test_scenario->barrier_count) {
        pthread_barrier_init(&test_barrier, NULL, test_scenario->barrier_count);
    } else {
        pthread_barrier_init(&test_barrier, NULL, MAX_TEST_TASKS * 2);
    }
}

static void test_cleanup() {
    pthread_barrier_destroy(&test_barrier);
}

static void run_current_test() {
    fprintf(stderr, "running test %s\n", test_scenario->description);
    test_setup();
    pool_setup(test_scenario->thread_count);
    for (int i = 0; i < test_scenario->submit_count; i += 1) {
        int index = test_submit_next();
        if (test_scenario->need_wait_after_submit[index]) {
            while (test_wait_next()) {}
        }
        if (test_scenario->need_barrier_after_submit[index]) {
            pthread_barrier_wait(&test_barrier);
        }
    }
    pool_stop();
    while (test_wait_next());
    // check number of submitted tasks
    int expect_submitted = test_scenario->submit_count;
    for (int i = 0; i < test_next_submit_index; i += 1) {
        if (test_order[i] == -1) {
            fail_test("task with index %d (0-based) not run", i);
        } else if (test_order[i] < test_scenario->min_test_order[i] ||
                   test_order[i] > test_scenario->max_test_order[i]) {
            fail_test("task with index %d (0-based) run with "
                      "index %d (expected between %d and %d inclusive)",
                      i, test_order[i],
                      test_scenario->min_test_order[i],
                      test_scenario->max_test_order[i]);
        }
    }
    for (int i = 0; i < MAX_TEST_TASKS; i += 1) {
        if (test_scenario->need_submit_extra[i]) {
            expect_submitted += 1;
        }
    }
    if (test_next_submit_index != expect_submitted) {
        fail_test("expected to submit %d tasks, but only submitted %d\n",
            expect_submitted, test_next_submit_index);
    }
    test_cleanup();
}

int main() {
    for (int i = 0; i < sizeof(scenarios)/sizeof(scenarios[0]); i += 1) {
        test_scenario = &scenarios[i];
        run_current_test();
    }
}

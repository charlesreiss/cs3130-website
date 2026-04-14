#define _XOPEN_SOURCE 700

#include "pool.h"

#include <assert.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_TEST_TASKS 24
#define PAUSE_NSEC (1000L * 1000L * 500L) /* 500 ms */

static pthread_mutex_t test_lock = PTHREAD_MUTEX_INITIALIZER;
static int task_ids[MAX_TEST_TASKS];
static int test_order[MAX_TEST_TASKS];
static int test_order_index;
static int test_next_submit_index;
static struct TestScenario *test_scenario;
static pthread_barrier_t test_barrier;

struct TestScenario {
    const char *description;
    int thread_count;
    int submit_count;
    int barrier_count;
    bool need_wait_after_submit[MAX_TEST_TASKS];
    bool need_barrier_after_submit[MAX_TEST_TASKS];
    int need_submit_extra_index[MAX_TEST_TASKS];
    bool need_barrier[MAX_TEST_TASKS];
    bool need_task_pause[MAX_TEST_TASKS];
    bool need_pause_after_submit[MAX_TEST_TASKS];
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
        .description = "one thread, one task (that pauses), wait",
        .thread_count = 1,
        .submit_count = 1,
        .need_task_pause = {true},
        .need_wait_after_submit = {true},
        .min_test_order = {0},
        .max_test_order = {0},
    },
    {
        .description = "one thread, four tasks, pausing between task submissions",
        .thread_count = 1,
        .submit_count = 4,
        .need_pause_after_submit = {true, true, true, true},
        .min_test_order = {0, 1, 2, 3},
        .max_test_order = {0, 1, 2, 3},
    },
    {
        .description = "three threads, one task, wait",
        .thread_count = 3,
        .submit_count = 1,
        .need_wait_after_submit = {true},
        .min_test_order = {0},
        .max_test_order = {0},
    },
    {
        .description = "three threads, one task, wait, two tasks",
        .thread_count = 3,
        .submit_count = 3,
        .need_wait_after_submit = {true},
        .min_test_order = {0, 1, 1},
        .max_test_order = {0, 2, 2},
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
        .description = "two threads, one task, wait, one task",
        .thread_count = 2,
        .submit_count = 2,
        .need_wait_after_submit = {true, false},
        .min_test_order = {0, 1},
        .max_test_order = {0, 1},
    },
    {
        .description = "one thread, one task (that pauses), wait, one task (that pauses), wait",
        .thread_count = 1,
        .submit_count = 2,
        .need_task_pause = {true, true},
        .need_wait_after_submit = {true, true},
        .min_test_order = {0, 1},
        .max_test_order = {0, 1},
    },
    {
        .description = "one thread, two task, wait, two task, wait",
        .thread_count = 1,
        .submit_count = 4,
        .need_wait_after_submit = {false, true, false, true},
        .min_test_order = {0, 1, 2, 3},
        .max_test_order = {0, 1, 2, 3},
    },
    {
        .description = "one thread, two task (that pause), wait, two task (that pause), wait",
        .thread_count = 1,
        .submit_count = 4,
        .need_task_pause = {true, true, true, true},
        .need_wait_after_submit = {false, true, false, true},
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
        .description = "one thread, five tasks (that pause), wait",
        .thread_count = 1,
        .submit_count = 5,
        .need_task_pause = {true, true, true, true, true},
        .need_wait_after_submit = {false, false, false, false, true},
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
        .description = "two threads, five tasks (that pause), wait",
        .thread_count = 2,
        .submit_count = 5,
        .need_task_pause = {true, true, true, true, true},
        .need_wait_after_submit = {false, false, false, false, true},
        .min_test_order = {0, 0, 0, 0, 0},
        .max_test_order = {4, 4, 4, 4, 4},
    },
    {
        .description = "two threads, five tasks (pausing between submissions), wait",
        .thread_count = 2,
        .submit_count = 5,
        .need_pause_after_submit = {true, true, true, true, false},
        .need_wait_after_submit = {false, false, false, false, true},
        .min_test_order = {0, 0, 0, 0, 0},
        .max_test_order = {4, 4, 4, 4, 4},
    },
    {
        .description = "two thread, five tasks, wait on barrier with 1st task before submitting last two",
        .thread_count = 2,
        .submit_count = 5,
        .barrier_count = 2,
        .need_barrier = {true, false, false, false, false},
        .need_barrier_after_submit = {false, false, true, false, false},
        .min_test_order = {0, 0, 0, 1, 1},
        .max_test_order = {2, 4, 4, 4, 4},
    },
    {
        .description = "two thread, six tasks, submit 3, wait on barrier with 1st+3rd before submitting last 3",
        .thread_count = 2,
        .submit_count = 6,
        .barrier_count = 3,
        .need_barrier = {true, false, true, false, false, false},
        .need_barrier_after_submit = {false, false, true, false, false, false},
        .min_test_order = {0, 0, 0, 2, 2, 2},
        .max_test_order = {2, 5, 2, 5, 5, 5},
    },
    {
        .description = "two threads, four tasks, all waiting on barrier",
        .thread_count = 2,
        .submit_count = 4,
        .barrier_count = 2,
        .need_barrier = {true, true, true, true},
        .min_test_order = {0, 0, 2, 2},
        .max_test_order = {1, 1, 3, 3},
    },
    {
        .description = "two threads, five tasks, 1/2/4/5 waiting on barrier, pausing between submissions",
        .thread_count = 2,
        .submit_count = 5,
        .barrier_count = 2,
        .need_pause_after_submit = {true, true, true, true, false},
        .need_barrier = {true, true, false, true, true},
        .min_test_order = {0, 0, 2, 3, 3},
        .max_test_order = {1, 1, 2, 4, 4},
    },
    {
        .description = "two threads, five tasks, 1/2/4/5 waiting on barrier",
        .thread_count = 2,
        .submit_count = 5,
        .barrier_count = 2,
        .need_barrier = {true, true, false, true, true},
        .min_test_order = {0, 0, 2, 2, 2},
        .max_test_order = {1, 1, 4, 4, 4},
    },
    {
        .description = "two threads, six tasks, 2nd/4th/5th/6th waiting on barrier",
        .thread_count = 2,
        .submit_count = 6,
        .barrier_count = 2,
        .need_barrier = {false, true, false, true, true, true},
        .min_test_order = {0, 0, 1, 2, 4, 4},
        .max_test_order = {1, 3, 2, 3, 5, 5},
    },
    {
        .description = "one thread, one task submitting extra task",
        .thread_count = 1,
        .submit_count = 1,
        .barrier_count = 0,
        .need_submit_extra_index = {1, 0},
        .min_test_order = {0, 1},
        .max_test_order = {0, 1},
    },
    {
        .description = "two thread, four tasks, one submitting extra task, tasks 0/1, 4/5 using barriers",
        .thread_count = 2,
        .submit_count = 4,
        .barrier_count = 2,
        .need_barrier = {true, true, false, true, true},
        .need_submit_extra_index = {0, 4, 0, 0, 0},
        .min_test_order = {0, 0, 0, 2, 2},
        .max_test_order = {2, 2, 4, 4, 4},
    },
};



static int test_submit_index(int index);

static const char *th_string(int index) {
    if (index == 11 || index == 12 || index == 13) {
        return "th";
    } else {
        switch (index % 10) {
        case 1:
            return "st";
        case 2:
            return "nd";
        case 3:
            return "rd";
        default:
            return "th";
        }
    }
}

static void test_pause() {
    static const struct timespec duration = {
        .tv_sec = 0,
        .tv_nsec = PAUSE_NSEC,
    };
    nanosleep(&duration, NULL);
}

static void *test_task_function(void *argument) {
    unsigned long arg_as_ulong = (long) argument;
    int index = arg_as_ulong & 0xFFFF;
    if ((arg_as_ulong & 0xFFFF0000ul) != 0xBEEF0000) {
        fail_test("argument value passed incorrectly to task");
    }
    if (test_scenario->need_task_pause[index]) {
        test_pause();
    }
    if (test_scenario->need_submit_extra_index[index] > 0) {
        test_submit_index(test_scenario->need_submit_extra_index[index]);
    }
    pthread_mutex_lock(&test_lock);
    if (test_order_index >= MAX_TEST_TASKS)
        fail_test("task functions run too many time (running %d)", index);
    if (test_order[index] != -1)
        fail_test("task with submit index %d (id %d) run too many times", index, task_ids[index]);
    test_order[index] = test_order_index;
    test_order_index += 1;
    pthread_mutex_unlock(&test_lock);
    if (test_scenario->need_barrier[index]) {
        pthread_barrier_wait(&test_barrier);
    }
    return (void*) (long) (index + 1000);
}

static int test_submit_index(int index) {
    unsigned long argument = 0xBEEF0000ul | index;
    task_ids[index] = pool_submit_task(test_task_function, (void*) argument);
    return index;
}

static void test_check_return(int index) {
    void *result = pool_get_task_result(task_ids[index]);
    if (result == NULL) {
        fail_test("task with submit index %d (id %d) has NULL return (not completed?)", index, task_ids[index]);
    }
    if ((long) result != index + 1000) {
        fail_test("wrong return value from task with submit index %d (id %d)", index, task_ids[index]);
    }
}

static void test_setup() {
    for (int i = 0; i < MAX_TEST_TASKS; i += 1) {
        test_order[i] = -1;
    }
    test_order_index = 0;
    if (test_scenario->barrier_count) {
        pthread_barrier_init(&test_barrier, NULL, test_scenario->barrier_count);
    } else {
        pthread_barrier_init(&test_barrier, NULL, MAX_TEST_TASKS * 2);
    }
}

static void test_cleanup() {
    pthread_barrier_destroy(&test_barrier);
}

static void test_wait() {
    pthread_mutex_lock(&test_lock);
    // FIXME: assuming only already submitted tasks waited for
    int max_submit_index = test_next_submit_index;
    pthread_mutex_unlock(&test_lock);
    pool_wait();
    for (int i = 0; i < max_submit_index; i += 1) {
        test_check_return(i);
    }
}

static void run_current_test() {
    fprintf(stderr, "running test %s\n", test_scenario->description);
    test_setup();
    pool_setup(test_scenario->thread_count);
    for (int index = 0; index < test_scenario->submit_count; index += 1) {
        test_submit_index(index);
        if (test_scenario->need_wait_after_submit[index]) {
            test_wait();
        }
        if (test_scenario->need_pause_after_submit[index]) {
            test_pause();
        }
        if (test_scenario->need_barrier_after_submit[index]) {
            pthread_barrier_wait(&test_barrier);
        }
    }
    pool_stop();
    // check number of submitted tasks
    int expect_submitted = test_scenario->submit_count;
    for (int i = 0; i < MAX_TEST_TASKS; i += 1) {
        if (test_scenario->need_submit_extra_index[i] > 0) {
            expect_submitted += 1;
        }
    }
    for (int i = 0; i < expect_submitted; i += 1) {
        test_check_return(i);
        if (test_order[i] == -1) {
            fail_test("task with index %d (id %d) not run", i, task_ids[i]);
        } else if (test_order[i] < test_scenario->min_test_order[i] ||
                   test_order[i] > test_scenario->max_test_order[i]) {
            fail_test("task with index %d (id %d) was "
                      "run %d%s (expected between %d%s and %d%s inclusive)",
                      i, task_ids[i],
                      test_order[i] + 1, th_string(test_order[i] + 1),
                      test_scenario->min_test_order[i] + 1, th_string(test_scenario->min_test_order[i] + 1),
                      test_scenario->max_test_order[i] + 1, th_string(test_scenario->max_test_order[i] + 1));
        }
    }
    for (int i = expect_submitted; i < MAX_TEST_TASKS; i += 1) {
        if (test_order[i] != -1) {
            fail_test("task unexpected run with index %d", i);
        }
    }
    test_cleanup();
}

int main() {
    for (int i = 0; i < sizeof(scenarios)/sizeof(scenarios[0]); i += 1) {
        test_scenario = &scenarios[i];
        run_current_test();
    }
}

#include <stdio.h>
#include "pool.h"

#define THREADS 2
#define TASKS_BEFORE_WAIT 3
#define TASKS_AFTER_WAIT 3

static void *task_function(void *argument) {
    printf("running task with argument=%p\n", argument);
    return argument;
}

static void *int_to_pointer(int x) {
    return (void *) (long) x;
}

int main() {
    int task_ids[TASKS_BEFORE_WAIT + TASKS_AFTER_WAIT];
    pool_setup(THREADS);
    for (int i = 0; i < TASKS_BEFORE_WAIT; i += 1) {
        task_ids[i] = pool_submit_task(task_function, int_to_pointer(i * 10));
        printf("pool_submit_task returned id %d\n", task_ids[i]);
    }
    printf("waiting for tasks\n");
    pool_wait();
    for (int i = 0; i < TASKS_BEFORE_WAIT; i += 1) {
        printf("result for task id %d is %p (expected %p)\n", 
            task_ids[i], pool_get_task_result(task_ids[i]), int_to_pointer(i * 10));
    }
    for (int i = TASKS_BEFORE_WAIT; i < TASKS_BEFORE_WAIT + TASKS_AFTER_WAIT; i += 1) {
        task_ids[i] = pool_submit_task(task_function, int_to_pointer(i * 10));
        printf("pool_submit_task returned id %d\n", task_ids[i]);
    }
    printf("stopping thread pool\n");
    pool_stop();
    for (int i = TASKS_BEFORE_WAIT; i < TASKS_BEFORE_WAIT + TASKS_AFTER_WAIT; i += 1) {
        printf("result for task id %d is %p (expected %p)\n", 
            task_ids[i], pool_get_task_result(task_ids[i]), int_to_pointer(i * 10));
    }
}

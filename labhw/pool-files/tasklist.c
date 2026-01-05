#include <stdbool.h>
#include <string.h>

struct Task {
    char *name;
    /* you can add fields here */

    struct Task *next;
    struct Task *prev;
};

struct TaskList {
    struct Task *head;
    struct Task *tail;
};

static bool list_is_empty(struct TaskList *list) {
    return list->head == NULL;
}

static void list_remove(struct TaskList *list, struct Task *node) {
    if (node->prev) {
        node->prev->next = node->next;
    }
    if (node->next) {
        node->next->prev = node->prev;
    }
    if (node == list->tail) {
        list->tail = node->prev;
    }
    if (node == list->head) {
        list->head = node->next;
    }
}

static struct Task *list_remove_tail(struct TaskList *list) {
    if (list->tail == NULL) {
        return NULL;
    } else {
        struct Task *result = list->tail;
        list_remove(list, result);
        return result;
    }
}

static void list_insert_head(struct TaskList *list, struct Task *node) {
    node->next = list->head;
    node->prev = NULL;
    if (list->head) {
        list->head->prev = node;
    } else {
        list->tail = node;
    }
    list->head = node;
}

static struct Task *list_find_named(struct TaskList *list, const char *name) {
    struct Task *node = list->head;
    while (node) {
        if (strcmp(node->name, name) == 0)
            return node;
        node = node->next;
    }
    return NULL;
}

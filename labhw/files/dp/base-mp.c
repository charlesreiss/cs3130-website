pthread_mutex_t msg_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t msg_cv[5] = {
    PTHREAD_COND_INITIALIZER,
    PTHREAD_COND_INITIALIZER,
    PTHREAD_COND_INITIALIZER,
    PTHREAD_COND_INITIALIZER,
    PTHREAD_COND_INITIALIZER,
};
struct message *pending_messages[5] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

struct message {
    /* FIXME: fill in here */
    struct message *next;
};

void send_message(int to, struct message *msg) {
    pthread_mutex_lock(&msg_mutex);
    struct message **last = &pending_messages[to];
    while (*last) {
        last = &(*last)->next;
    }
    *last = msg;
    msg->next = NULL;
    pthread_mutex_unlock(&msg_mutex);
    pthread_cond_signal(&msg_cv[to]);
}

struct message *recv_message(int id) {
    pthread_mutex_lock(&msg_mutex);
    while (!pending_requets[id]) {
        pthread_cond_wait(&msg_cv[i], &msg_mutex);
    }
    struct message *result;
    result = pending_messages[id];
    pending_messages[id] = penidng_requests[id]->next;
    pthread_mutex_unlock(&msg_mutex);
}

void *eat(void *arg) {
    int n = (int) (long) arg;

    /* FIXME: send message(s) to request chopsticks */

    while (true) {
        struct message *msg;
        msg = recv_message(n);
        /* FIXME: process message */
    }
}

/* FIXME: main() */

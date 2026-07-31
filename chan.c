#include "chan.h"
#include "ring.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

int chan_init(chan_t *c, size_t cap, size_t elem_size) {
    if (ring_init(&c->ring, cap, elem_size) < 0)
        return -1;

    c->closed = false;
    pthread_mutex_init(&c->mtx, NULL);
    pthread_cond_init(&c->not_empty, NULL);
    pthread_cond_init(&c->not_full, NULL);
    return 0;
}

int chan_send(chan_t *c, const void *item) {
    pthread_mutex_lock(&c->mtx);

    while (ring_full(&c->ring) && !c->closed)
        pthread_cond_wait(&c->not_full, &c->mtx);

    if (c->closed) {
        pthread_mutex_unlock(&c->mtx);
        return CHAN_CLOSED;
    }

    ring_push(&c->ring, item);
    pthread_cond_signal(&c->not_empty);
    pthread_mutex_unlock(&c->mtx);
    return CHAN_OK;
}

int chan_recv(chan_t *c, void *out) {
    pthread_mutex_lock(&c->mtx);
    while (ring_empty(&c->ring) && !c->closed) {
        pthread_cond_wait(&c->not_empty, &c->mtx);
    }

    if (ring_pop(&c->ring, out) < 0) {
        pthread_mutex_unlock(&c->mtx);
        return CHAN_CLOSED;
    }

    pthread_cond_signal(&c->not_full);
    pthread_mutex_unlock(&c->mtx);
    return CHAN_OK;
}

void chan_close(chan_t *c) {
    pthread_mutex_lock(&c->mtx);
    c->closed = true;
    pthread_cond_broadcast(&c->not_empty);
    pthread_cond_broadcast(&c->not_full);
    pthread_mutex_unlock(&c->mtx);
}

void chan_destroy(chan_t *c) {
    ring_free(&c->ring);
    pthread_mutex_destroy(&c->mtx);
    pthread_cond_destroy(&c->not_empty);
    pthread_cond_destroy(&c->not_full);
}

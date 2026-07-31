#ifndef CHAN_H
#define CHAN_H

#include <pthread.h>
#include <stdbool.h>

#include "ring.h"

#define CHAN_OK 0
#define CHAN_CLOSED -1

typedef struct {
    ring_t ring;
    bool closed;
    pthread_mutex_t mtx;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} chan_t;

// cap must be a power of two. Returns 0, or -1 on failure.
int chan_init(chan_t *c, size_t cap, size_t elem_size);

// Sends an item, blocking while full. Returns CHAN_CLOSED if closed.
int chan_send(chan_t *c, const void *item);

// Receives the oldest item, blocking while empty. Buffered items are
// still delivered after close; returns CHAN_CLOSED once drained.
int chan_recv(chan_t *c, void *out);

// Marks the channel closed and wakes every blocked sender and
// receiver. Buffered items remain receivable. Idempotent.
void chan_close(chan_t *c);

// Frees the channel. Call chan_close first and join all threads:
// destroying with a thread still blocked inside is undefined behavior.
void chan_destroy(chan_t *c);

#endif

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "chan.h"
#include <pthread.h>

typedef struct {
    void (*fn)(void *arg);
    void *arg;
} task_t;

typedef struct {
    chan_t task_queue;
    pthread_t *threads;
    size_t nthreads;
} pool_t;

// nthreads workers, queue_cap pending tasks. queue_cap must be a power
// of two. Returns 0, or -1 on failure.
int pool_init(pool_t *p, size_t nthreads, size_t queue_cap);

// Blocks while the queue is full. Returns -1 if the pool is shut down.
int pool_submit(pool_t *p, void (*fn)(void *), void *arg);

// Stops accepting tasks, runs the queued ones, joins every worker.
void pool_shutdown(pool_t *p);

// Frees the pool. Call pool_shutdown first.
void pool_destroy(pool_t *p);

#endif

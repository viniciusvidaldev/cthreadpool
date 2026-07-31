#include "thread_pool.h"
#include "chan.h"
#include <stdlib.h>

static void *worker(void *arg) {
    chan_t *chan = arg;

    for (;;) {
        task_t got;
        if (chan_recv(chan, &got) != CHAN_OK) {
            break;
        }
        got.fn(got.arg);
    }

    return NULL;
}

int pool_init(pool_t *p, size_t nthreads, size_t queue_cap) {
    if (nthreads == 0) {
        return -1;
    }

    if (chan_init(&p->task_queue, queue_cap, sizeof(task_t)) < 0) {
        return -1;
    }

    p->threads = malloc(nthreads * sizeof(pthread_t));
    if (p->threads == NULL) {
        chan_destroy(&p->task_queue);
        return -1;
    }
    p->nthreads = nthreads;

    for (size_t i = 0; i < nthreads; i++) {
        if (pthread_create(&p->threads[i], NULL, worker, &p->task_queue) != 0) {
            chan_close(&p->task_queue);
            for (size_t j = 0; j < i; j++)
                pthread_join(p->threads[j], NULL);
            free(p->threads);
            chan_destroy(&p->task_queue);
            return -1;
        }
    }

    return 0;
}

int pool_submit(pool_t *p, void (*fn)(void *), void *arg) {
    if (fn == NULL)
        return -1;

    task_t task = {.fn = fn, .arg = arg};
    return chan_send(&p->task_queue, &task) == CHAN_OK ? 0 : -1;
}

void pool_shutdown(pool_t *p) {
    chan_close(&p->task_queue);
    for (size_t i = 0; i < p->nthreads; i++)
        pthread_join(p->threads[i], NULL);
}

void pool_destroy(pool_t *p) {
    free(p->threads);
    p->threads = NULL;
    chan_destroy(&p->task_queue);
}

#include "thread_pool.h"
#include <assert.h>
#include <stdatomic.h>
#include <stdio.h>

#define NTASKS 100000
static atomic_int counter;

void increment(void *arg) {
    (void)arg;
    atomic_fetch_add(&counter, 1);
}

int main(void) {
    pool_t pool;

    if (pool_init(&pool, 4, 8) < 0)
        return 1;

    for (int i = 0; i < NTASKS; i++)
        pool_submit(&pool, increment, NULL);

    pool_shutdown(&pool);

    int total = atomic_load(&counter);
    int rejected = pool_submit(&pool, increment, NULL) < 0;
    pool_destroy(&pool);

    printf("ran %d/%d, post-shutdown submit rejected: %d\n", total, NTASKS, rejected);

    assert(total == NTASKS && rejected);
    return 0;
}

#ifndef RING_H
#define RING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct {
    uint8_t *buf;
    size_t len;
    size_t head;
    size_t elem_size;
    size_t cap;  // power of two
    size_t mask; // cap - 1, computed only once
} ring_t;

// cap must be a power of two. Returns 0, or -1 on failure.
int ring_init(ring_t *r, size_t cap, size_t elem_size);
void ring_free(ring_t *r);

static inline bool ring_empty(ring_t *r) { return r->len == 0; }
static inline bool ring_full(ring_t *r) { return r->len == r->cap; }

// Pushes an element to the back of the buffer.
// Returns -1 if full, 0 on success.
int ring_push(ring_t *r, const void *item);

// Pops the oldest element into out.
// Returns -1 if empty, 0 on success.
int ring_pop(ring_t *r, void *out);

#endif

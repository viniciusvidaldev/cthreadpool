#include "ring.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static bool is_pow2(size_t n) { return n && (n & (n - 1)) == 0; }

int ring_init(ring_t *r, size_t cap, size_t elem_size) {
    if (elem_size == 0 || cap > SIZE_MAX / elem_size)
        return -1;
    if (!is_pow2(cap))
        return -1;
    r->buf = malloc(cap * elem_size);
    if (r->buf == NULL)
        return -1;
    r->elem_size = elem_size;
    r->cap = cap;
    r->head = 0;
    r->len = 0;
    r->mask = cap - 1;
    return 0;
}

void ring_free(ring_t *r) {
    free(r->buf);
    r->buf = NULL;
}

// Maps a sequence number to the address of its slot.
// Wraps via mask, so any seq is valid.
static inline void *slot(const ring_t *r, size_t seq) {
    return r->buf + (seq & r->mask) * r->elem_size;
};

int ring_push(ring_t *r, const void *item) {
    if (ring_full(r))
        return -1;

    void *s = slot(r, r->head + r->len);
    memcpy(s, item, r->elem_size);
    r->len += 1;
    return 0;
}

int ring_pop(ring_t *r, void *out) {
    if (ring_empty(r))
        return -1;

    void *s = slot(r, r->head);
    memcpy(out, s, r->elem_size);
    r->head += 1;
    r->len -= 1;
    return 0;
}

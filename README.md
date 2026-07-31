# cthreadpool

A bounded blocking channel in C, plus a thread pool built on top of it.

## What's here

- `ring.[ch]` — fixed-capacity ring buffer, power-of-two size, generic over element size.
- `chan.[ch]` — blocking MPMC channel: mutex + two condvars around the ring. `close` wakes all waiters; buffered items still drain, then receivers get `CHAN_CLOSED`.
- `thread_pool.[ch]` — N worker threads pulling `task_t` off a channel. `shutdown` closes the channel and joins.

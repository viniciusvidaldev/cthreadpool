CFLAGS = -std=c11 -Wall -Wextra -g -pthread

main: ring.c chan.c thread_pool.c main.c
	cc $(CFLAGS) -o $@ $^

clear:
	rm -f main

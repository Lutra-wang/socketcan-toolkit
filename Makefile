CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -O2
all: can_send can_dump
can_send: src/can_send.c
	$(CC) $(CFLAGS) -o $@ $<
can_dump: src/can_dump.c
	$(CC) $(CFLAGS) -o $@ $<
clean:
	rm -f can_send can_dump
.PHONY: all clean
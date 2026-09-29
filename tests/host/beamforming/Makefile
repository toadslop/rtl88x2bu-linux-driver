ROOT := ../../..
HOST_INC := ../include
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)
L2_SRC := bf_entry_packet_l2.c

.PHONY: all test test-c clean
all: test

test: test-c

test-c: test_bf_entry_packet
	./test_bf_entry_packet

test_bf_entry_packet: $(L2_SRC)
	$(CC) $(CFLAGS) -o $@ $(L2_SRC)

clean:
	rm -f test_bf_entry_packet

ROOT := ../../..
HOST_INC := ../include
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)
ENTRY_SRC := bf_entry_packet_l2.c
PACKET_SRC := bf_packet_leaf_l2.c

.PHONY: all test test-c test-packet clean
all: test

test: test-c test-packet

test-c: test_bf_entry_packet
	./test_bf_entry_packet

test-packet: test_bf_packet_leaf
	./test_bf_packet_leaf

test_bf_entry_packet: $(ENTRY_SRC)
	$(CC) $(CFLAGS) -o $@ $(ENTRY_SRC)

test_bf_packet_leaf: $(PACKET_SRC)
	$(CC) $(CFLAGS) -o $@ $(PACKET_SRC)

clean:
	rm -f test_bf_entry_packet test_bf_packet_leaf

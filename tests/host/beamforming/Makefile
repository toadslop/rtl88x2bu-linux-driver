ROOT := ../../..
HOST_INC := ../include
RUST_BF := $(ROOT)/rust/rtw_beamforming.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)
ENTRY_SRC := bf_entry_packet_l2.c
PACKET_SRC := bf_packet_leaf_l2.c

.PHONY: all test test-c test-rust test-packet test-packet-rust clean
all: test test-rust test-packet-rust

test: test-c test-packet

test-rust: test_bf_entry_packet_rust
	./test_bf_entry_packet_rust

test-packet-rust: test_bf_packet_leaf_rust
	./test_bf_packet_leaf_rust

test-c: test_bf_entry_packet
	./test_bf_entry_packet

test-packet: test_bf_packet_leaf
	./test_bf_packet_leaf

test_bf_entry_packet: $(ENTRY_SRC)
	$(CC) $(CFLAGS) -o $@ $(ENTRY_SRC)

test_bf_entry_packet_rust: $(ENTRY_SRC) librust_bf_entry_packet.a
	$(CC) $(CFLAGS) -DHOST_BF_ENTRY_PACKET_RUST -o $@ $(ENTRY_SRC) librust_bf_entry_packet.a

test_bf_packet_leaf: $(PACKET_SRC)
	$(CC) $(CFLAGS) -o $@ $(PACKET_SRC)

test_bf_packet_leaf_rust: $(PACKET_SRC) librust_bf_entry_packet.a
	$(CC) $(CFLAGS) -DHOST_BF_ENTRY_PACKET_RUST -o $@ $(PACKET_SRC) librust_bf_entry_packet.a

librust_bf_entry_packet.a: $(RUST_BF)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_bf_entry_packet_test \
		--crate-type staticlib $(RUST_BF) -o $@

clean:
	rm -f test_bf_entry_packet test_bf_entry_packet_rust test_bf_packet_leaf \
		test_bf_packet_leaf_rust librust_bf_entry_packet.a

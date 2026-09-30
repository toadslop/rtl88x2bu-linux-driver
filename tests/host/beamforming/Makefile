ROOT := ../../..
HOST_INC := ../include
RUST_BF := $(ROOT)/rust/rtw_beamforming.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)
ENTRY_SRC := bf_entry_packet_l2.c
PACKET_SRC := bf_packet_leaf_l2.c
INIT_CMD_SRC := bf_init_cmd_leaf_l2.c
GID_SRC := bf_gid_leaf_l2.c

.PHONY: all test test-c test-rust test-packet test-packet-rust test-init-cmd test-init-cmd-rust test-gid clean
all: test test-rust test-packet-rust test-init-cmd test-init-cmd-rust test-gid

test: test-c test-packet test-init-cmd test-gid

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

test-init-cmd: test_bf_init_cmd_leaf
	./test_bf_init_cmd_leaf

test-init-cmd-rust: test_bf_init_cmd_leaf_rust
	./test_bf_init_cmd_leaf_rust

test_bf_init_cmd_leaf: $(INIT_CMD_SRC)
	$(CC) $(CFLAGS) -o $@ $(INIT_CMD_SRC)

test_bf_init_cmd_leaf_rust: $(INIT_CMD_SRC) librust_bf_init_cmd.a
	$(CC) $(CFLAGS) -DHOST_BF_INIT_CMD_RUST -o $@ $(INIT_CMD_SRC) librust_bf_init_cmd.a

test-gid: test_bf_gid_leaf
	./test_bf_gid_leaf

test_bf_gid_leaf: $(GID_SRC)
	$(CC) $(CFLAGS) -o $@ $(GID_SRC)

test_bf_packet_leaf_rust: $(PACKET_SRC) librust_bf_entry_packet.a
	$(CC) $(CFLAGS) -DHOST_BF_ENTRY_PACKET_RUST -o $@ $(PACKET_SRC) librust_bf_entry_packet.a

librust_bf_entry_packet.a: $(RUST_BF)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_bf_entry_packet_test \
		--crate-type staticlib $(RUST_BF) -o $@

librust_bf_init_cmd.a: $(RUST_BF)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_bf_entry_packet_test \
		--cfg host_bf_init_cmd_test --crate-type staticlib $(RUST_BF) -o $@

clean:
	rm -f test_bf_entry_packet test_bf_entry_packet_rust test_bf_packet_leaf \
		test_bf_packet_leaf_rust test_bf_init_cmd_leaf test_bf_init_cmd_leaf_rust \
		librust_bf_entry_packet.a librust_bf_init_cmd.a

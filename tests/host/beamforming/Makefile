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
CMD_TRAFFIC_SRC := bf_cmd_traffic_l2.c
CMD_TRAFFIC_SHIM := host_beamforming_bf_cmd_traffic_shim.c
CMD_TRAFFIC_JSON := bf_cmd_traffic_vectors.json
HOST_JSON := ../crypto/host_vector_json.c

.PHONY: all test test-c test-rust test-packet test-init-cmd test-init-cmd-rust test-gid \
	test-cmd-traffic clean
all: test

test: test-c test-rust test-packet test-init-cmd test-init-cmd-rust test-gid test-cmd-traffic

test-rust: test_bf_entry_packet_rust
	./test_bf_entry_packet_rust

test-c: test_bf_entry_packet
	./test_bf_entry_packet

test-packet: test_bf_packet_leaf
	./test_bf_packet_leaf

test-init-cmd: test_bf_init_cmd_leaf
	./test_bf_init_cmd_leaf

test-init-cmd-rust: test_bf_init_cmd_leaf_rust
	./test_bf_init_cmd_leaf_rust

test-gid: test_bf_gid_leaf
	./test_bf_gid_leaf

test-cmd-traffic: test_bf_cmd_traffic
	./test_bf_cmd_traffic $(CMD_TRAFFIC_JSON)

test_bf_entry_packet: $(ENTRY_SRC)
	$(CC) $(CFLAGS) -o $@ $(ENTRY_SRC)

test_bf_entry_packet_rust: $(ENTRY_SRC) librust_bf_entry_packet.a
	$(CC) $(CFLAGS) -DHOST_BF_ENTRY_PACKET_RUST -o $@ $(ENTRY_SRC) librust_bf_entry_packet.a

test_bf_packet_leaf: $(PACKET_SRC)
	$(CC) $(CFLAGS) -o $@ $(PACKET_SRC)

test_bf_init_cmd_leaf: $(INIT_CMD_SRC)
	$(CC) $(CFLAGS) -o $@ $(INIT_CMD_SRC)

test_bf_init_cmd_leaf_rust: $(INIT_CMD_SRC) librust_bf_init_cmd.a
	$(CC) $(CFLAGS) -DHOST_BF_INIT_CMD_RUST -o $@ $(INIT_CMD_SRC) librust_bf_init_cmd.a

test_bf_gid_leaf: $(GID_SRC)
	$(CC) $(CFLAGS) -o $@ $(GID_SRC)

test_bf_cmd_traffic: $(CMD_TRAFFIC_SRC) $(CMD_TRAFFIC_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ $(CMD_TRAFFIC_SRC) $(CMD_TRAFFIC_SHIM) $(HOST_JSON)

librust_bf_entry_packet.a: $(RUST_BF)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_bf_entry_packet_test \
		--crate-type staticlib $(RUST_BF) -o $@

librust_bf_init_cmd.a: $(RUST_BF)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_bf_entry_packet_test \
		--cfg host_bf_init_cmd_test --crate-type staticlib $(RUST_BF) -o $@

clean:
	rm -f test_bf_entry_packet test_bf_entry_packet_rust test_bf_packet_leaf \
		test_bf_init_cmd_leaf test_bf_init_cmd_leaf_rust test_bf_gid_leaf \
		test_bf_cmd_traffic \
		librust_bf_entry_packet.a librust_bf_init_cmd.a

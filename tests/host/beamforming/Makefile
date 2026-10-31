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
SOUND_ENTER_SRC := bf_sound_enter_l2.c
SOUND_LEAVE_SRC := bf_sound_leave_l2.c
RUST_SOUND := $(ROOT)/rust/rtw_beamforming_sound.rs
HOST_JSON := ../crypto/host_vector_json.c

.PHONY: all test test-c test-rust test-packet test-init-cmd test-init-cmd-rust test-gid \
	test-cmd-traffic test-sound-enter test-sound-enter-rust test-sound-leave test-sound-leave-rust clean
all: test

test: test-c test-rust test-packet test-init-cmd test-init-cmd-rust test-gid test-cmd-traffic \
	test-cmd-traffic-rust test-sound-enter test-sound-enter-rust test-sound-leave test-sound-leave-rust

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

test-cmd-traffic-rust: test_bf_cmd_traffic_rust
	./test_bf_cmd_traffic_rust $(CMD_TRAFFIC_JSON)

test-sound-enter: test_bf_sound_enter
	./test_bf_sound_enter

test-sound-enter-rust: test_bf_sound_enter_rust
	./test_bf_sound_enter_rust

test-sound-leave: test_bf_sound_leave
	./test_bf_sound_leave

test-sound-leave-rust: test_bf_sound_leave_rust
	./test_bf_sound_leave_rust

test_bf_sound_leave: $(SOUND_LEAVE_SRC)
	$(CC) $(CFLAGS) -o $@ $(SOUND_LEAVE_SRC)

test_bf_sound_leave_rust: $(SOUND_LEAVE_SRC) librust_bf_sound_leave.a
	$(CC) $(CFLAGS) -DHOST_BF_SOUND_LEAVE_RUST -o $@ $(SOUND_LEAVE_SRC) librust_bf_sound_leave.a

librust_bf_sound_leave.a: $(RUST_SOUND)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_bf_sound_leave_test \
		--crate-type staticlib $(RUST_SOUND) -o $@

test_bf_sound_enter: $(SOUND_ENTER_SRC)
	$(CC) $(CFLAGS) -o $@ $(SOUND_ENTER_SRC)

test_bf_sound_enter_rust: $(SOUND_ENTER_SRC) librust_bf_sound_enter.a
	$(CC) $(CFLAGS) -DHOST_BF_SOUND_ENTER_RUST -o $@ $(SOUND_ENTER_SRC) librust_bf_sound_enter.a

librust_bf_sound_enter.a: $(RUST_SOUND)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_bf_sound_enter_test \
		--crate-type staticlib $(RUST_SOUND) -o $@

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

librust_bf_cmd_traffic.a: $(ROOT)/rust/rtw_beamforming_cmd.rs
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_bf_cmd_traffic_test \
		--crate-type staticlib $(ROOT)/rust/rtw_beamforming_cmd.rs -o $@

test_bf_cmd_traffic_rust: $(CMD_TRAFFIC_SRC) $(CMD_TRAFFIC_SHIM) librust_bf_cmd_traffic.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_BF_CMD_TRAFFIC_RUST -o $@ $(CMD_TRAFFIC_SRC) $(CMD_TRAFFIC_SHIM) \
		$(HOST_JSON) librust_bf_cmd_traffic.a

librust_bf_entry_packet.a: $(RUST_BF)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_bf_entry_packet_test \
		--crate-type staticlib $(RUST_BF) -o $@

librust_bf_init_cmd.a: $(RUST_BF)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_bf_entry_packet_test \
		--cfg host_bf_init_cmd_test --crate-type staticlib $(RUST_BF) -o $@

clean:
	rm -f test_bf_entry_packet test_bf_entry_packet_rust test_bf_packet_leaf \
		test_bf_init_cmd_leaf test_bf_init_cmd_leaf_rust test_bf_gid_leaf \
		test_bf_cmd_traffic test_bf_cmd_traffic_rust test_bf_sound_enter test_bf_sound_enter_rust \
		test_bf_sound_leave test_bf_sound_leave_rust \
		librust_bf_entry_packet.a librust_bf_init_cmd.a librust_bf_cmd_traffic.a \
		librust_bf_sound_enter.a librust_bf_sound_leave.a

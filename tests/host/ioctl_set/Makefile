ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
RUST_IOCTL := $(ROOT)/rust/rtw_ioctl_set.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)

.PHONY: all test test-c test-rust clean
all: test

test: test-c test-rust

test-c: test_ioctl_validate test_ioctl_scan_channel test_ioctl_connect test_ioctl_regd \
	test_ioctl_bssid_scan
	./test_ioctl_validate ioctl_validate_vectors.json
	./test_ioctl_scan_channel ioctl_scan_channel_vectors.json
	./test_ioctl_connect ioctl_connect_vectors.json
	./test_ioctl_regd ioctl_regd_vectors.json
	./test_ioctl_bssid_scan ioctl_bssid_scan_vectors.json

test-rust: test_ioctl_validate_rust test_ioctl_scan_channel_rust test_ioctl_connect_rust \
	test_ioctl_regd_rust test_ioctl_bssid_scan_rust
	./test_ioctl_validate_rust ioctl_validate_vectors.json
	./test_ioctl_scan_channel_rust ioctl_scan_channel_vectors.json
	./test_ioctl_connect_rust ioctl_connect_vectors.json
	./test_ioctl_regd_rust ioctl_regd_vectors.json
	./test_ioctl_bssid_scan_rust ioctl_bssid_scan_vectors.json

test_ioctl_validate: ioctl_validate_l2.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ ioctl_validate_l2.c $(HOST_JSON)

test_ioctl_scan_channel: ioctl_scan_channel_l2.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ ioctl_scan_channel_l2.c $(HOST_JSON)

test_ioctl_connect: ioctl_connect_l2.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ ioctl_connect_l2.c $(HOST_JSON)

test_ioctl_regd: ioctl_regd_l2.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ ioctl_regd_l2.c $(HOST_JSON)

test_ioctl_bssid_scan: ioctl_bssid_scan_l2.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ ioctl_bssid_scan_l2.c $(HOST_JSON)

test_ioctl_validate_rust: ioctl_validate_l2.c librust_ioctl_validate.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_IOCTL_VALIDATE_RUST -o $@ ioctl_validate_l2.c $(HOST_JSON) librust_ioctl_validate.a

test_ioctl_scan_channel_rust: ioctl_scan_channel_l2.c librust_ioctl_scan_channel.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_IOCTL_SCAN_CHANNEL_RUST -o $@ ioctl_scan_channel_l2.c $(HOST_JSON) librust_ioctl_scan_channel.a

test_ioctl_connect_rust: ioctl_connect_l2.c librust_ioctl_connect.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_IOCTL_CONNECT_RUST -o $@ ioctl_connect_l2.c $(HOST_JSON) librust_ioctl_connect.a

test_ioctl_regd_rust: ioctl_regd_l2.c librust_ioctl_regd.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_IOCTL_REGD_RUST -o $@ ioctl_regd_l2.c $(HOST_JSON) librust_ioctl_regd.a

test_ioctl_bssid_scan_rust: ioctl_bssid_scan_l2.c librust_ioctl_bssid_scan.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_IOCTL_BSSID_SCAN_RUST -o $@ ioctl_bssid_scan_l2.c $(HOST_JSON) \
		librust_ioctl_bssid_scan.a

librust_ioctl_validate.a: $(RUST_IOCTL)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_ioctl_validate_test \
		--crate-type staticlib $(RUST_IOCTL) -o $@

librust_ioctl_scan_channel.a: $(RUST_IOCTL)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_ioctl_scan_channel_test \
		--crate-type staticlib $(RUST_IOCTL) -o $@

librust_ioctl_connect.a: $(RUST_IOCTL)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_ioctl_connect_test \
		--crate-type staticlib $(RUST_IOCTL) -o $@

librust_ioctl_regd.a: $(RUST_IOCTL)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_ioctl_regd_test \
		--crate-type staticlib $(RUST_IOCTL) -o $@

librust_ioctl_bssid_scan.a: $(RUST_IOCTL)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_ioctl_bssid_scan_test \
		--crate-type staticlib $(RUST_IOCTL) -o $@

clean:
	rm -f test_ioctl_validate test_ioctl_validate_rust test_ioctl_scan_channel \
		test_ioctl_scan_channel_rust test_ioctl_connect test_ioctl_connect_rust \
		test_ioctl_regd test_ioctl_regd_rust test_ioctl_bssid_scan test_ioctl_bssid_scan_rust \
		librust_ioctl_validate.a librust_ioctl_scan_channel.a librust_ioctl_connect.a \
		librust_ioctl_regd.a librust_ioctl_bssid_scan.a

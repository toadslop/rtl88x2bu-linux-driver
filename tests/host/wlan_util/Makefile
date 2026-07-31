# Host L2 differential tests for core/rtw_wlan_util.c helpers (T5).
# Builds with gcc only — no kernel headers or KDIR.

include ../rust.mk

ROOT     := ../../..
HOST_INC := ../include
ORACLE   := wlan_util_c_oracle.c
RUST_WLAN := $(ROOT)/rust/rtw_wlan_util.rs

CC      := gcc
RUSTC   := rustc
CFLAGS  := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
           -I$(HOST_INC) -I.
RUSTFLAGS := $(RUST_BASE_FLAGS)
HOST_JSON := ../crypto/host_vector_json.c

.PHONY: all test test-c test-rust test-ratetbl clean

all: test

test: test-c test-rust test-ratetbl test-ratetbl-rust test-tail test-tail-rust

test-c: test_wlan_util
	./test_wlan_util rate_class_vectors.json

test-rust: test_wlan_util_rust
	./test_wlan_util_rust rate_class_vectors.json

test-ratetbl: test_ratetbl
	./test_ratetbl ratetbl_vectors.json

test-ratetbl-rust: test_ratetbl_rust
	./test_ratetbl_rust ratetbl_vectors.json

test_wlan_util: test_wlan_util.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_wlan_util.c $(ORACLE) $(HOST_JSON)

librust_wlan_util.a: $(RUST_WLAN) $(ROOT)/rust/domain/types.rs
	$(RUSTC) $(RUSTFLAGS) --cfg host_wlan_util_test --crate-type staticlib \
		$(RUST_WLAN) -o $@

test_wlan_util_rust: test_wlan_util.c librust_wlan_util.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_WLAN_UTIL_ORACLE -o $@ test_wlan_util.c $(HOST_JSON) \
		librust_wlan_util.a

test_ratetbl: test_ratetbl.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_ratetbl.c $(ORACLE) $(HOST_JSON)

test_ratetbl_rust: test_ratetbl.c librust_wlan_util.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_WLAN_UTIL_ORACLE -o $@ test_ratetbl.c $(HOST_JSON) \
		librust_wlan_util.a

test-tail: test_wlan_tail
	./test_wlan_tail wlan_tail_vectors.json

test-tail-rust: test_wlan_tail_rust
	./test_wlan_tail_rust wlan_tail_vectors.json

test_wlan_tail: test_wlan_tail.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_wlan_tail.c $(ORACLE) $(HOST_JSON)

test_wlan_tail_rust: test_wlan_tail.c librust_wlan_util.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_WLAN_UTIL_ORACLE -o $@ test_wlan_tail.c $(HOST_JSON) \
		librust_wlan_util.a

clean:
	rm -f test_wlan_util test_wlan_util_rust test_ratetbl test_ratetbl_rust \
		test_wlan_tail test_wlan_tail_rust librust_wlan_util.a

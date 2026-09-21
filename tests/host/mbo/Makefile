ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
ORACLE := mbo_ie_c_oracle.c
RUST_MBO := $(ROOT)/rust/rtw_mbo.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -I$(ROOT)/include

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_mbo_ie
	./test_mbo_ie mbo_ie_vectors.json

test-rust: test_mbo_ie_rust
	./test_mbo_ie_rust mbo_ie_vectors.json

test_mbo_ie: test_mbo_ie.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_mbo_ie.c $(ORACLE) $(HOST_JSON)

librust_mbo.a: $(RUST_MBO)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_mbo_test \
		--crate-type staticlib $(RUST_MBO) -o $@

test_mbo_ie_rust: test_mbo_ie.c librust_mbo.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_mbo_ie.c $(HOST_JSON) librust_mbo.a

clean:
	rm -f test_mbo_ie test_mbo_ie_rust librust_mbo.a

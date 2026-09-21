ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
ORACLE := wnm_btm_c_oracle.c
RUST_WNM := $(ROOT)/rust/rtw_wnm.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -I$(ROOT)/include

.PHONY: all test test-c test-rust-rustc clean

all: test

test: test-c

test-c: test_wnm_btm
	./test_wnm_btm wnm_btm_vectors.json

test-rust-rustc: librust_wnm.a

test_wnm_btm: test_wnm_btm.c $(ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_wnm_btm.c $(ORACLE) $(HOST_JSON)

librust_wnm.a: $(RUST_WNM)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_wnm_test \
		--crate-type staticlib $(RUST_WNM) -o $@

clean:
	rm -f test_wnm_btm test_wnm_btm_rust librust_wnm.a

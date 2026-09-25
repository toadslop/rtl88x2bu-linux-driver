ROOT := ../../..
HOST_INC := ../include
HOST_JSON := ../crypto/host_vector_json.c
RUST_MI := $(ROOT)/rust/rtw_mi.rs
RUSTC := rustc
CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC)

.PHONY: all test test-c test-rust clean
all: test

test: test-c test-rust

test-c: test_mi_ch_union
	./test_mi_ch_union mi_ch_union_vectors.json

test-rust: test_mi_ch_union_rust
	./test_mi_ch_union_rust mi_ch_union_vectors.json

test_mi_ch_union: mi_ch_union_l2.c $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ mi_ch_union_l2.c $(HOST_JSON)

librust_mi.a: $(RUST_MI)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_mi_ch_union_test \
		--crate-type staticlib $(RUST_MI) -o $@

test_mi_ch_union_rust: mi_ch_union_l2.c librust_mi.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DHOST_MI_RUST -o $@ mi_ch_union_l2.c $(HOST_JSON) librust_mi.a

clean:
	rm -f test_mi_ch_union test_mi_ch_union_rust librust_mi.a

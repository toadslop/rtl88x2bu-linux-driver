# Host L2 differential tests for core/rtw_io_rest.c (W3-18).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
IO_SRC   := $(CORE)/rtw_io_rest.c
HOST_JSON := ../crypto/host_vector_json.c
RUST_IO_REST := $(ROOT)/rust/rtw_io_rest.rs

CC     := gcc
RUSTC  := rustc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_IO_TEST -DDBG_IO -DDBG_IO_SNIFF_EXAMPLE

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_io
	./test_io io_vectors.json

test_io: test_io.c $(IO_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_io.c $(IO_SRC) $(HOST_JSON)

librust_io_rest.a: $(RUST_IO_REST)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_io_rest_test --cfg dbg_io \
		--crate-type staticlib $(RUST_IO_REST) -o $@

test-rust: test_io_rust
	./test_io_rust io_vectors.json

test_io_rust: test_io.c librust_io_rest.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_IO_REST_ORACLE -o $@ test_io.c $(HOST_JSON) librust_io_rest.a

clean:
	rm -f test_io test_io_rust librust_io_rest.a

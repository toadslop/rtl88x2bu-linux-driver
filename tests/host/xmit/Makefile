# Host L2 differential tests for core/rtw_xmit_rest.c (W3-40).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
XMIT_REST_SRC := $(CORE)/rtw_xmit_rest.c
HOST_JSON := ../crypto/host_vector_json.c
SHIM     := host_xmit_shim.c
RUST_XMIT := $(ROOT)/rust/rtw_xmit.rs

CC     := gcc
RUSTC  := rustc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_XMIT_TEST

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_xmit
	./test_xmit xmit_vectors.json

test_xmit: test_xmit.c $(XMIT_REST_SRC) $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_xmit.c $(XMIT_REST_SRC) $(SHIM) $(HOST_JSON)

librust_xmit.a: $(RUST_XMIT)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_xmit_test \
		--crate-type staticlib $(RUST_XMIT) -o $@

test-rust: test_xmit_rust
	./test_xmit_rust xmit_vectors.json

test_xmit_rust: test_xmit.c librust_xmit.a $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_xmit.c $(HOST_JSON) librust_xmit.a

clean:
	rm -f test_xmit test_xmit_rust librust_xmit.a xmit_rest_c_ref.o

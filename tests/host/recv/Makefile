# Host L2 differential tests for core/rtw_recv_rest.c (W3-39).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
RECV_REST_SRC := $(CORE)/rtw_recv_rest.c
WFD_ORACLE := recv_wfd_c_oracle.c
HOST_JSON := ../crypto/host_vector_json.c
RUST_RECV := $(ROOT)/rust/rtw_recv.rs

CC     := gcc
RUSTC  := rustc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_RECV_TEST -DHOST_RECV_WFD_TEST

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_recv
	./test_recv recv_vectors.json

test_recv: test_recv.c $(RECV_REST_SRC) $(WFD_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_recv.c $(RECV_REST_SRC) $(WFD_ORACLE) $(HOST_JSON)

librust_recv.a: $(RUST_RECV)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_recv_test \
		--crate-type staticlib $(RUST_RECV) -o $@

test-rust: test_recv_rust
	./test_recv_rust recv_vectors.json

test_recv_rust: test_recv.c librust_recv.a $(WFD_ORACLE) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_recv.c $(WFD_ORACLE) $(HOST_JSON) librust_recv.a

clean:
	rm -f test_recv test_recv_rust librust_recv.a

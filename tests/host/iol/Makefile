# Host L2 differential tests for core/rtw_iol_rest.c (W3-50).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
IOL_REST_SRC := $(CORE)/rtw_iol_rest.c
HOST_JSON := ../crypto/host_vector_json.c
RUST_IOL_REST := $(ROOT)/rust/rtw_iol_rest.rs

CC     := gcc
RUSTC  := rustc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_IOL_TEST

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_iol
	./test_iol iol_vectors.json

test_iol: test_iol.c $(IOL_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_iol.c $(IOL_REST_SRC) $(HOST_JSON)

librust_iol_rest.a: $(RUST_IOL_REST)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_iol_test \
		--crate-type staticlib $(RUST_IOL_REST) -o $@

test-rust: test_iol_rust
	./test_iol_rust iol_vectors.json

test_iol_rust: test_iol.c librust_iol_rest.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_IOL_ORACLE -o $@ test_iol.c $(HOST_JSON) librust_iol_rest.a

clean:
	rm -f test_iol test_iol_rust librust_iol_rest.a iol_rest_c_ref.o

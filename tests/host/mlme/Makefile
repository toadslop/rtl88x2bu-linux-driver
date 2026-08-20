# Host L2 differential tests for core/rtw_mlme_rest.c (W3-53).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
MLME_REST_SRC := $(CORE)/rtw_mlme_rest.c
MLME_SHIM := host_mlme_shim.c
HOST_JSON := ../crypto/host_vector_json.c
RUST_MLME_REST := $(ROOT)/rust/rtw_mlme_rest.rs

CC     := gcc
RUSTC  := rustc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -O2 \
          -I$(HOST_INC) -I$(CORE) \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_MLME_TEST

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_mlme
	./test_mlme mlme_vectors.json

test_mlme: test_mlme.c $(MLME_REST_SRC) $(MLME_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_mlme.c $(MLME_REST_SRC) $(MLME_SHIM) $(HOST_JSON)

librust_mlme_rest.a: $(RUST_MLME_REST)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_mlme_test \
		--crate-type staticlib $(RUST_MLME_REST) -o $@

test-rust: test_mlme_rust
	./test_mlme_rust mlme_vectors.json

test_mlme_rust: test_mlme.c librust_mlme_rest.a $(MLME_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_MLME_ORACLE -o $@ test_mlme.c $(MLME_SHIM) $(HOST_JSON) librust_mlme_rest.a

clean:
	rm -f test_mlme test_mlme_rust librust_mlme_rest.a

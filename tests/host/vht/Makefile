# Host L2 differential tests for core/rtw_vht_rest.c (W3-35).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
VHT_REST_SRC := $(CORE)/rtw_vht_rest.c
HOST_JSON := ../crypto/host_vector_json.c
RUST_VHT := $(ROOT)/rust/rtw_vht.rs

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_VHT_TEST -DROKU_PRIVATE

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_vht
	./test_vht vht_vectors.json

test_vht: test_vht.c $(VHT_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_vht.c $(VHT_REST_SRC) $(HOST_JSON)

librust_vht.a: $(RUST_VHT)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_vht_test \
		--crate-type staticlib $(RUST_VHT) -o $@

test-rust: test_vht_rust
	./test_vht_rust vht_vectors.json

test_vht_rust: test_vht.c librust_vht.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_VHT_ORACLE -o $@ test_vht.c $(HOST_JSON) librust_vht.a

clean:
	rm -f test_vht test_vht_rust librust_vht.a

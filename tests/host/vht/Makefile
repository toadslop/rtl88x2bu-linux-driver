# Host L2 differential tests for core/rtw_vht_rest.c (W3-35, W3-36).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
VHT_REST_SRC := $(CORE)/rtw_vht_rest.c
VHT_REST_SHIM := host_vht_rest_shim.c
HOST_JSON := ../crypto/host_vector_json.c
RUST_VHT := $(ROOT)/rust/rtw_vht.rs

CC     := gcc
BASE_CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DROKU_PRIVATE
CFLAGS := $(BASE_CFLAGS) -DHOST_VHT_TEST
RESTRUCT_CFLAGS := $(BASE_CFLAGS) -DHOST_VHT_RESTRUCTURE_TEST -Wno-pointer-sign

.PHONY: all test test-c test-rust vht-restructure-compile test-restructure clean

all: test

test: test-c test-rust test-restructure

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

vht-restructure-compile: $(VHT_REST_SRC) $(VHT_REST_SHIM)
	$(CC) $(RESTRUCT_CFLAGS) -c $(VHT_REST_SRC) -o vht_rest_host.o
	$(CC) $(RESTRUCT_CFLAGS) -c $(VHT_REST_SHIM) -o vht_rest_shim_host.o

test-restructure: test_vht_restructure
	./test_vht_restructure vht_restructure_vectors.json

test_vht_restructure: test_vht_restructure.c $(VHT_REST_SRC) $(VHT_REST_SHIM) $(HOST_JSON)
	$(CC) $(RESTRUCT_CFLAGS) -o $@ test_vht_restructure.c $(VHT_REST_SRC) $(VHT_REST_SHIM) $(HOST_JSON)

clean:
	rm -f test_vht test_vht_rust test_vht_restructure librust_vht.a vht_rest_host.o vht_rest_shim_host.o

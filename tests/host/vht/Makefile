# Host L2 differential tests for core/rtw_vht_rest.c (W3-35, W3-36).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
VHT_REST_SRC := $(CORE)/rtw_vht_rest.c
VHT_REST_SHIM := host_vht_rest_shim.c
HOST_JSON := ../crypto/host_vector_json.c
VHT_MCS_RATE_ORACLE := vht_mcs_rate_c_oracle.c
RUST_VHT := $(ROOT)/rust/rtw_vht.rs

CC     := gcc
BASE_CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DROKU_PRIVATE
CFLAGS := $(BASE_CFLAGS) -DHOST_VHT_TEST
MCS_RATE_CFLAGS := $(BASE_CFLAGS) -DHOST_VHT_MCS_RATE_TEST
RESTRUCT_CFLAGS := $(BASE_CFLAGS) -DHOST_VHT_RESTRUCTURE_TEST -Wno-pointer-sign

.PHONY: all test test-c test-rust test-mcs-rate test-mcs-rate-rust \
	vht-restructure-compile test-restructure test-restructure-rust clean

all: test

test: test-c test-rust test-mcs-rate test-restructure test-restructure-rust

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

test-mcs-rate: test_vht_mcs_rate
	./test_vht_mcs_rate vht_mcs_rate_vectors.json

test_vht_mcs_rate: test_vht_mcs_rate.c $(VHT_MCS_RATE_ORACLE) $(HOST_JSON)
	$(CC) $(MCS_RATE_CFLAGS) -o $@ test_vht_mcs_rate.c $(VHT_MCS_RATE_ORACLE) $(HOST_JSON)

vht-restructure-compile: $(VHT_REST_SRC) $(VHT_REST_SHIM)
	$(CC) $(RESTRUCT_CFLAGS) -c $(VHT_REST_SRC) -o vht_rest_host.o
	$(CC) $(RESTRUCT_CFLAGS) -c $(VHT_REST_SHIM) -o vht_rest_shim_host.o

librust_vht_restructure.a: $(RUST_VHT)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_vht_restructure_test \
		--crate-type staticlib $(RUST_VHT) -o $@

test-restructure-rust: test_vht_restructure_rust
	./test_vht_restructure_rust vht_restructure_vectors.json

test_vht_restructure_rust: test_vht_restructure.c librust_vht_restructure.a $(VHT_REST_SHIM) $(HOST_JSON)
	$(CC) $(RESTRUCT_CFLAGS) -DRUST_VHT_ORACLE -o $@ test_vht_restructure.c $(VHT_REST_SHIM) $(HOST_JSON) librust_vht_restructure.a

test-restructure: test_vht_restructure
	./test_vht_restructure vht_restructure_vectors.json

test_vht_restructure: test_vht_restructure.c $(VHT_REST_SRC) $(VHT_REST_SHIM) $(HOST_JSON)
	$(CC) $(RESTRUCT_CFLAGS) -o $@ test_vht_restructure.c $(VHT_REST_SRC) $(VHT_REST_SHIM) $(HOST_JSON)

clean:
	rm -f test_vht test_vht_rust test_vht_mcs_rate test_vht_restructure test_vht_restructure_rust \
		librust_vht.a librust_vht_restructure.a vht_rest_host.o vht_rest_shim_host.o

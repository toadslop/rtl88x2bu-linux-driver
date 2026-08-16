# Host L2 differential tests for core/rtw_xmit_rest.c (W3-40) and
# core/rtw_xmit_qos_rest.c (W3-48).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
XMIT_REST_SRC := $(CORE)/rtw_xmit_rest.c
XMIT_QOS_REST_SRC := $(CORE)/rtw_xmit_qos_rest.c
HOST_JSON := ../crypto/host_vector_json.c
SHIM     := host_xmit_shim.c
RUST_XMIT := $(ROOT)/rust/rtw_xmit.rs

CC     := gcc
RUSTC  := rustc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_XMIT_TEST

QOS_CFLAGS := $(CFLAGS) -DHOST_XMIT_QOS_TEST

.PHONY: all test test-c test-rust test-qos clean

all: test test-qos

test: test-c test-rust

test-qos: test-qos-c test-qos-rust

test-c: test_xmit
	./test_xmit xmit_vectors.json

test_xmit: test_xmit.c $(XMIT_REST_SRC) $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_xmit.c $(XMIT_REST_SRC) $(SHIM) $(HOST_JSON)

librust_xmit.a: $(RUST_XMIT)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_xmit_test --cfg host_xmit_qos_test \
		--crate-type staticlib $(RUST_XMIT) -o $@

librust_xmit_qos.a: $(RUST_XMIT)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_xmit_test --cfg host_xmit_qos_test \
		--crate-type staticlib $(RUST_XMIT) -o $@

test-rust: test_xmit_rust
	./test_xmit_rust xmit_vectors.json

test_xmit_rust: test_xmit.c librust_xmit.a $(SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_xmit.c $(SHIM) $(HOST_JSON) librust_xmit.a

test-qos-c: test_xmit_qos
	./test_xmit_qos xmit_qos_vectors.json

test_xmit_qos: test_xmit_qos.c $(XMIT_QOS_REST_SRC) $(HOST_JSON)
	$(CC) $(QOS_CFLAGS) -o $@ test_xmit_qos.c $(XMIT_QOS_REST_SRC) $(HOST_JSON)

test-qos-rust: test_xmit_qos_rust
	./test_xmit_qos_rust xmit_qos_vectors.json

test_xmit_qos_rust: test_xmit_qos.c librust_xmit_qos.a $(HOST_JSON)
	$(CC) $(QOS_CFLAGS) -o $@ test_xmit_qos.c $(HOST_JSON) librust_xmit_qos.a

clean:
	rm -f test_xmit test_xmit_rust test_xmit_qos test_xmit_qos_rust \
		librust_xmit.a librust_xmit_qos.a xmit_rest_c_ref.o xmit_qos_c_ref.o

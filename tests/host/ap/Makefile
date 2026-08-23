# Host L2 differential tests for core/rtw_ap_rest.c (W3-55).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
AP_REST_SRC := $(CORE)/rtw_ap_rest.c
HOST_JSON := ../crypto/host_vector_json.c
RUST_AP_REST := $(ROOT)/rust/rtw_ap_rest.rs

CC     := gcc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_AP_REST_TEST -DCONFIG_FW_HANDLE_TXBCN -DCONFIG_LIMITED_AP_NUM=4

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_ap_rest
	./test_ap_rest ap_rest_vectors.json

test_ap_rest: test_ap_rest.c $(AP_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_ap_rest.c $(AP_REST_SRC) $(HOST_JSON)

librust_ap_rest.a: $(RUST_AP_REST)
	rustc -C opt-level=2 -C overflow-checks=on --cfg host_ap_rest_test --cfg fw_handle_txbcn \
		--crate-type staticlib $(RUST_AP_REST) -o $@

test-rust: test_ap_rest_rust
	./test_ap_rest_rust ap_rest_vectors.json

test_ap_rest_rust: test_ap_rest.c librust_ap_rest.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_AP_REST_ORACLE -o $@ test_ap_rest.c $(HOST_JSON) librust_ap_rest.a

clean:
	rm -f test_ap_rest test_ap_rest_rust librust_ap_rest.a

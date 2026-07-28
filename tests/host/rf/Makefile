# Host L2 differential tests for core/rtw_rf_rest.c (W3-19).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
RF_SRC   := $(CORE)/rtw_rf_rest.c
HOST_JSON := ../crypto/host_vector_json.c
RUST_RF_REST := $(ROOT)/rust/rtw_rf_rest.rs

CC     := gcc
RUSTC  := rustc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_RF_TEST

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_rf
	./test_rf rf_vectors.json

test_rf: test_rf.c $(RF_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rf.c $(RF_SRC) $(HOST_JSON)

librust_rf_rest.a: $(RUST_RF_REST)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_rf_rest_test \
		--crate-type staticlib $(RUST_RF_REST) -o $@

test-rust: test_rf_rust
	./test_rf_rust rf_vectors.json

test_rf_rust: test_rf.c librust_rf_rest.a $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_RF_REST_ORACLE -o $@ test_rf.c $(HOST_JSON) librust_rf_rest.a

clean:
	rm -f test_rf test_rf_rust librust_rf_rest.a

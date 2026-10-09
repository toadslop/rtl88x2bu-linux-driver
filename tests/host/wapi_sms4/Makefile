# Host L2 differential harness for core/rtw_wapi_sms4.c SMS4 block helpers (T17).

ROOT := ../../..
HOST_INC := ../include
ORACLE := wapi_sms4_c_oracle.c
RUST_SMS4 := $(ROOT)/rust/rtw_wapi_sms4.rs

CC := gcc
RUSTC := rustc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -DWAPI_SMS4_FULL_VECTORS
RUSTFLAGS := -C opt-level=2 -C overflow-checks=on

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_wapi_sms4
	./test_wapi_sms4

test-rust: test_wapi_sms4_rust
	./test_wapi_sms4_rust

test_wapi_sms4: test_wapi_sms4.c $(ORACLE)
	$(CC) $(CFLAGS) -o $@ test_wapi_sms4.c $(ORACLE)

librust_wapi_sms4.a: $(RUST_SMS4)
	$(RUSTC) $(RUSTFLAGS) --cfg host_wapi_sms4_test --crate-type staticlib \
		$(RUST_SMS4) -o $@

test_wapi_sms4_rust: test_wapi_sms4.c librust_wapi_sms4.a
	$(CC) $(CFLAGS) -DRUST_WAPI_SMS4_ORACLE -o $@ test_wapi_sms4.c librust_wapi_sms4.a

clean:
	rm -f test_wapi_sms4 test_wapi_sms4_rust librust_wapi_sms4.a

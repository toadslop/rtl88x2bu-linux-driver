# Host L2 differential harness for core/rtw_wapi_sms4.c SMS4 block helpers (T17).

ROOT := ../../..
HOST_INC := ../include
ORACLE := wapi_sms4_c_oracle.c
PN_IV_ORACLE := wapi_sms4_pn_iv_c_oracle.c
OFB_ORACLE := wapi_sms4_ofb_c_oracle.c
RUST_SMS4 := $(ROOT)/rust/rtw_wapi_sms4.rs

CC := gcc
RUSTC := rustc
CFLAGS := -Wall -Wextra -Werror -O2 -I$(HOST_INC) -DWAPI_SMS4_FULL_VECTORS
RUSTFLAGS := -C opt-level=2 -C overflow-checks=on

.PHONY: all test test-c test-rust test-pn-iv test-pn-iv-c test-pn-iv-rust \
	test-ofb test-ofb-c test-ofb-rust clean

all: test test-pn-iv test-ofb

test: test-c test-rust

test-pn-iv: test-pn-iv-c test-pn-iv-rust

test-ofb: test-ofb-c test-ofb-rust

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

test_wapi_sms4_pn_iv: test_wapi_sms4_pn_iv.c $(PN_IV_ORACLE)
	$(CC) $(CFLAGS) -o $@ test_wapi_sms4_pn_iv.c $(PN_IV_ORACLE)

test-pn-iv-c: test_wapi_sms4_pn_iv
	./test_wapi_sms4_pn_iv

test_wapi_sms4_pn_iv_rust: test_wapi_sms4_pn_iv.c librust_wapi_sms4.a
	$(CC) $(CFLAGS) -DRUST_WAPI_PN_IV_ORACLE -o $@ test_wapi_sms4_pn_iv.c librust_wapi_sms4.a

test-pn-iv-rust: test_wapi_sms4_pn_iv_rust
	./test_wapi_sms4_pn_iv_rust

test_wapi_sms4_ofb: test_wapi_sms4_ofb.c $(ORACLE) $(OFB_ORACLE)
	$(CC) $(CFLAGS) -I. -o $@ test_wapi_sms4_ofb.c $(ORACLE) $(OFB_ORACLE)

test-ofb-c: test_wapi_sms4_ofb
	./test_wapi_sms4_ofb

test_wapi_sms4_ofb_rust: test_wapi_sms4_ofb.c librust_wapi_sms4.a
	$(CC) $(CFLAGS) -I. -DRUST_WAPI_SMS4_ORACLE -o $@ test_wapi_sms4_ofb.c librust_wapi_sms4.a

test-ofb-rust: test_wapi_sms4_ofb_rust
	./test_wapi_sms4_ofb_rust

clean:
	rm -f test_wapi_sms4 test_wapi_sms4_rust test_wapi_sms4_pn_iv \
		test_wapi_sms4_pn_iv_rust test_wapi_sms4_ofb test_wapi_sms4_ofb_rust \
		librust_wapi_sms4.a

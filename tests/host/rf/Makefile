# Host L2 differential tests for core/rtw_rf_rest.c (W3-19, W3-51 regd_exc, W3-52 txpwr_lmt).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
RF_SRC   := $(CORE)/rtw_rf_rest.c
HOST_JSON := ../crypto/host_vector_json.c
REGD_EXC_SHIM := host_rf_regd_exc_shim.c
TXPWR_LMT_SHIM := host_rf_txpwr_lmt_shim.c

CC     := gcc
RUSTC  := rustc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -Wno-format-truncation -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_RF_TEST

REGD_EXC_CFLAGS := $(CFLAGS) -DCONFIG_TXPWR_LIMIT -Wno-unused-but-set-variable
TXPWR_LMT_CFLAGS := $(REGD_EXC_CFLAGS)

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_rf test_regd_exc test_txpwr_lmt
	./test_rf rf_vectors.json
	./test_regd_exc regd_exc_vectors.json
	./test_txpwr_lmt txpwr_lmt_vectors.json

test_rf: test_rf.c $(RF_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rf.c $(RF_SRC) $(HOST_JSON)

test_regd_exc: test_regd_exc.c $(RF_SRC) $(REGD_EXC_SHIM) $(TXPWR_LMT_SHIM) $(HOST_JSON)
	$(CC) $(REGD_EXC_CFLAGS) -o $@ test_regd_exc.c $(REGD_EXC_SHIM) $(TXPWR_LMT_SHIM) $(RF_SRC) $(HOST_JSON)

test_txpwr_lmt: test_txpwr_lmt.c $(RF_SRC) $(REGD_EXC_SHIM) $(TXPWR_LMT_SHIM) $(HOST_JSON)
	$(CC) $(TXPWR_LMT_CFLAGS) -o $@ test_txpwr_lmt.c $(REGD_EXC_SHIM) $(TXPWR_LMT_SHIM) $(RF_SRC) $(HOST_JSON)

librust_rf_rest.a: $(ROOT)/rust/rtw_rf_rest.rs
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_rf_rest_test --cfg txpwr_limit --cfg rust_txpwr_lmt \
		--cfg ieee80211_band_5ghz \
		--crate-type staticlib $(ROOT)/rust/rtw_rf_rest.rs -o $@

# PR1: C oracle for txpwr_lmt; defer test_txpwr_lmt_rust to PR2 (Rust port).
test-rust: test_rf_rust test_regd_exc_rust
	./test_rf_rust rf_vectors.json
	./test_regd_exc_rust regd_exc_vectors.json

test_rf_rust: test_rf.c librust_rf_rest.a $(REGD_EXC_SHIM) $(TXPWR_LMT_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_RF_REST_ORACLE -o $@ test_rf.c $(REGD_EXC_SHIM) $(TXPWR_LMT_SHIM) $(HOST_JSON) librust_rf_rest.a

test_regd_exc_rust: test_regd_exc.c librust_rf_rest.a $(REGD_EXC_SHIM) $(HOST_JSON)
	$(CC) $(REGD_EXC_CFLAGS) -DRUST_RF_REGD_EXC_ORACLE -o $@ test_regd_exc.c $(REGD_EXC_SHIM) $(HOST_JSON) librust_rf_rest.a

clean:
	rm -f test_rf test_regd_exc test_txpwr_lmt test_rf_rust test_regd_exc_rust test_txpwr_lmt_rust librust_rf_rest.a

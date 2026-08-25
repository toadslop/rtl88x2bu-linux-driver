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

OPC_PREF_SRC := $(CORE)/rtw_rf_op_class_pref.c
OPC_PREF_SHIM := host_rf_op_class_pref_shim.c
OPC_PREF_CFLAGS := $(CFLAGS) -DHOST_RF_OP_CLASS_PREF_TEST

OPC_DUMP_SRC := $(CORE)/rtw_rf_op_class_dump.c
OPC_DUMP_SHIM := host_rf_op_class_dump_shim.c
OPC_DUMP_CFLAGS := $(CFLAGS) -DHOST_RF_OP_CLASS_DUMP_TEST

DUMP_TXPWR_LMT_SRC := $(CORE)/rtw_rf_dump_txpwr_lmt.c
DUMP_TXPWR_LMT_SHIM := host_rf_dump_txpwr_lmt_shim.c
DUMP_TXPWR_LMT_CFLAGS := $(CFLAGS) -DCONFIG_TXPWR_LIMIT -DHOST_RF_TEST \
	-DHOST_RF_DUMP_TXPWR_LMT_TEST -Wno-format-truncation -Wno-unused-but-set-variable

KFREE_TX_GAIN_SRC := $(CORE)/rtw_rf_kfree_tx_gain.c
KFREE_TX_GAIN_CFLAGS := $(CFLAGS) -DHOST_RF_KFREE_TX_GAIN_TEST -Wno-unused-but-set-variable

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_rf test_regd_exc test_txpwr_lmt test_op_class_pref test_op_class_dump test_dump_txpwr_lmt test_kfree_tx_gain
	./test_rf rf_vectors.json
	./test_regd_exc regd_exc_vectors.json
	./test_txpwr_lmt txpwr_lmt_vectors.json
	./test_op_class_pref op_class_pref_vectors.json
	./test_op_class_dump op_class_dump_vectors.json
	./test_dump_txpwr_lmt dump_txpwr_lmt_vectors.json
	./test_kfree_tx_gain kfree_tx_gain_vectors.json

test_rf: test_rf.c $(RF_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_rf.c $(RF_SRC) $(HOST_JSON)

test_regd_exc: test_regd_exc.c $(RF_SRC) $(REGD_EXC_SHIM) $(TXPWR_LMT_SHIM) $(HOST_JSON)
	$(CC) $(REGD_EXC_CFLAGS) -o $@ test_regd_exc.c $(REGD_EXC_SHIM) $(TXPWR_LMT_SHIM) $(RF_SRC) $(HOST_JSON)

test_txpwr_lmt: test_txpwr_lmt.c $(RF_SRC) $(REGD_EXC_SHIM) $(TXPWR_LMT_SHIM) $(HOST_JSON)
	$(CC) $(TXPWR_LMT_CFLAGS) -o $@ test_txpwr_lmt.c $(REGD_EXC_SHIM) $(TXPWR_LMT_SHIM) $(RF_SRC) $(HOST_JSON)

test_op_class_pref: test_op_class_pref.c $(RF_SRC) $(OPC_PREF_SRC) $(OPC_PREF_SHIM) $(HOST_JSON)
	$(CC) $(OPC_PREF_CFLAGS) -o $@ test_op_class_pref.c $(OPC_PREF_SHIM) $(RF_SRC) $(OPC_PREF_SRC) $(HOST_JSON)

test_op_class_dump: test_op_class_dump.c $(RF_SRC) $(OPC_DUMP_SRC) $(OPC_DUMP_SHIM) $(HOST_JSON)
	$(CC) $(OPC_DUMP_CFLAGS) -o $@ test_op_class_dump.c $(OPC_DUMP_SHIM) $(RF_SRC) $(OPC_DUMP_SRC) $(HOST_JSON)

test_dump_txpwr_lmt: test_dump_txpwr_lmt.c $(RF_SRC) $(DUMP_TXPWR_LMT_SRC) $(DUMP_TXPWR_LMT_SHIM) $(HOST_JSON)
	$(CC) $(DUMP_TXPWR_LMT_CFLAGS) -o $@ test_dump_txpwr_lmt.c $(DUMP_TXPWR_LMT_SHIM) $(RF_SRC) $(DUMP_TXPWR_LMT_SRC) $(HOST_JSON)

test_kfree_tx_gain: test_kfree_tx_gain.c $(RF_SRC) $(KFREE_TX_GAIN_SRC) $(HOST_JSON)
	$(CC) $(KFREE_TX_GAIN_CFLAGS) -o $@ test_kfree_tx_gain.c $(RF_SRC) $(KFREE_TX_GAIN_SRC) $(HOST_JSON)

RUST_RF_KFREE_TX_GAIN := $(ROOT)/rust/rtw_rf_kfree_tx_gain.rs

librust_rf_kfree_tx_gain.a: $(RUST_RF_KFREE_TX_GAIN)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_rf_kfree_tx_gain_test --cfg rf_power_trim \
		--crate-type staticlib $(RUST_RF_KFREE_TX_GAIN) -o $@

test_kfree_tx_gain_rust: test_kfree_tx_gain.c librust_rf_kfree_tx_gain.a $(RF_SRC) $(HOST_JSON)
	$(CC) $(KFREE_TX_GAIN_CFLAGS) -DRUST_RF_KFREE_TX_GAIN_ORACLE -o $@ test_kfree_tx_gain.c $(RF_SRC) $(HOST_JSON) librust_rf_kfree_tx_gain.a

RUST_RF_DUMP_TXPWR_LMT := $(ROOT)/rust/rtw_rf_dump_txpwr_lmt.rs

librust_rf_dump_txpwr_lmt.a: $(RUST_RF_DUMP_TXPWR_LMT)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_rf_dump_txpwr_lmt_test \
		--cfg txpwr_limit --cfg ieee80211_band_5ghz \
		--crate-type staticlib $(RUST_RF_DUMP_TXPWR_LMT) -o $@

test_dump_txpwr_lmt_rust: test_dump_txpwr_lmt.c librust_rf_dump_txpwr_lmt.a $(RF_SRC) $(DUMP_TXPWR_LMT_SHIM) $(HOST_JSON)
	$(CC) $(DUMP_TXPWR_LMT_CFLAGS) -DRUST_RF_DUMP_TXPWR_LMT_ORACLE -o $@ test_dump_txpwr_lmt.c $(DUMP_TXPWR_LMT_SHIM) $(RF_SRC) $(HOST_JSON) librust_rf_dump_txpwr_lmt.a

RUST_RF_OP_CLASS_PREF := $(ROOT)/rust/rtw_rf_op_class_pref.rs

librust_rf_op_class_pref.a: $(RUST_RF_OP_CLASS_PREF)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_rf_op_class_pref_test \
		--crate-type staticlib $(RUST_RF_OP_CLASS_PREF) -o $@

test_op_class_pref_rust: test_op_class_pref.c librust_rf_op_class_pref.a $(RF_SRC) $(OPC_PREF_SHIM) $(HOST_JSON)
	$(CC) $(OPC_PREF_CFLAGS) -o $@ test_op_class_pref.c $(OPC_PREF_SHIM) $(RF_SRC) $(HOST_JSON) librust_rf_op_class_pref.a

librust_rf_op_class_dump.a: $(ROOT)/rust/rtw_rf_op_class_dump.rs
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_rf_op_class_dump_test --cfg config_rtw_debug \
		--crate-type staticlib $(ROOT)/rust/rtw_rf_op_class_dump.rs -o $@

test_op_class_dump_rust: test_op_class_dump.c librust_rf_op_class_dump.a $(RF_SRC) $(OPC_DUMP_SHIM) $(HOST_JSON)
	$(CC) $(OPC_DUMP_CFLAGS) -DRUST_RF_OP_CLASS_DUMP_ORACLE -o $@ test_op_class_dump.c $(OPC_DUMP_SHIM) $(RF_SRC) $(HOST_JSON) librust_rf_op_class_dump.a

librust_rf_rest.a: $(ROOT)/rust/rtw_rf_rest.rs
	$(RUSTC) -C opt-level=2 -C overflow-checks=on --cfg host_rf_rest_test --cfg txpwr_limit --cfg rust_txpwr_lmt \
		--cfg ieee80211_band_5ghz \
		--crate-type staticlib $(ROOT)/rust/rtw_rf_rest.rs -o $@

# PR2: enable Rust txpwr_lmt oracle (stacked on PR1 C harness).
test-rust: test_rf_rust test_regd_exc_rust test_txpwr_lmt_rust test_op_class_pref_rust test_op_class_dump_rust test_dump_txpwr_lmt_rust test_kfree_tx_gain_rust
	./test_rf_rust rf_vectors.json
	./test_regd_exc_rust regd_exc_vectors.json
	./test_txpwr_lmt_rust txpwr_lmt_vectors.json
	./test_op_class_pref_rust op_class_pref_vectors.json
	./test_op_class_dump_rust op_class_dump_vectors.json
	./test_dump_txpwr_lmt_rust dump_txpwr_lmt_vectors.json
	./test_kfree_tx_gain_rust kfree_tx_gain_vectors.json

test_rf_rust: test_rf.c librust_rf_rest.a $(REGD_EXC_SHIM) $(TXPWR_LMT_SHIM) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_RF_REST_ORACLE -o $@ test_rf.c $(REGD_EXC_SHIM) $(TXPWR_LMT_SHIM) $(HOST_JSON) librust_rf_rest.a

test_regd_exc_rust: test_regd_exc.c librust_rf_rest.a $(REGD_EXC_SHIM) $(TXPWR_LMT_SHIM) $(HOST_JSON)
	$(CC) $(REGD_EXC_CFLAGS) -DRUST_RF_REGD_EXC_ORACLE -o $@ test_regd_exc.c $(REGD_EXC_SHIM) $(TXPWR_LMT_SHIM) $(HOST_JSON) librust_rf_rest.a

test_txpwr_lmt_rust: test_txpwr_lmt.c librust_rf_rest.a $(REGD_EXC_SHIM) $(TXPWR_LMT_SHIM) $(HOST_JSON)
	$(CC) $(TXPWR_LMT_CFLAGS) -DRUST_RF_TXPWR_LMT_ORACLE -o $@ test_txpwr_lmt.c $(REGD_EXC_SHIM) $(TXPWR_LMT_SHIM) $(HOST_JSON) librust_rf_rest.a

clean:
	rm -f test_rf test_regd_exc test_txpwr_lmt test_op_class_pref test_op_class_dump test_dump_txpwr_lmt test_kfree_tx_gain test_rf_rust test_regd_exc_rust test_txpwr_lmt_rust test_op_class_pref_rust test_op_class_dump_rust test_dump_txpwr_lmt_rust test_kfree_tx_gain_rust librust_rf_rest.a librust_rf_op_class_pref.a librust_rf_op_class_dump.a librust_rf_dump_txpwr_lmt.a librust_rf_kfree_tx_gain.a

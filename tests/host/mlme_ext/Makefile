# Host L2 differential tests for core/rtw_mlme_ext_rest.c (W3-54 chset).

ROOT     := ../../..
CORE     := $(ROOT)/core
HOST_INC := ../include
MLME_EXT_REST_SRC := $(CORE)/rtw_mlme_ext_rest.c
RF_REST_SRC := $(CORE)/rtw_rf_rest.c
RUST_MLME_EXT_REST := $(ROOT)/rust/rtw_mlme_ext_rest.rs
HOST_JSON := ../crypto/host_vector_json.c
SHIM := host_mlme_ext_shim.c

CC     := gcc
RUSTC  := rustc
CFLAGS := -Wall -Wextra -Werror -Wno-unused-parameter -Wno-format-truncation -O2 \
          -I$(HOST_INC) -I$(CORE) -I$(ROOT)/include \
          -include $(HOST_INC)/host_autoconf.h \
          -DHOST_MLME_EXT_TEST -DHOST_RF_TEST -DCONFIG_DFS_MASTER

.PHONY: all test test-c test-rust clean

all: test

test: test-c test-rust

test-c: test_chset
	./test_chset chset_vectors.json

test_chset: test_chset.c $(MLME_EXT_REST_SRC) $(SHIM) $(RF_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -o $@ test_chset.c $(MLME_EXT_REST_SRC) $(SHIM) \
		$(RF_REST_SRC) $(HOST_JSON)

librust_mlme_ext_rest.a: $(RUST_MLME_EXT_REST)
	$(RUSTC) -C opt-level=2 -C overflow-checks=on \
		--cfg host_mlme_ext_test --cfg dfs_master \
		--crate-type staticlib $(RUST_MLME_EXT_REST) -o $@

test-rust: test_chset_rust
	./test_chset_rust chset_vectors.json

test_chset_rust: test_chset.c librust_mlme_ext_rest.a $(SHIM) $(RF_REST_SRC) $(HOST_JSON)
	$(CC) $(CFLAGS) -DRUST_MLME_EXT_REST_ORACLE -o $@ test_chset.c $(SHIM) \
		$(RF_REST_SRC) $(HOST_JSON) librust_mlme_ext_rest.a

clean:
	rm -f test_chset test_chset_rust librust_mlme_ext_rest.a
